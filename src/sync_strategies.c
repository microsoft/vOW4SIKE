// Utilities

void print_all_threads(shared_state_t *S, private_state_t *private_state, unsigned char *buffer)
{
#pragma omp critical
    {
        printf("\n%d: ", private_state->thread_id);
        for (int i = 0; i < S->N_OF_CORES; i++) {
            printf("%u ", buffer[i]);
        }
    }
}

/* 
    Windowed_resync: 
    every so many functions (the "window"), thread 0 forces the other threads to reset their state to its own. 
*/

bool windowed_resync_should_resync(shared_state_t *S, private_state_t *private_state)
{
    // Need a resync
    if (private_state->thread_id == 0) {
        return private_state->random_functions % S->resync_frequency == 0 && S->resync_cores[private_state->thread_id] == 0;
    } else {
        return S->resync_cores[0] == 1 && S->resync_cores[private_state->thread_id] == 0;
    }
}

void windowed_resync_do_resync(shared_state_t *S, private_state_t *private_state)
{
    uint16_t cores_that_resyncd = 0;

    if (private_state->thread_id == 0) {
        // Need to resync to this
        S->resync_function_version = private_state->function_version;
        S->resync_random_functions = private_state->random_functions;

        // Have resync'd
        S->resync_cores[private_state->thread_id] = 1;
    } else {
        // Need to resync
        private_state->function_version = S->resync_function_version;
        private_state->random_functions = S->resync_random_functions;
        sample(private_state);
        private_state->current_dist = 0;

        // Have resync'd
        S->resync_cores[private_state->thread_id] = 1;

        // Have all the others also resync'd?
        cores_that_resyncd = 0;
        for (int i = 0; i < S->N_OF_CORES; i++) {
            cores_that_resyncd += (uint16_t)(S->resync_cores[i] == 1);
        }
        if (cores_that_resyncd == S->N_OF_CORES) {
            memset(S->resync_cores, 0, S->N_OF_CORES);
            S->resync_cores[0] = 2;  // This flags "this random function has already been synced", it's reset inside update_random_function
        }
    }
}

void windowed_resync(shared_state_t *S, private_state_t *private_state)
{
    if (windowed_resync_should_resync(S, private_state)) {
        windowed_resync_do_resync(S, private_state);
    }
}

/* 
   Stakhanovist_resync: 
   as threads finish their portion, wait for others to catch up. In the meantime, they keep mining points.
*/

bool stakhanovist_resync_should_resync(shared_state_t *S, private_state_t *private_state)
{
    // In the real attack, and we are done with out official portion of the function
    // Signal we are done
    if (S->resync_cores[private_state->thread_id] == 0) {
        S->resync_cores[private_state->thread_id] = 1;
    }

    // Check if also others are done
    bool everybody_done = true;
    for (int i = 0; i < S->N_OF_CORES; i++)
    {
        everybody_done &= (S->resync_cores[i] > 0);
    }

    if (private_state->thread_id == 0) {
        // I'm the leader and I think everybody is done
        bool everybody_moved_on = true;
        for (int i = 1; i < S->N_OF_CORES; i++) {
            everybody_moved_on &= (S->resync_cores[i] == 2);
        }

        return everybody_done && everybody_moved_on;
    } else {
        // I'm not leader and I think everyone is done.
        bool have_I_already_increased_fun_version = S->resync_cores[private_state->thread_id] == 2;
        return everybody_done && !have_I_already_increased_fun_version;
    }
}

void stakhanovist_resync_do_resync(shared_state_t *S, private_state_t *private_state)
{
    if (private_state->thread_id == 0) {
        memset(S->resync_cores, 0, S->N_OF_CORES * sizeof(uint8_t));
        private_state->random_functions++; // maybe this could be merged with update_random_function
        private_state->current_dist = 0;
    } else {
        // Move to the next function, thread = 0 does it last, in the next block
        private_state->random_functions++; // Maybe this could be merged with update_random_function
        private_state->current_dist = 0;
        S->resync_cores[private_state->thread_id] = 2;
    }
}

/* 
    nobiggie sync:
    every thread mines their share and then waits for others to finish theirs before updating the random function.
    This could probably be obtained with a single omp barrier, but it would require changing a bit the layout of vOW_one_iteration()  
    (since we otherwise get stuck when the solution is found while other threads are at the barrier).
    An alternative is to busy wait for other threads, but that's energy inefficient.
    A more efficient option is to have signalling between threads, for example using sigwait(). Yet, this is openmp-implementation- and 
    os-dependent (e.g., gcc's openmp on linux should be using pthreads, so sigwait/pthread_sigmask should do it, see eg. https://www.shrubbery.net/solaris9ab/SUNWdev/MTP/p36.html). Seems a bit complex.
    A portable and cheaper alternative to busy waiting is to just sleep for a fixed small amount of time while waiting. This brings down thread cycles, though it looks clunky. For now we go with this.
*/

void nobiggie_sync_small_sleep()
{
#if (OS_TARGET == OS_WIN)
    Sleep((DWORD)1);
#elif (OS_TARGET == OS_LINUX)
    usleep(1);
#endif
}

bool nobiggie_resync_should_resync(shared_state_t *S, private_state_t *private_state, bool *success)
{
    if (S->resync_cores[private_state->thread_id] == 0) {
        S->resync_cores[private_state->thread_id] = 1;
    }

    bool everybody_done;
    int i;
    do
    {
        if (*success) {
            return false;  // Should not resync, rather return!
        }
        nobiggie_sync_small_sleep();
        everybody_done = true;
        for (i = 0; i < S->N_OF_CORES; i++) {
            everybody_done &= (S->resync_cores[i] > 0);
        }
    } while (!everybody_done);

    return true;  // Go ahead with resync
}

void nobiggie_resync_do_resync(shared_state_t *S, private_state_t *private_state)
{
    // From here on I know we are all without success
    sample(private_state);
    update_random_function(S, private_state);
    private_state->random_functions++;  // Maybe this could be merged with update_random_function
    private_state->current_dist = 0;

    S->resync_cores[private_state->thread_id] = 2;

    // We could use signals instead of sleep to avoid busy waiting. In practice this seems to do fine

    if (private_state->thread_id == 0) {
        // Wait for everybody to move on
        bool everybody_moved_on;
        int i;
        do
        {
            nobiggie_sync_small_sleep();
            everybody_moved_on = true;
            for (i = 1; i < S->N_OF_CORES; i++) {
                everybody_moved_on &= (S->resync_cores[i] == 2);
            }
        } while (!everybody_moved_on);
        memset(S->resync_cores, 0, S->N_OF_CORES * sizeof(uint8_t));
    }
}
