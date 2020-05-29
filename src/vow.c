/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: functions for van Oorschot-Wiener attack
*********************************************************************************************/

#include <omp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "prng.h"
#include "../tests/test_extras.h"
#include "triples.h"

#include "sync_strategies.c"
#include "benchmarking.c"

/**
 * @brief runs one "iteration" of vOW: sampling a point, checking for distinguishedness and possibly backtracking
 * 
 * @param S shared state pointer
 * @param private_state private state pointer
 * @param t temporary triple pointer
 * @param success pointer to global success variable
 * @return true vOW terminated, break out of loop
 * @return false keep looping
 */
static inline bool vOW_one_iteration(
    shared_state_t *S,
    private_state_t *private_state,
    trip_t *t,
    bool *success,
    double ratio_of_points_to_mine)
{
    // Walk to the next point using the current random function
    update(private_state);
    private_state->current.current_steps += 1;

    // Check if the new point is distinguisihed
    if (distinguished(private_state))
    {
        // Found a distinguished point. Try backtracking if unsuccessful, sample a new starting point
        uint64_t id;
        bool read;
        bool res;
        private_state->current_dist++;
        private_state->dist_points++;  // S->current_dist gets reset, this doesn't

        id = mem_index(private_state);
        copy_trip(&private_state->trip, &S->memory[id], private_state->NWORDS_STATE);
        read = (private_state->trip.current_steps > 0);

        // Did not get a collision in value, hence it was just a memory address collision
        if (!read || !is_equal_st(&private_state->trip.current_state, &private_state->current.current_state, private_state->NWORDS_STATE)) {
            private_state->mem_collisions += 1;
        } else {
            // Not a simple memory collision, backtrack!
            copy_trip(t, &private_state->current, private_state->NWORDS_STATE);
            res = backtrack(&private_state->trip, t, S, private_state);

            // Only check for success when not running for stats
            if (!private_state->collect_vow_stats) {
                if (res || *success) {
                    *success = true;
                    return true;
                }
            }
        }
        // Didn't get the golden collision, write the current distinguished point to memory
        // and sample a new starting point
        write_to_memory(&private_state->current, S, private_state, id);
        sample(private_state);
    }

    // Check if enough points have been mined for the current random function
    if (private_state->current_dist >= private_state->MAX_DIST * ratio_of_points_to_mine) {
        // Enough points collected for this random function
        if (!private_state->collect_vow_stats) {
#if defined(STAKHANOVIST_SYNC)
            if (stakhanovist_resync_should_resync(S, private_state)) {
                sample(private_state);
                update_random_function(S, private_state);
                stakhanovist_resync_do_resync(S, private_state);
            }
#elif defined(WINDOWED_SYNC)
            // In real attack. Sample a new starting point and random function
            sample(private_state);
            update_random_function(S, private_state);
            private_state->random_functions++; // maybe this could be merged with update_random_function
            private_state->current_dist = 0;
#elif defined(NOBIGGIE_SYNC)
            if (nobiggie_resync_should_resync(S, private_state, success)) {   
                // Resync, no thread has found the solution in this function version, so barriers inside this scope would be hit by all
                nobiggie_resync_do_resync(S, private_state);
// Wait for 0 to reset S->resync_cores inside resync
#pragma omp barrier
            } else {
                // Some core found the solution while waiting
                return true;
            }
#endif
        } else {
            // we are collecting stats only for one random function, can stop vOW
            return true;
        }
    }

    if (private_state->current.current_steps >= private_state->MAX_STEPS) {
        // Walked too long without finding a new distinguished point
        // hence, sample a new starting point
        sample(private_state);
    }
    return false;
}

#if (OS_TARGET == OS_LINUX)
// Handle Ctrl+C to stop prematurely and collect statistics
bool ctrl_c_pressed = false;
void sigintHandler(int sig_num)
{
    /* Refer http://en.cppreference.com/w/c/program/signal */
    ctrl_c_pressed = true;
}
#endif

bool vOW(shared_state_t *S)
{
    bool success = false;
    double start_wall_time = omp_get_wtime();
    double *points_ratio = NULL;

    S->cpu_cycles = -cpu_cycles();

    // Explicitly disable dynamic teams (ensures running on S->N_OF_CORES cores)
    omp_set_dynamic(0);

    // Runs cores benchmarks (across remote machines if used) to allocate work
    points_ratio = (double *)malloc(S->N_OF_CORES * sizeof(double));
    if (points_ratio == NULL) {
        fprintf(stderr, "error: could not alloc points_ratio memory");
        goto end;
    }
    run_benchmark(points_ratio, S->instance, 5000);

// Runs the real attack
#pragma omp parallel num_threads(S->N_OF_CORES)
    {
        private_state_t private_state;
        init_private_state(S, &private_state);
        double ratio_of_points_to_mine = points_ratio[private_state.thread_id];
        double internal_cpu_time = omp_get_wtime();
        initialize_private_memory(S, &private_state);
        trip_t t = init_trip(private_state.NWORDS_STATE);

#if (OS_TARGET == OS_LINUX)
        // Set a Ctrl+C handler to dump statistics
        signal(SIGINT, sigintHandler);
#endif

        // while we haven't exhausted the random functions to try
        while (private_state.random_functions <= private_state.MAX_FUNCTION_VERSIONS && !success)
        {
#if (OS_TARGET == OS_LINUX)
            if (ctrl_c_pressed) {
                printf("\n%d: thinks ctrl+c was pressed", private_state.thread_id);
                break;
            }
#endif

#if defined(WINDOWED_SYNC)
            // "Windowed" resync
            windowed_resync(S, &private_state);
#endif
            // Mine new points
            if (vOW_one_iteration(S, &private_state, &t, &success, ratio_of_points_to_mine)) {
                break;
            }
        }
        internal_cpu_time = omp_get_wtime() - internal_cpu_time;

// Collect all the stats from each thread
#pragma omp critical
        {
            S->collisions += private_state.collisions;
            S->mem_collisions += private_state.mem_collisions;
            S->dist_points += private_state.dist_points;
            S->number_steps_collect += private_state.number_steps_collect;
            S->number_steps_locate += private_state.number_steps_locate;
            S->number_steps = S->number_steps_collect + S->number_steps_locate;
            S->total_time += internal_cpu_time;
            S->final_avg_random_functions += (double)private_state.random_functions / (double)S->N_OF_CORES;
        }
        free_trip(&t);
        cleanup_private_memory(&private_state);
        free_private_state(&private_state);
    }

end:
#if (OS_TARGET == OS_LINUX)
    ctrl_c_pressed = false;
#endif
    S->cpu_cycles += cpu_cycles();
    free(points_ratio);
    S->success = success;
    S->wall_time = omp_get_wtime() - start_wall_time;

    return success;
}
