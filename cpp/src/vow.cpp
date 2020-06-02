#include <omp.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <cstring>
#include "config.h"
extern "C"
{
#if (OS_TARGET == OS_LINUX)
#include <signal.h>
#endif
}
#include "utils/cycles.h"
#include "sync_strategies.hpp"
#include "types/triples.hpp"
#include "utils/bintree.hpp"
#include "utils/buftools.h"
#include "vow.hpp"


template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool vOW<Point, Memory, RandomFunction, PRNG, Instance>::hansel_and_gretel_backtrack(Trip<Point, Instance> &c_mem, Trip<Point, Instance> &c_crumbs, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{

    uint64_t L;
    Trip<Point, Instance> cmem(instance);
    uint64_t i, k, index;
    uint64_t crumb;
    bool resp, equal;

    cmem.from_trip(c_mem);

    // Make the memory trail (without crumbs) at most the length of the crumbs trail.
    if (cmem.current_steps > c_crumbs.current_steps)
    {
        L = cmem.current_steps - c_crumbs.current_steps;
        // Catch up the trails
        for (i = 0; i < L; i++)
        {
            private_state.step_function->eval(*cmem.initial_state);
            private_state.number_steps_locate++;
        }
        cmem.current_steps = c_crumbs.current_steps;
    }
    
    // Check for Robin Hood
    if (*(cmem.initial_state) == *(c_crumbs.initial_state))
    {
        return false; // Robin Hood
    }

    // The memory path is L steps shorter than the crumbs path.
    L = c_crumbs.current_steps - cmem.current_steps;
    k = 0;
    // Since there has been at least one step, there is at least one crumb.
    // Crumbs only store intermediate points, not the initial state and not
    // necessarily the current state.
    index = private_state.crumbs.positions[0] + 1;

    while ((L > index) && (k + 1 < private_state.crumbs.num_crumbs))
    {
        // There are still crumbs to check and we haven't found the next crumb to reach.
        k++;
        index = private_state.crumbs.positions[k] + 1;
    }
    // Either have found the next crumb or ran out of crumbs to check.
    if (L > index)
    {
        // Ran out of crumbs to check, i.e. already in the interval beyond the last crumb.
        // Trails collide after last crumb.
        // Call original backtrack on memory trail and shortened crumbs trail.
        c_crumbs.initial_state->from_buf(&private_state.crumbs.crumbs[private_state.crumbs.index_crumbs[k]]);
        c_crumbs.current_steps -= (private_state.crumbs.positions[k] + 1);
        resp = classic_backtrack(cmem, c_crumbs, private_state);
    }
    else
    {
        Trip<Point, Instance> c0_(instance);

        // Next crumb to check lies before (or is) the last crumb.
        c0_.from_trip(cmem);

        do
        {
            cmem.current_steps = c0_.current_steps;
            cmem.initial_state->from_point(*c0_.initial_state);
            crumb = private_state.crumbs.crumbs[private_state.crumbs.index_crumbs[k]];
            index = private_state.crumbs.positions[k] + 1;

            L = cmem.current_steps - (c_crumbs.current_steps - index);
            for (i = 0; i < L; i++)
            {
                private_state.step_function->eval(*c0_.initial_state);
                private_state.number_steps_locate++;
            }
            c0_.current_steps -= L;
            k++;
            equal = (c0_.initial_state->words[0] == crumb); // TODO: this is fixed for 64 bit crumbs!!!
        } while (!equal && k < private_state.crumbs.num_crumbs);
        // Either found the colliding crumb or moved to the interval beyond the last crumb.

        if (equal)
        { // Have a colliding crumb.
            cmem.current_state->from_buf(&crumb);
            cmem.current_steps -= c0_.current_steps;
            if (k == 1)
            {
                c0_.current_steps = private_state.crumbs.positions[0] + 1;
                c0_.initial_state->from_point(*c_crumbs.initial_state);
            }
            else
            {
                c0_.current_steps = private_state.crumbs.positions[k - 1] - private_state.crumbs.positions[k - 2];
                c0_.initial_state->from_buf(&private_state.crumbs.crumbs[private_state.crumbs.index_crumbs[k - 2]]);
            }
            c0_.current_state->from_buf(&crumb);
        }
        else
        { // Collision happens after the last crumb.
            cmem.current_steps = c0_.current_steps;
            cmem.initial_state->from_buf(&crumb);
        }
        resp = classic_backtrack(cmem, c0_, private_state);
    }
    return resp;
}

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool vOW<Point, Memory, RandomFunction, PRNG, Instance>::classic_backtrack(Trip<Point, Instance> &c0, Trip<Point, Instance> &c1, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    uint64_t L;
    uint64_t i;

    // Make c0 have the largest number of steps
    if (c0.current_steps < c1.current_steps)
    {
        SwapPoint<Point>(*c0.initial_state, *c1.initial_state);
        L = (unsigned long)(c1.current_steps - c0.current_steps);
    }
    else
    {
        L = (unsigned long)(c0.current_steps - c1.current_steps);
    }

    // Catch up the trails
    for (i = 0; i < L; i++)
    {
        private_state.step_function->eval(*c0.initial_state);
        private_state.number_steps_locate++;
    }

    if (*(c0.initial_state) == *(c1.initial_state))
    {
        return false; // Robin Hood
    }

    Point c0_(instance);
    Point c1_(instance);

    for (i = 0; i < c1.current_steps + 1; i++)
    {
        private_state.step_function->eval(c0_, *c0.initial_state);
        private_state.number_steps_locate++;
        private_state.step_function->eval(c1_, *c1.initial_state);
        private_state.number_steps_locate++;

        if (c0_ == c1_)
        {
            // Record collision
            private_state.collisions++;
            if (private_state.collect_vow_stats)
            {
                #pragma omp critical
                {
                    insertTree<Point, Instance>(&dist_cols, c0.initial_state, c1.initial_state, instance);
                }
            }

            if (c0_ == *(private_state.step_function->image))
            {
                // // multithreading may make this fail
                // // verbose check of having found the collision
                // #pragma omp critical
                // {
                //     printf("\n\n\n");
                //     printf("%d: found the collision\n", private_state.thread_id);
                //     printf("preimages: ");
                //     private_state.step_function->preimages[0]->print();
                //     printf(" ");
                //     private_state.step_function->preimages[1]->print();
                //     printf(" image: ");
                //     private_state.step_function->image->print();
                //     printf("\ngolden found: ");
                //     c0_.print();
                //     printf("\npoints found: ");
                //     c0.initial_state->print();
                //     printf(" ");
                //     c1.initial_state->print();
                //     printf("\n");
                //     printf("success %d\n", (int)(
                //         (*(c0.initial_state) == *(private_state.step_function->preimages[0])
                //         && *(c1.initial_state) == *(private_state.step_function->preimages[1]))
                //         ||
                //         (*(c0.initial_state) == *(private_state.step_function->preimages[1])
                //         && *(c1.initial_state) == *(private_state.step_function->preimages[0]))
                //     ));
                // }

                assert( // Only for verifying
                    (
                        (*(c0.initial_state) == *(private_state.step_function->preimages[0])
                        && *(c1.initial_state) == *(private_state.step_function->preimages[1]))
                        ||
                        (*(c0.initial_state) == *(private_state.step_function->preimages[1])
                        && *(c1.initial_state) == *(private_state.step_function->preimages[0]))
                    )
                );
                return true;
            }
            else
            {
                return false;
            }
            /*
            // more compact
            bool retval = bool(c0_ == private_state.image);
            return retval;
            */
        }
        else
        {
            (*c0.initial_state).from_point(c0_);
            (*c1.initial_state).from_point(c1_);
        }
    }
    // assert(false); // Should never reach here
    // if multiprocessing, it may reach here if a thread backtracks while another changes a global function version
    // should not apply to privately held function versions
    return false;
}


template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool vOW<Point, Memory, RandomFunction, PRNG, Instance>::backtrack(Trip<Point, Instance> &c0, Trip<Point, Instance> &c1, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    if (instance->HANSEL_GRETEL)
    {
        return hansel_and_gretel_backtrack(c0, c1, private_state);
    }
    else
    {
        return classic_backtrack(c0, c1, private_state);
    }
}


/**
* @brief walk to the next point in the vOW, following the random function

*
* @param S
* @param private_state
*/
template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void vOW<Point, Memory, RandomFunction, PRNG, Instance>::step(private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    private_state.step_function->eval(*private_state.current->current_state);
    private_state.number_steps_collect++;

    if (instance->HANSEL_GRETEL)
    {
        if (private_state.crumbs.num_crumbs < private_state.crumbs.max_crumbs)
        {
            bufcpy(&private_state.crumbs.crumbs[private_state.crumbs.position], private_state.current->current_state->words, sizeof(digit_t) * instance->NWORDS_STATE);
            private_state.crumbs.positions[private_state.crumbs.position] = private_state.crumbs.position;
            private_state.crumbs.index_crumbs[private_state.crumbs.position] = private_state.crumbs.position;
            private_state.crumbs.num_crumbs++;
        }
        else if (private_state.crumbs.position - private_state.crumbs.positions[private_state.crumbs.max_crumbs - 1] == private_state.crumbs.max_dist)
        {
            uint64_t temp = private_state.crumbs.index_crumbs[private_state.crumbs.index_position];
            for (uint64_t i = private_state.crumbs.index_position; i < private_state.crumbs.max_crumbs - 1; i++)
            { // Updating table with crumb indices for the crump table
                private_state.crumbs.index_crumbs[i] = private_state.crumbs.index_crumbs[i + 1];
            }
            private_state.crumbs.index_crumbs[private_state.crumbs.max_crumbs - 1] = temp;
            private_state.crumbs.index_position++;
            if (private_state.crumbs.index_position > private_state.crumbs.max_crumbs - 1)
                private_state.crumbs.index_position = 0;
            bufcpy(&private_state.crumbs.crumbs[temp], private_state.current->current_state->words, sizeof(digit_t) * instance->NWORDS_STATE); // Inserting a new crumb at the end of the crumb table

            for (uint64_t i = private_state.crumbs.scratch_position; i < private_state.crumbs.max_crumbs - 1; i++)
            { // Updating table with crumb positions
                private_state.crumbs.positions[i] = private_state.crumbs.positions[i + 1];
            }
            private_state.crumbs.positions[private_state.crumbs.max_crumbs - 1] = private_state.crumbs.position;
            private_state.crumbs.swap_position += 2 * private_state.crumbs.real_dist;
            private_state.crumbs.scratch_position++;
            if (private_state.crumbs.swap_position > private_state.crumbs.max_crumbs - 1)
            { // Kind of cumbersome, maybe this can be simplified (but not time critical)
                private_state.crumbs.swap_position = 0;
                private_state.crumbs.real_dist <<= 1;
            }
            if (private_state.crumbs.scratch_position > private_state.crumbs.max_crumbs - 1)
            {
                private_state.crumbs.scratch_position = 0;
                private_state.crumbs.max_dist <<= 1;
                private_state.crumbs.swap_position = private_state.crumbs.max_dist;
            }
        }
        private_state.crumbs.position++;
    }
}


/**
 * @brief runs one "iteration" of vOW: sampling a point, checking for distiniguishedness and possibly backtracking
 *
 * @param S shared state pointer
 * @param private_state private state pointer
 * @param t temporary triple pointer
 * @param success pointer to global success variable
 * @return true vOW terminated, break out of loop
 * @return false keep looping
 */
template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool vOW<Point, Memory, RandomFunction, PRNG, Instance>::iteration(
    private_state_t<Point, PRNG, RandomFunction, Instance> &private_state,
    Trip<Point, Instance> &t,
    double ratio_of_points_to_mine
)
{
    // printf("%d", private_state->thread_id);


    // walk to the next point using the current random function
    step(private_state);
    private_state.current->current_steps++; // todo: are we interfering with H&G?
    // private_state.current->print();
    // printf("\n");

    // check if the new point is distinguisihed
    // todo: do we wanna use function_version or random_functions as a distinguisher?
    if (private_state.current->current_state->is_distinguished(private_state.step_function->function_version))
    {
        // found a distinguished point. try backtracking
        // if unsuccessful, sample a new starting point
        uint64_t id;
        bool read;
        bool res;
        private_state.current_dist++;
        private_state.dist_points++; // private_state.current_dist gets reset, this doesn't

        id = private_state.current->current_state->mem_index(private_state.random_functions);

        private_state.trip->from_trip((*memory)[id]);
        read = (private_state.trip->current_steps > 0); // if == 0, the mem location was empty

        // private_state.current->current_state->print(); printf("  ");
        // private_state.trip->current_state->print();
        // printf("\t==: %d\n", (int)(*(private_state.trip->current_state) == *(private_state.current->current_state)));

        // did not get a collision in value, hence it was just a memory address collision
        if (!read || *(private_state.trip->current_state) != *(private_state.current->current_state))
        {
            private_state.mem_collisions += (int)(read);
        }
        else
        {
            // not a simple memory collision, backtrack!
            t.from_trip(private_state.current);
            res = backtrack(*private_state.trip, t, private_state);

            // we only check for success when not running for stats
            if (!private_state.collect_vow_stats)
            {
                if (res || success)
                {
                    success = true;
                    // printf("\n%d: success", private_state->thread_id);
                    return true;
                }
            }
        }

        // we didn't get the golden collision, write the current distinguished point to memory
        memory->write(private_state.current, id);
        // and sample a new starting point
        private_state.current->sample(private_state.prng);
        if (instance->HANSEL_GRETEL)
        {
            private_state.clean_crumbs();
        }
    }

    // check if we have mined enough points for the current random function
    if (private_state.current_dist >= instance->MAX_DIST * ratio_of_points_to_mine)
    {
        // we have collected enough points for this random function
        if (!private_state.collect_vow_stats)
        {

        #if defined(STAKHANOVIST_SYNC)
            // dbg output
            // printf("\n%d: has found %" PRIu64 " dist points / %.2f", private_state->thread_id, private_state->current_dist, (double)(private_state->MAX_DIST * ratio_of_points_to_mine));
            if (stakhanovist_resync_should_resync<Point>(this, private_state))
            {
                stakhanovist_resync_do_resync<Point>(this, private_state);
            }
        #elif defined(WINDOWED_SYNC)
            // we are running the real attack
            // we already just sampled a new starting point
            // sample a new randndom function
            private_state.random_functions++; // todo: maybe this could be merged with update_random_function
            private_state.current_dist = 0;
            private_state.step_function->update();
            private_state.current->sample(private_state.prng);
            if (instance->HANSEL_GRETEL)
            {
                private_state.clean_crumbs();
            }

            // dbg output
            //printf("\n%d %"PRIu64, private_state->thread_id, private_state.step_function->function_version);
            //if (private_state->thread_id == 0)
            //{ printf("\n0 (%"PRIu64"): %" PRIu64" -> ", private_state->random_functions, private_state.step_function->function_version); }

            // reset "resync done" flag for master
            if (private_state.thread_id == 0)
            {
                resync_state->cores[0] = 0;
            }

            // dbg output
            //if (private_state->thread_id == 0)
            //{ printf("%" PRIu64, private_state.step_function->function_version); }

        #elif defined(NOBIGGIE_SYNC)
            // dbg output
            // printf("\n%d: has found %" PRIu64 " dist points / %.2f", private_state->thread_id, private_state->current_dist, (double)(private_state->MAX_DIST * ratio_of_points_to_mine));

            if (nobiggie_resync_should_resync(this, private_state))
            {
                // resync, no thread has found the solution in this function version, so barriers inside this scope would be hit by all
                nobiggie_resync_do_resync(this, private_state);

                // wait for 0 to reset S->resync_state->cores inside resync
                // #pragma omp barrier
            }
            else
            {
                // some core found the solution while waiting
                return true;
            }
        #endif
        }
        else
        {
            // we are collecting stats only for one random function, can stop vOW
            return true;
        }
    }

    if (private_state.current->current_steps >= instance->MAX_STEPS)
    {
        // walked too long without finding a new distinguished point
        // hence, sample a new starting point
        private_state.current->sample(private_state.prng);
        if (instance->HANSEL_GRETEL)
        {
            private_state.clean_crumbs();
        }
    }

    return false;
}




#if (OS_TARGET == OS_LINUX)
// handle Ctrl+C to stop prematurely and collect statistics
bool ctrl_c_pressed = false;
void sigintHandler(int sig_num)
{
    (void)sig_num;
    /* Refer http://en.cppreference.com/w/c/program/signal */
    ctrl_c_pressed = true;
}
#endif

#include <iostream>

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool vOW<Point, Memory, RandomFunction, PRNG, Instance>::run()
{

    // printf("\n------------------------------------------------------\n");
    // #if defined(STAKHANOVIST_SYNC)
    // printf("\nstakhanov");
    // #elif defined(WINDOWED_SYNC)
    // printf("\nwindows -- not an ad");
    // #elif defined(NOBIGGIE_SYNC)
    // printf("\nnobiggie");
    // #endif

    success = false;
    wall_time = omp_get_wtime();
    points_ratio = NULL;
    cycles = -cpu_cycles();

    // explicitly disable dynamic teams (ensures running on vow->instance->N_OF_CORES threads)
    omp_set_dynamic(0);

    // runs cores benchmarks (across remote machines if used) to allocate work
    points_ratio = (double *)calloc(instance->N_OF_CORES, sizeof(double));
    if (points_ratio == NULL)
    {
        fprintf(stderr, "error: could not alloc points_ratio memory");
        goto end;
    }
    // printf("benchmark\n");
    benchmark(5000);
    // printf("after benchmark instance->PRNG_SEED = %lu\n", instance->PRNG_SEED);
    // getchar();

    // ( debug output )
    // printf("ratios: "); for (int i = 0; i < instance->N_OF_CORES; i++) { printf("%g ", points_ratio[i]); } printf("\n");

    // runs the real attack
    #pragma omp parallel num_threads(instance->N_OF_CORES)
    {
        private_state_t<Point, PRNG, RandomFunction, Instance> private_state(instance);
        double ratio_of_points_to_mine = points_ratio[private_state.thread_id];
        double internal_cpu_time = omp_get_wtime();

        // this is not being used at the time, may be useful for remote storage.
        // if added, add also to vOW::benchmark();
        // initialize_private_memory(this, &private_state);
        Trip<Point, Instance> t(instance);

        #if (OS_TARGET == OS_LINUX)
        // set a Ctrl+C handler to dump statistics
        signal(SIGINT, sigintHandler);
        #endif

        // while we haven't exhausted the random functions to try
        // int count = 0;
        // std::cout << "while, hg" << instance->HANSEL_GRETEL << " crumbs " << instance->MAX_CRUMBS << std::endl;
        while (private_state.random_functions <= instance->MAX_FUNCTION_VERSIONS && !success) // && count++ < 140)
        {
            #if (OS_TARGET == OS_LINUX)
            if (ctrl_c_pressed)
            {
                printf("\n%d: thinks ctrl+c was pressed", private_state.thread_id);
                break;
            }
            #endif

            #if defined(WINDOWED_SYNC)
            // "windowed" resync
            windowed_resync(this, private_state);
            #endif

            // mine new points
            if (iteration(private_state, t, ratio_of_points_to_mine))
            {
                break;
            }
        }
        internal_cpu_time = omp_get_wtime() - internal_cpu_time;

        // Collect all the stats from each thread
        #pragma omp critical
        {
            collisions += private_state.collisions;
            mem_collisions += private_state.mem_collisions;
            dist_points += private_state.dist_points;
            number_steps_collect += private_state.number_steps_collect;
            number_steps_locate += private_state.number_steps_locate;
            number_steps = number_steps_collect + number_steps_locate;
            total_time += internal_cpu_time;
            final_avg_random_functions += (double)private_state.random_functions / (double)instance->N_OF_CORES;
        }

        // cleanup_private_memory(&private_state);  // se above comment for initialize_private_memory
    }

    end:
    #if (OS_TARGET == OS_LINUX)
    // need to reset it since it's global
    ctrl_c_pressed = false;
    #endif
    cycles += cpu_cycles();
    free(points_ratio);

    wall_time = omp_get_wtime() - wall_time;

    return success;
}


template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
vOW<Point, Memory, RandomFunction, PRNG, Instance>::vOW(Instance *inst)
{
    /* Initialize state */
    if (!inst->processed)
    {
        inst->process();
    }
    instance = new Instance(inst);

    // prepare the global memory
    memory = new Memory(instance->MEMORY_SIZE, instance);

    /* Statistics */
    initTree<Point>(&dist_cols);
    success = false;
    wall_time = 0.;
    collisions = 0;
    mem_collisions = 0;
    dist_points = 0;
    number_steps_collect = 0;
    number_steps_locate = 0;
    number_steps = 0;
    final_avg_random_functions = 0.;

    /* resync */
    resync_state = new resync_state_t();
    resync_state->frequency = 5; // todo: this should be a param
    resync_state->cores = (uint8_t *)calloc(instance->N_OF_CORES, sizeof(uint8_t));
}



template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
vOW<Point, Memory, RandomFunction, PRNG, Instance>::~vOW()
{
    delete memory;
    delete instance;

    if (dist_cols.size != 0)
    {
        freeTree<Point>(dist_cols.root);
    }

    free(resync_state->cores);
    delete resync_state;
}



template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void vOW<Point, Memory, RandomFunction, PRNG, Instance>::reset()
{
    memory->reset();

    success = false;
    wall_time = 0.;
    total_time = 0.;
    final_avg_random_functions = 0.;
    collisions = 0;
    mem_collisions = 0;
    dist_points = 0;
    number_steps_collect = 0;
    number_steps_locate = 0;
    number_steps = 0;

    if (dist_cols.size != 0)
    {
        freeTree<Point>(dist_cols.root);
    }
    initTree<Point>(&dist_cols);

    /* resync */
    free(resync_state->cores);
    resync_state->cores = (uint8_t *)calloc(instance->N_OF_CORES, sizeof(uint8_t));
    // note, could use memset, but sometimes it causes Python to crash when called from SWIG
    // memset(resync_state->cores, 0, instance->N_OF_CORES * sizeof(*resync_state->cores));
}

#include "templating/vow.inc"