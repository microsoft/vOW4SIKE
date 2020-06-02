#pragma once
#include <cstdint>
#include "../prng/interface.hpp"
#include "points.hpp"
#include "triples.hpp"
#include "crumbs.hpp"

template <class Point, class PRNG, class RandomFunction, class Instance>
class private_state_t // rename this?
{
    public: // is all this really public?
        int thread_id;

        /* State */
        Trip<Point, Instance> *current; // rename to current_triple?
        uint64_t current_dist;
        uint64_t random_functions;
        RandomFunction *step_function;

        /* Prng */
        uint64_t PRNG_SEED;
        PRNG *prng;

        /* Statistics */
        bool collect_vow_stats; // don't terminate on collision found if true
        uint64_t iterations;
        uint64_t collisions;
        uint64_t mem_collisions;
        uint64_t dist_points;
        uint64_t number_steps_collect; // Counts function evaluations for collecting distinguished points
        uint64_t number_steps_locate;  // Counts function evaluations during collision locating

        /* Storage */
        Trip<Point, Instance> *trip; // todo: what's this for?

        /* Hansel & Gretel */
        CrumbStruct crumbs;

        private_state_t(Instance *instance);
        ~private_state_t();
        void clean_crumbs();
};