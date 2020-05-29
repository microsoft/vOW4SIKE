/* 
 * Experimental, unused code for writing to remote databases.
 * 
 * POTENTIAL IMPROVEMENTS:
 *		implement batch queries rather than single ones
 *		implement a time mechanism not to get stuck on recvfrom after a read query. It should be possible
 *			with something like https://stackoverflow.com/questions/1824465/set-timeout-for-winsock-recvfrom
 *		write the equivalent networking code for linux. 'c/spacetime_client' has some untidy basic code
 *			see https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html for more
 *		we use unsafe functions like strcpy and scanf. If we open the server to the internet 
 *			(in a database model like breaking ECC2k, we should secure all this or they'll take over!!)
 *			some options (and issues) are pointed out in https://randomascii.wordpress.com/2013/04/03/stop-using-strncpy-already/
 *		we are submitting bytelen info with every r/w query, but this is superfluous since this is fixed server side
 *		    it may be ok to keep it since the server (in a different application) may be used to store values of 
 *          different lengths...
 *		using the database gives a slight variation in number of distinguished points and step functions computed every time...
 *			packets in different order maybe? even in single thread?? or maybe just errors when copying the data from the db..
 */

#include <time.h>
#include <string.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "storage.h"

static inline uint64_t compute_endpoint(storage_state_t *state, uint64_t address)
{
    // Addresses start from 0, so floor division is enough
    return address / state->points_per_db_endpoint;
}

int initialize_storage(
    storage_state_t *state,
    uint64_t address_bitlength,
    uint64_t point_bytelength,
    db_settings_t *db_settings)
{
    uint64_t offset, address_bytelength;
    uint16_t tcp_port, udp_port, i, j;

    // Partitioning
    state->db_endpoints = db_settings->db_endpoints;
    state->n_of_db_endpoints = db_settings->n_of_db_endpoints;
    state->num_of_points = (uint64_t)1 << address_bitlength;
    state->points_per_db_endpoint = (state->num_of_points + state->n_of_db_endpoints - 1) / state->n_of_db_endpoints;
    address_bytelength = (address_bitlength + 7) >> 3;

    // Create a network_state_t[] for each database endpoint, each array contains a networking_state_t for each udp port
    state->networking_states = (networking_state_t **)malloc(state->n_of_db_endpoints * sizeof(networking_state_t *));
    for (i = 0; i < state->n_of_db_endpoints; i++)
    {
        state->networking_states[i] = (networking_state_t *)malloc(state->db_endpoints[i].ports * sizeof(networking_state_t));
    }

    // Setup ports
    for (i = 0; i < state->n_of_db_endpoints; i++)
    {
        tcp_port = state->db_endpoints[i].base_port;

        // For each endpoint create sockets to each udp port
        for (j = 0; j < state->db_endpoints[i].ports; j++)
        {
            udp_port = tcp_port + j;
            if (networking_start(&state->networking_states[i][j], state->db_endpoints[i].ipv4_addr, tcp_port, udp_port) == EXIT_FAILURE)
            {
                printf("Error initialising db endpoint %" PRIu16 "\n", i);
                return EXIT_FAILURE;
            }
        }

        // Setup database entry width
        offset = (uint64_t)i * state->points_per_db_endpoint;

        sprintf(state->query, "s %" PRIu64 " %" PRIu64 " %" PRIu64, address_bytelength, point_bytelength, offset);

        // I also wanna delete the db before starting
        // TODO: don't manage like this in multithreading
        state->query[0] += 'd';

        // Startup message is sent only to base_port for each endpoint
        if (networking_tcp_write(&state->networking_states[i][0], state->query, strlen(state->query)) == EXIT_FAILURE)
        {
            printf("Error sending start message to db endpoint %" PRIu16 "\n", i);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void terminate_storage(storage_state_t *state)
{
    // Drop db
    // NOTE: we pass "0 0" just because the server assumes (using sscanf) that a uint64_t and a size_t are gonna be provided in the query;
    //		 for simplicity's sake we can just pass any value.
    
    // End attack
    int res;
    uint64_t i, j;
    sprintf(state->query, "e 0 0 0");
    for (i = 0; i < state->n_of_db_endpoints; i++)
    {
        // End message sent only to base_port
        res = networking_tcp_write(&state->networking_states[i][0], state->query, strlen(state->query));
        if (res == EXIT_FAILURE)
        {
            printf("failed to send end command: %s\n", state->query);
        }

        for (j = 0; j < state->db_endpoints[i].ports; j++)
        {
            networking_end(&state->networking_states[i][j], res);
        }
        free(state->networking_states[i]);
    }
    free(state->networking_states);
}

void insert_storage_entry(storage_state_t *state, unsigned char *input_string, uint64_t input_bytelength, uint64_t address)
{
    int written, i, res;

    // Prepare write query
    written = sprintf(state->query, "w %" PRIx64 " %" PRIu64 " ", address, input_bytelength);
    for (i = 0; i < input_bytelength; i++)
    {
        written += sprintf(state->query + written, "%c", input_string[i]);
    }

    // Make query
    uint64_t ep = compute_endpoint(state, address);
    uint64_t udp_port_id = omp_get_thread_num() % state->db_endpoints[ep].ports; // computing this % all the time seems slow
    res = networking_udp_write(&state->networking_states[ep][udp_port_id], state->query, written);
    if (res == EXIT_FAILURE)
    {
        printf("failed writing over udp\n");
    }
}

int retrieve_storage_entry(storage_state_t *state, unsigned char *output_string, int buffer_len, uint64_t address)
{
    int written;

    // Prepare read query
    written = sprintf(state->query, "r %" PRIx64 " %d", address, buffer_len);
    
    uint64_t ep = compute_endpoint(state, address);
    uint64_t udp_port_id = omp_get_thread_num() % state->db_endpoints[ep].ports; // computing this % all the time seems slow
    
    if (networking_udp_write(&state->networking_states[ep][udp_port_id], state->query, written) == EXIT_FAILURE)
    {
        printf("failed to request read\n");
        return -1;
    }

    int received = networking_udp_read(&state->networking_states[ep][udp_port_id], (char *)output_string, (size_t)buffer_len);
    
    return received;
}

void print_db_settings(db_settings_t *db_settings)
{
    uint64_t i;

    for (i = 0; i < db_settings->n_of_db_endpoints; i++)
    {
        printf("addr: %s\n", db_settings->db_endpoints[i].ipv4_addr);
        printf("ports: [%" PRIu16 ", %" PRIu16 ")\n",
               db_settings->db_endpoints[i].base_port,
               db_settings->db_endpoints[i].base_port + db_settings->db_endpoints[i].ports);
        printf("\n");
    }

    printf("Total number of endpoints: %" PRIu64 "\n", db_settings->n_of_db_endpoints);
}