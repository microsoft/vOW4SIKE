#include <omp.h>
#include "triples.hpp"
#include "state.hpp"
#include "../prng/xof.h"
#include "../vow.hpp"


template <class Point, class PRNG, class RandomFunction, class Instance>
private_state_t<Point, PRNG, RandomFunction, Instance>::private_state_t(
    Instance *instance
)
{
    thread_id = omp_get_thread_num();

    current_dist = 0;
    random_functions = 1;
    step_function = new RandomFunction(instance);

    trip = new Trip<Point, Instance>(instance);
    current = new Trip<Point, Instance>(instance);

    collect_vow_stats = instance->collect_vow_stats;
    collisions = 0;
    mem_collisions = 0;
    dist_points = 0;
    number_steps_collect = 0;
    number_steps_locate = 0;

    /* PRNG */

    // derive a new seed for the internal prng
    XOF((unsigned char *)(&(PRNG_SEED)), (unsigned char *)(&instance->PRNG_SEED), sizeof(PRNG_SEED), sizeof(PRNG_SEED), (unsigned long)omp_get_thread_num() + 1);
    prng = new PRNG((unsigned long)PRNG_SEED);

    /* Initialization for Hansel&Gretel */
    crumbs.swap_position = 0;
    crumbs.scratch_position = 0;
    crumbs.index_position = 0;
    crumbs.max_dist = 1;
    crumbs.real_dist = 1;
    crumbs.position = 0;
    crumbs.num_crumbs = 0;
    crumbs.max_crumbs = instance->MAX_CRUMBS;
    crumbs.positions = (uint64_t *) calloc(1, crumbs.max_crumbs * sizeof(uint64_t));
    crumbs.index_crumbs = (uint64_t *) calloc(1, crumbs.max_crumbs * sizeof(uint64_t));
    crumbs.crumbs = (uint64_t *) calloc(instance->NWORDS_STATE, crumbs.max_crumbs * sizeof(uint64_t));
    // note: should check for null pointers.
}

template <class Point, class PRNG, class RandomFunction, class Instance>
private_state_t<Point, PRNG, RandomFunction, Instance>::~private_state_t()
{
    free(crumbs.positions);
    free(crumbs.index_crumbs);
    free(crumbs.crumbs);
    // delete trip;     // don't delete these, they point to Memory
    // delete current;  // don't delete these, they point to Memory
    delete prng;
    delete step_function;
}

template <class Point, class PRNG, class RandomFunction, class Instance>
void private_state_t<Point, PRNG, RandomFunction, Instance>::clean_crumbs()
{
    /* Initialization for Hansel&Gretel */
    crumbs.swap_position = 0;
    crumbs.scratch_position = 0;
    crumbs.index_position = 0;
    crumbs.max_dist = 1;
    crumbs.real_dist = 1;
    crumbs.position = 0;
    crumbs.num_crumbs = 0;
}

#include "../templating/state.inc"