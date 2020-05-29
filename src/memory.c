#include <string.h>
#include "memory.h"
#include "triples.h"
#include "settings.h"

int initialize_shared_memory(shared_state_t *S)
{
#ifdef STORE_IN_MEMORY
    if ((S->memory = calloc(S->MEMORY_SIZE, sizeof(trip_t))) == NULL) // triples
        return EXIT_FAILURE;
    for (uint64_t i = 0; i < S->MEMORY_SIZE; i++)
    {
        if ((S->memory[i].current_state.words = calloc(S->NWORDS_STATE, sizeof(digit_t))) == NULL)  /* Only words since > bytes */
            return EXIT_FAILURE;
        if ((S->memory[i].initial_state.words = calloc(S->NWORDS_STATE, sizeof(digit_t))) == NULL)
            return EXIT_FAILURE;
        S->memory[i].current_steps = 0;
    }
    return EXIT_SUCCESS;
#elif defined(STORE_IN_DATABASE)

    // Database settings
    int res;
    S->STORAGE_TRIP_BYTELEN = (int)(2 * S->NBYTES_STATE + 8);  // Size of a trip
    res = initialize_storage(&S->storage_state, S->MEMORY_LOG_SIZE, S->STORAGE_TRIP_BYTELEN, S->db_settings);
    return res;
#endif
}

void initialize_private_memory(shared_state_t *S, private_state_t *private_state)
{
#ifdef STORE_IN_MEMORY
    (void)S;
    (void)private_state;
#elif defined(STORE_IN_DATABASE)
    // One temp buffer per thread
    private_state->storage_temp_buffer[0] = (unsigned char *)malloc(S->STORAGE_TRIP_BYTELEN);
    private_state->storage_dummy_trip.current_state.bytes = NULL;
    private_state->storage_dummy_trip.current_steps = 0;
    private_state->storage_dummy_trip.initial_state.bytes = NULL;
    private_state->temp_trip_pointer = (trip_t *)malloc(sizeof(trip_t));
#endif
}

void cleanup_shared_memory(shared_state_t *shared_state)
{
#ifdef STORE_IN_MEMORY
    for (unsigned int i = 0; i < shared_state->MEMORY_SIZE; i++) {
        free_trip(&shared_state->memory[i]);
    }
    free(shared_state->memory);
#elif defined(STORE_IN_DATABASE)
    terminate_storage(&shared_state->storage_state);
#endif
}

void cleanup_private_memory(private_state_t *private_state)
{
#ifdef STORE_IN_MEMORY
    // Do nothing
    (void)private_state;
#elif defined(STORE_IN_DATABASE)
    free(private_state->temp_trip_pointer);
    free(private_state->storage_temp_buffer[0]);
#endif
}

/*
* Reads triple from memory at specified address.
* If no triple is found there, returns false, else true.
*/
extern inline bool read_from_memory(trip_t **t, shared_state_t *S, private_state_t *private_state, uint64_t address)
{
#ifdef STORE_IN_MEMORY
    (void)private_state;
    *t = &S->memory[address];
#endif

#ifdef STORE_IN_DATABASE
    // We read the full triple and recyle it, cause memory collisions are rare,
    // no point in reading column by column

#ifdef COLLECT_DATABASE_STATS
    clock_t tt = clock();
#endif

    retrieve_storage_entry(&S->storage_state, private_state->storage_temp_buffer[0], S->STORAGE_TRIP_BYTELEN, address);

#ifdef COLLECT_DATABASE_STATS
    S->debug_stats[0] += (double)1.;
    S->debug_stats[1] += (double)(clock() - tt) / CLOCKS_PER_SEC;
#endif

    // NOTE HERE WE SHOULD USE THE THREAD ID, NOT [0]
    // 8s are due to being sizeof(uint64_t), i.e. the type of current_steps
    ((*t)->initial_state).bytes = private_state->storage_temp_buffer[0];
    (*t)->current_steps = *(uint64_t *)(private_state->storage_temp_buffer[0] + S->NBYTES_STATE);
    ((*t)->current_state).bytes = private_state->storage_temp_buffer[0] + S->NBYTES_STATE + 8;
#endif

    return (*t)->current_steps > 0; // if no steps, then memory location was empty
}

extern inline void write_to_memory(trip_t *t, shared_state_t *S, private_state_t *private_state, uint64_t address)
{
#ifdef STORE_IN_MEMORY
    (void)private_state;
    copy_trip(&S->memory[address], t, private_state->NWORDS_STATE);
#endif

#ifdef STORE_IN_DATABASE
    // Using a single storage_temp_buffer requiring copying the values of our triple
    // still, this may be <= than doing multiple/a single but structured query
    // (since anyway we have to copy every byte when doing the query)

    // NOTE HERE WE SHOULD USE THE THREAD ID, NOT 0
    // 8s are due to being sizeof(uint64_t), i.e. the type of current_steps

    memcpy(private_state->storage_temp_buffer[0], t->initial_state.bytes, S->NBYTES_STATE);
    memcpy(private_state->storage_temp_buffer[0] + S->NBYTES_STATE, (unsigned char *)&t->current_steps, 8);
    memcpy(private_state->storage_temp_buffer[0] + S->NBYTES_STATE + 8, t->current_state.bytes, S->NBYTES_STATE);

#ifdef COLLECT_DATABASE_STATS
    clock_t tt = clock();
#endif

    // Hardcoded [0], assuming only one thread
    insert_storage_entry(&S->storage_state, private_state->storage_temp_buffer[0], S->STORAGE_TRIP_BYTELEN, address);

#ifdef COLLECT_DATABASE_STATS
    S->debug_stats[2] += (double)1.;
    S->debug_stats[3] += (double)(clock() - tt) / CLOCKS_PER_SEC;
#endif
#endif // STORE_IN_DATABASE
}

void fix_overflow(st_t *s, const uint64_t nbytes_state, const uint64_t nbits_overflow)
{
    if (nbits_overflow != 0)
        s->bytes[nbytes_state - 1] &= (0xFF >> (8 - nbits_overflow));
}