#pragma once
#include <cstdint>
#include "config.h"
#include "settings.h"
#include "types/random_function.hpp"
#include "memory/interface.hpp"
#include "types/instance.hpp"
#include "types/triples.hpp"
#include "types/state.hpp"
#include "types/resync_state.hpp"
#include "types/bintree.hpp"

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
class vOW
{
    protected:
        // methods
        void step(private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
        bool iteration(private_state_t<Point, PRNG, RandomFunction, Instance> &private_state, Trip<Point, Instance> &t, double ratio_of_points_to_mine);
        bool hansel_and_gretel_backtrack(Trip<Point, Instance> &c0, Trip<Point, Instance> &c1, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
        bool classic_backtrack(Trip<Point, Instance> &c0, Trip<Point, Instance> &c1, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
        bool backtrack(Trip<Point, Instance> &c0, Trip<Point, Instance> &c1, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
        void benchmark(uint64_t target_number_of_points);

    public:
        // settings
        Instance *instance;
        Memory *memory;
        RandomFunction *step_function; // probably not needed, since private states should have it
        double *points_ratio;

        /* resync */
        resync_state_t *resync_state;

        binTree_t<Point> dist_cols;
        bool success;
        double wall_time;
        double total_time;
        uint64_t collisions;
        uint64_t mem_collisions;
        uint64_t dist_points;
        double final_avg_random_functions;
        uint64_t number_steps_collect; /* Counts function evaluations for collecting distinguished points.*/
        uint64_t number_steps_locate;  /* Counts function evaluations during collision locating. */
        uint64_t number_steps;         /* Total count, the sum of the above. */
        int64_t cycles;

        vOW(Instance *inst);
        ~vOW();
        void reset();
        bool run();
};


