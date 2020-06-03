#include <cstdio>
#include "config.h"
#include "sync_strategies.hpp"
extern "C"
{
#if (OS_TARGET == OS_WIN)
#include <intrin.h>
#include <windows.h>
#elif (OS_TARGET == OS_LINUX)
#if (TARGET == TARGET_ARM || TARGET == TARGET_ARM64)
#include <time.h>
#endif
#include <unistd.h>
#endif
}

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void print_all_threads(
    const vOW<Point, Memory, RandomFunction, PRNG, Instance> *vow,
    private_state_t<Point, PRNG, RandomFunction, Instance> &private_state,
    unsigned char *buffer)
{
    #pragma omp critical
    {
        printf("\n%d: ", private_state.thread_id);
        for (int i = 0; i < vow->instance->N_OF_CORES; i++)
        {
            printf("%u ", buffer[i]);
        }
    }
}

/* windowed_resync
 * every so many functions (the "window"), thread 0 forces the other threads to 
 * reset their state to its own.
*/

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool windowed_resync_should_resync(
    const vOW<Point, Memory, RandomFunction, PRNG, Instance> *vow,
    private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    // need a resync
    if (private_state.thread_id == 0)
    {
        return private_state.random_functions % vow->resync_state->frequency == 0 && vow->resync_state->cores[private_state.thread_id] == 0;
    }
    else
    {
        return vow->resync_state->cores[0] == 1 && vow->resync_state->cores[private_state.thread_id] == 0;
    }
}


template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void windowed_resync_do_resync(
    const vOW<Point, Memory, RandomFunction, PRNG, Instance> *S,
    private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    if (private_state.thread_id == 0)
    {
        // need to resync to this
        S->resync_state->function_version = private_state.step_function->function_version;
        S->resync_state->random_functions = private_state.random_functions;

        // have resync'd
        S->resync_state->cores[private_state.thread_id] = 1;
    }
    else
    {
        uint16_t cores_that_resyncd = 0;

        // need to resync
        private_state.step_function->function_version = S->resync_state->function_version;
        private_state.random_functions = S->resync_state->random_functions;
        private_state.current_dist = 0;

        // we currently have a point on the path of this new version
        // that comes from the previous one. We could get away with
        // only setting private_state.current->current_steps = 0
        // but let's just restart
        private_state.current->sample(private_state.prng);
        if (S->instance->HANSEL_GRETEL)
        {
            private_state.clean_crumbs();
        }

        // have resync'd
        S->resync_state->cores[private_state.thread_id] = 1;

        // have all the others also resync'd?
        for (int i = 0; i < S->instance->N_OF_CORES; i++)
        {
            cores_that_resyncd += (uint16_t)(S->resync_state->cores[i] == 1);
        }
        if (cores_that_resyncd == S->instance->N_OF_CORES)
        {
            memset(S->resync_state->cores, 0, S->instance->N_OF_CORES);
            // stop the master thread from asking to resync at every step of the current random function
            // gets cleared when the master thread updates its random function
            S->resync_state->cores[0] = 2;
        }
    }
}

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void windowed_resync(const vOW<Point, Memory, RandomFunction, PRNG, Instance> *vow, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    if (windowed_resync_should_resync(vow, private_state))
    {
        windowed_resync_do_resync(vow, private_state);
    }
}

/* stakhanovist_resync
 * as thread finish their portion, they wait for others to catch up.
 * in the meantime, they keep mining points.
*/

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool stakhanovist_resync_should_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    // we are running the real attack, and we are done with out official portion of the function
    // signal we are done
    if (S->resync_state->cores[private_state.thread_id] == 0)
    {
        S->resync_state->cores[private_state.thread_id] = 1;
    }

    // check if also others are done
    bool everybody_done = true;
    for (int i = 0; i < S->instance->N_OF_CORES; i++)
    {
        everybody_done &= (S->resync_state->cores[i] > 0);
    }

    if (private_state.thread_id == 0)
    {
        // I'm the leader and I think everybody is done.
        bool everybody_moved_on = true;
        for (int i = 1; i < S->instance->N_OF_CORES; i++)
        {
            everybody_moved_on &= (S->resync_state->cores[i] == 2);
        }

        return everybody_done && everybody_moved_on;
    }
    else
    {
        // I'm not leader and I think everyone is done.
        bool have_I_already_increased_fun_version = S->resync_state->cores[private_state.thread_id] == 2;
        return everybody_done && !have_I_already_increased_fun_version;
    }
}

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void stakhanovist_resync_do_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    if (private_state.thread_id == 0)
    {
        memset(S->resync_state->cores, 0, S->instance->N_OF_CORES * sizeof(uint8_t));
    }
    else
    {
        // move to the next function, thread = 0 does it last, in the next block
        S->resync_state->cores[private_state.thread_id] = 2;
    }
    private_state.random_functions++;
    private_state.current_dist = 0;
    private_state.current->sample(private_state.prng);
    if (S->instance->HANSEL_GRETEL)
    {
        private_state.clean_crumbs();
    }
    private_state.step_function->update();
}

/* nobiggie sync

 * every thread mines their share and then waits for others to finish theirs
 * before updating the random function.

 * this could probably be obtained with a single omp barrier, but it would
 * require changing a bit the layout of vow->iteration(), since we otherwise
 * get stuck when the solution is found while other threads are at the barrier.
 * An alternative is to busy wait for other threads, but that's energy inefficient.

 * A more efficient option is to have signalling between threads, for example
 * using sigwait(). Yet, this is openmp-implementation- and os-dependent
 * (eg, gcc's openmp on linux should be using pthreads, so sigwait/pthread_sigmask
 * should do it, see eg. https://www.shrubbery.net/solaris9ab/SUNWdev/MTP/p36.html).
 * Seems a bit complex.

 * A portable and cheaper alternative to busy waiting is to just sleep for a
 * fixed small amount of time while waiting. This brings down thread cycles,
 * though it looks clunky. For now we go with this.

*/

void nobiggie_sync_small_sleep(int s)
{
    #if (OS_TARGET == OS_WIN)
    Sleep((DWORD)s);
    #elif (OS_TARGET == OS_LINUX)
    usleep(s);
    #endif
}

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool nobiggie_resync_should_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    if (S->resync_state->cores[private_state.thread_id] == 0)
    {
        S->resync_state->cores[private_state.thread_id] = 1;
    }

    bool everybody_done;
    int i;
    do
    {
        if (S->success)
        {
            return false; // should not resync, rather return!
        }
        nobiggie_sync_small_sleep(1);
        everybody_done = true;
        for (i = 0; i < S->instance->N_OF_CORES; i++)
        {
            everybody_done &= (S->resync_state->cores[i] > 0);
        }
    } while (!everybody_done);

    return true; // go ahead with resync
}

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void nobiggie_resync_do_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state)
{
    // from here on I know we are all without success
    private_state.random_functions++;
    private_state.current_dist = 0;
    private_state.current->sample(private_state.prng);
    if (S->instance->HANSEL_GRETEL)
    {
        private_state.clean_crumbs();
    }
    private_state.step_function->update();

    S->resync_state->cores[private_state.thread_id] = 2;

    if (private_state.thread_id == 0)
    {
        // wait for everybody to move on
        bool everybody_moved_on;
        int i;
        do
        {
            nobiggie_sync_small_sleep(1);
            everybody_moved_on = true;
            for (i = 1; i < S->instance->N_OF_CORES; i++)
            {
                everybody_moved_on &= (S->resync_state->cores[i] == 2);
            }
        } while (!everybody_moved_on);
        memset(S->resync_state->cores, 0, S->instance->N_OF_CORES * sizeof(uint8_t));
    }
}

#include "templating/sync_strategies.inc"