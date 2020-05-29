#pragma once

#include <stdio.h>
#include <inttypes.h>
#include <omp.h>
#include "networking.h"

// Assuming that the ports are [base_port, base_port + 1, ..., base_port + ports)
typedef struct
{
    char ipv4_addr[16];
    uint16_t base_port;
    uint16_t ports;
} db_ep_coordinates_t;

typedef struct
{
    uint64_t n_of_db_endpoints;
    db_ep_coordinates_t *db_endpoints;
} db_settings_t;

typedef struct
{
    networking_state_t **networking_states;
    db_ep_coordinates_t *db_endpoints;
    char query[128];
    uint64_t points_per_db_endpoint;
    uint64_t num_of_points;
    uint64_t n_of_db_endpoints;
} storage_state_t;

// General api
int initialize_storage(
    storage_state_t *state,
    uint64_t address_bytelength,
    uint64_t point_bytelength,
    db_settings_t *db_settings);

void terminate_storage(
    storage_state_t *state);

void insert_storage_entry(
    storage_state_t *state,
    unsigned char *input_string,
    uint64_t input_bytelength,
    uint64_t address);

int retrieve_storage_entry(
    storage_state_t *state,
    unsigned char *output_string,
    int buffer_len,
    uint64_t address);

// Debug tool
void print_db_settings(db_settings_t *db_settings);