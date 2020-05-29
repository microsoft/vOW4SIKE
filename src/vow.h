#pragma once
#include <stdbool.h>
#include "types/state.h"
#include "types/triples.h"

bool vOW_one_iteration(
    shared_state_t *S,
    private_state_t *private_state,
    trip_t *t,
    bool *success,
    double ratio_of_points_to_mine);