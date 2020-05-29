#pragma once
#include <inttypes.h>
#include "types/triples.h"
#include "types/state.h"

trip_t init_trip(uint64_t nwords_state);
void free_trip(trip_t *t);
void is_set_trip(st_t *r, const st_t *s, const shared_state_t *S);
bool is_equal_st(const st_t *s, const st_t *t, const uint64_t nwords_state);
void copy_trip(trip_t *t, const trip_t *u, const uint64_t nwords_state);

// debug
void print_st(st_t *s, shared_state_t *shared_state);
void print_trip(trip_t *t, shared_state_t *shared_state);