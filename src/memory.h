#pragma once
#include "types/triples.h"
#include "types/state.h"

int initialize_shared_memory(shared_state_t *shared_state);
void initialize_private_memory(shared_state_t *shared_state, private_state_t *private_state);
void cleanup_shared_memory(shared_state_t *shared_state);
void cleanup_private_memory(private_state_t *private_state);
bool read_from_memory(trip_t **t, shared_state_t *shared_state, private_state_t *private_state, uint64_t address);
void write_to_memory(trip_t *t, shared_state_t *shared_state, private_state_t *private_state, uint64_t address);
void fix_overflow(st_t *s, const uint64_t nbytes_state, const uint64_t nbits_overflow);