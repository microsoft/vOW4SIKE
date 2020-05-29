#include "triples.h"

trip_t init_trip(uint64_t nwords_state)
{
    trip_t t;

    t.current_state.words = calloc(nwords_state, sizeof(digit_t));
    t.initial_state.words = calloc(nwords_state, sizeof(digit_t));
    return t;
}

void free_trip(trip_t *t)
{
    free(t->current_state.words);
    free(t->initial_state.words);
}