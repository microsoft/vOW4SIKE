#include <stdio.h>
#include <time.h>
#include <omp.h>
#include "settings.h"
#include "triples.h"
#include "bintree.h"
#include "gen_vow.h"
#include "memory.h"
#include "vow.c"

// Functions for swig interface
#include "swig_helpers.c"
#include "state.c"

// Print statements for debugging
void print_st(st_t *s, shared_state_t *shared_state)
{
    uint64_t i;

    if (s->bytes == NULL) {
        for (i = 0; i < shared_state->NBYTES_STATE; i++)
            printf("--");
        return;
    }
    for (i = 0; i < shared_state->NBYTES_STATE; i++)
        printf("%02x", s->bytes[i]);
}

void print_trip(trip_t *t, shared_state_t *shared_state)
{
    print_st(&t->initial_state, shared_state);
    printf(" %lu ", (unsigned long)t->current_steps);
    print_st(&t->initial_state, shared_state);
    printf("\n");
}

/*
  ______                               
 /      \                              
|  $$$$$$\ ______    ______    ______  
| $$_  \$$/      \  /      \  /      \ 
| $$ \   |  $$$$$$\|  $$$$$$\|  $$$$$$\
| $$$$   | $$   \$$| $$    $$| $$    $$
| $$     | $$      | $$$$$$$$| $$$$$$$$
| $$     | $$       \$$     \ \$$     \
 \$$      \$$        \$$$$$$$  \$$$$$$$
                                       
                                                                    |  \
 ______ ____    ______   ______ ____    ______    ______   __    __ | $$
|      \    \  /      \ |      \    \  /      \  /      \ |  \  |  \| $$
| $$$$$$\$$$$\|  $$$$$$\| $$$$$$\$$$$\|  $$$$$$\|  $$$$$$\| $$  | $$| $$
| $$ | $$ | $$| $$    $$| $$ | $$ | $$| $$  | $$| $$   \$$| $$  | $$ \$$
| $$ | $$ | $$| $$$$$$$$| $$ | $$ | $$| $$__/ $$| $$      | $$__/ $$ __ 
| $$ | $$ | $$ \$$     \| $$ | $$ | $$ \$$    $$| $$       \$$    $$|  \
 \$$  \$$  \$$  \$$$$$$$ \$$  \$$  \$$  \$$$$$$  \$$       _\$$$$$$$ \$$
                                                          |  \__| $$    
                                                           \$$    $$    
                                                            \$$$$$$     
*/

/* Initializations */
static st_t init_st(uint64_t nwords_state)
{
    st_t s;
    s.words = calloc(nwords_state, sizeof(digit_t));
    return s;
}

static void free_st(st_t *s)
{
    free(s->words);
}

/* Simple functions on states */
static void copy_st(st_t *r, const st_t *s, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
        r->words[i] = s->words[i];
}

static void copy_st2uint64(uint64_t *r, const st_t *s, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
        r[i] = s->words[i];
}

static void copy_uint642st(st_t *r, const uint64_t *s, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
        r->words[i] = s[i];
}


/* TODO: Include a seed here.. */
void init_shared_state(instance_t *inst, shared_state_t *S
#ifdef STORE_IN_DATABASE
                       ,
                       db_settings_t *db_settings
#endif
)
{
    /* Initialize state */
    S->instance = inst;
    S->NBITS_STATE = inst->NBITS_STATE;
    S->NBYTES_STATE = ((inst->NBITS_STATE + 7) / 8);                 /* Number of bytes needed for state */
    S->NWORDS_STATE = ((inst->NBITS_STATE + RADIX64 - 1) / RADIX64); /* Number of words need for state */
    S->NBITS_OVERFLOW = (inst->NBITS_STATE % 8);
    S->PRNG_SEED = (unsigned long)inst->PRNG_SEED;

    /* Initialize memory */
    S->MEMORY_LOG_SIZE = inst->MEMORY_LOG_SIZE;
    S->MEMORY_SIZE = (uint64_t)(1 << S->MEMORY_LOG_SIZE);
    S->MEMORY_SIZE_MASK = S->MEMORY_SIZE - 1;
    assert(S->MEMORY_LOG_SIZE < 64); /* Assumes that MEMORY_SIZE <= 2^RADIX */
    /* Prepare the global memory */
    if (initialize_shared_memory(S) == EXIT_FAILURE) {
        printf("Error initialising shared memory\n");
        assert(0);
    }

    /* Initialize omp params */
    S->N_OF_CORES = inst->N_OF_CORES;

#ifdef STORE_IN_DATABASE
    /* db settings */
    S->db_settings = db_settings;
#endif

    /* Initialise H&G */
    S->HANSEL_GRETEL = inst->HANSEL_GRETEL;
    S->MAX_CRUMBS = inst->MAX_CRUMBS;

    /* Initialize vOW params */
    double THETA = inst->ALPHA * sqrt((double)S->MEMORY_SIZE / (double)pow(2,(double)inst->NBITS_STATE));
    S->MAX_DIST = (uint64_t)(inst->BETA * S->MEMORY_SIZE);
    S->MAX_STEPS = ceil(inst->GAMMA / THETA);
    S->MAX_FUNCTION_VERSIONS = 10000;
    if (inst->NBITS_STATE <= S->MEMORY_LOG_SIZE)
        printf("\nToo much memory for too small state, can't use last word to distinguish points.\n\n");
    // Assumes we are not filling digit_t, should shift by min(sizeof(digit_t)*8, inst->NBITS_STATE) - S->MEMORY_LOG_SIZE
    if (inst->NBITS_STATE - S->MEMORY_LOG_SIZE <= 64) {
        S->DIST_BOUND = THETA * pow(2,(double)(inst->NBITS_STATE - S->MEMORY_LOG_SIZE));
    } else { /* Use max precision */
        S->DIST_BOUND = THETA * pow(2,64);
    }
    assert(inst->NBITS_STATE > S->MEMORY_LOG_SIZE); // > -log(theta), we want some resolution

    /* Statistics */
    S->collect_vow_stats = false; // By default don't collect stats (=> terminate run when successful)
    initTree(&S->dist_cols);      // Initing even if not using
    S->success = false;
    S->wall_time = 0.;
    S->collisions = 0;
    S->mem_collisions = 0;
    S->dist_points = 0;
    S->number_steps_collect = 0;
    S->number_steps_locate = 0;
    S->number_steps = 0;
    S->initial_function_version = 1;
    S->final_avg_random_functions = 0.;

    /* Resync */
    S->resync_frequency = 1;
    S->resync_cores = (uint8_t *)calloc(S->N_OF_CORES, sizeof(uint8_t));

    /* Initial golden preimages and image, uses a temporary prng state */
    prng_state_t prng_state;
    init_prng(&prng_state, S->PRNG_SEED);
    S->image.words = calloc(S->NWORDS_STATE, sizeof(digit_t));
    S->preimages[0].words = calloc(S->NWORDS_STATE, sizeof(digit_t));
    S->preimages[1].words = calloc(S->NWORDS_STATE, sizeof(digit_t));
    sample_prng(&prng_state, S->image.bytes, (unsigned long)S->NBYTES_STATE);
    sample_prng(&prng_state, S->preimages[0].bytes, (unsigned long)S->NBYTES_STATE);
    do
    {
        sample_prng(&prng_state, S->preimages[1].bytes, (unsigned long)S->NBYTES_STATE);
    } while (is_equal_st(&S->preimages[0], &S->preimages[1], S->NWORDS_STATE));
    fix_overflow(&S->image, S->NBYTES_STATE, S->NBITS_OVERFLOW);
    fix_overflow(&S->preimages[0], S->NBYTES_STATE, S->NBITS_OVERFLOW);
    fix_overflow(&S->preimages[1], S->NBYTES_STATE, S->NBITS_OVERFLOW);
}

void free_shared_state(shared_state_t *S)
{
    cleanup_shared_memory(S);

    free(S->image.words);
    free(S->preimages[0].words);
    free(S->preimages[1].words);

    if (S->dist_cols.size != 0) {
        freeTree(S->dist_cols.root);
    }
    free(S->resync_cores);
}

//--------------------------------------------------------------------
static void SwapStGen(st_t *r, st_t *s, uint64_t nwords_state)
{
    st_t t = init_st(nwords_state);

    copy_st(&t, r, nwords_state);
    copy_st(r, s, nwords_state);
    copy_st(s, &t, nwords_state);
    free_st(&t);
}

bool is_equal_st(const st_t *s, const st_t *t, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
    {
        if (s->words[i] != t->words[i])
            return false;
    }
    return true;
}

static bool is_equal_st_words(const st_t *s, const uint64_t *r, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
    {
        if (s->words[i] != r[i])
            return false;
    }
    return true;
}

void copy_trip(trip_t *t, const trip_t *u, const uint64_t nwords_state)
{
    copy_st(&t->current_state, &u->current_state, nwords_state);
    t->current_steps = u->current_steps;
    copy_st(&t->initial_state, &u->initial_state, nwords_state);
}

static void get_val(uint64_t *val, uint64_t NWORDS_STATE, digit_t *state_words, uint64_t MEMORY_LOG_SIZE)
{
    /* Use as many bits as possible, with a max of 64 (word size) 
     * This limits the precision of distinguishedness checking to 64 bits */
    if (NWORDS_STATE == 1) {
        *val = state_words[0] >> MEMORY_LOG_SIZE;
    } else {
        *val = (state_words[0] >> MEMORY_LOG_SIZE) | (state_words[1] << (64-MEMORY_LOG_SIZE));
    }
}

static void add_fv(uint64_t *val, const uint64_t fv, const uint64_t db)
{
    *val = *val + fv*db;
}

// /* Functions for vOW */
/**
 * @brief Evaluates whether a point is distinguished
 * 
 * @param S 
 * @param private_state 
 * @return true point is distinguished
 * @return false point is not distinguished
 */
bool DistinguishedGen(private_state_t *private_state)
{
    /* Divide distinguishedness over interval to avoid bad cases */
    assert(private_state->MEMORY_LOG_SIZE > 0);
    uint64_t val;

    get_val(&val, private_state->NWORDS_STATE, private_state->current.current_state.words, private_state->MEMORY_LOG_SIZE);
    add_fv(&val, private_state->function_version, (uint64_t)private_state->DIST_BOUND);
    /* Zero top bits if < 64 bits filled */
    if ((private_state->NBITS_STATE - private_state->MEMORY_LOG_SIZE) <= 64) {
        val &= (((uint64_t)1 << (private_state->NBITS_STATE - private_state->MEMORY_LOG_SIZE)) - 1);
    }

    return (val <= (uint64_t)private_state->DIST_BOUND);
}

/**
 * @brief Recovers memory index for storing current distinguished point
 * 
 * @param S 
 * @param private_state 
 * @return uint64_t 
 */
uint64_t MemIndexGen(private_state_t *private_state)
{
    // Returns (lob + function) mod memsize
    return (uint64_t)((private_state->current.current_state.words[0] + private_state->random_functions) & private_state->MEMORY_SIZE_MASK);
}

/**
 * @brief Samples a new starting point
 * 
 * @param private_state pointer
 */
void SampleGen(private_state_t *private_state)
{
    sample_prng(&private_state->prng_state, private_state->current.current_state.bytes, (unsigned long)private_state->NBYTES_STATE);
    fix_overflow(&private_state->current.current_state, private_state->NBYTES_STATE, private_state->NBITS_OVERFLOW);
    private_state->current.current_steps = 0;
    copy_st(&private_state->current.initial_state, &private_state->current.current_state, private_state->NWORDS_STATE);

    // Hansel & Gretel
    clean_private_state(private_state);
}

static void UpdateStGen(st_t *r, const st_t *s, private_state_t *private_state)
{
    if (is_equal_st(s, &private_state->preimages[0], private_state->NWORDS_STATE) || is_equal_st(s, &private_state->preimages[1], private_state->NWORDS_STATE)) {
        copy_st(r, &private_state->image, private_state->NWORDS_STATE);
    } else {
        XOF(r->bytes, s->bytes, (unsigned long)private_state->NBYTES_STATE, (unsigned long)private_state->NBYTES_STATE, (unsigned long)private_state->function_version);
        fix_overflow(r, private_state->NBYTES_STATE, private_state->NBITS_OVERFLOW);

        if (is_equal_st(r, &private_state->image, private_state->NWORDS_STATE))
            r->words[0] += 1; // Force golden collision

        fix_overflow(r, private_state->NBYTES_STATE, private_state->NBITS_OVERFLOW);
    }
}

/**
 * @brief walk to the next point in the vOW, following the random function
 * 
 * @param S 
 * @param private_state 
 */
void UpdateGen(private_state_t *private_state)
{
    uint64_t i, temp;

    UpdateStGen(&private_state->current.current_state, &private_state->current.current_state, private_state);
    private_state->number_steps_collect += 1;

    if (private_state->HANSEL_GRETEL) {
        if (private_state->crumbs.num_crumbs < private_state->crumbs.max_crumbs) {
            copy_st2uint64(&private_state->crumbs.crumbs[private_state->crumbs.position], &private_state->current.current_state, private_state->NWORDS_STATE);
            private_state->crumbs.positions[private_state->crumbs.position] = private_state->crumbs.position;
            private_state->crumbs.index_crumbs[private_state->crumbs.position] = private_state->crumbs.position;
            private_state->crumbs.num_crumbs++;
        } else if (private_state->crumbs.position - private_state->crumbs.positions[private_state->crumbs.max_crumbs - 1] == private_state->crumbs.max_dist) {
            temp = private_state->crumbs.index_crumbs[private_state->crumbs.index_position];
            for (i = private_state->crumbs.index_position; i < private_state->crumbs.max_crumbs - 1; i++) { 
                // Updating table with crumb indices for the crump table
                private_state->crumbs.index_crumbs[i] = private_state->crumbs.index_crumbs[i + 1];
            }
            private_state->crumbs.index_crumbs[private_state->crumbs.max_crumbs - 1] = temp;
            private_state->crumbs.index_position++;
            if (private_state->crumbs.index_position > private_state->crumbs.max_crumbs - 1)
                private_state->crumbs.index_position = 0;
            copy_st2uint64(&private_state->crumbs.crumbs[temp], &private_state->current.current_state, private_state->NWORDS_STATE); // Inserting a new crumb at the end of the crumb table

            for (i = private_state->crumbs.scratch_position; i < private_state->crumbs.max_crumbs - 1; i++) { 
                // Updating table with crumb positions
                private_state->crumbs.positions[i] = private_state->crumbs.positions[i + 1];
            }
            private_state->crumbs.positions[private_state->crumbs.max_crumbs - 1] = private_state->crumbs.position;
            private_state->crumbs.swap_position += 2 * private_state->crumbs.real_dist;
            private_state->crumbs.scratch_position++; 
            if (private_state->crumbs.swap_position > private_state->crumbs.max_crumbs - 1) { 
                // Kind of cumbersome, maybe this can be simplified (but not time critical)
                private_state->crumbs.swap_position = 0;
                private_state->crumbs.real_dist <<= 1;
            }
            if (private_state->crumbs.scratch_position > private_state->crumbs.max_crumbs - 1) {
                private_state->crumbs.scratch_position = 0;
                private_state->crumbs.max_dist <<= 1;
                private_state->crumbs.swap_position = private_state->crumbs.max_dist;
            }
        }
        private_state->crumbs.position++;
    }
}

/**
 * @brief generate a new random function to walk on
 * 
 * @param S 
 * @param private_state 
 */
void UpdateRandomFunctionGen(shared_state_t *S, private_state_t *private_state)
{
    private_state->function_version++;
    // Reset "resync done" flag
    if (private_state->thread_id == 0) {
        S->resync_cores[0] = 0;
    }
}

static inline bool BacktrackGen_core(trip_t *c0, trip_t *c1, shared_state_t *S, private_state_t *private_state)
{
    uint64_t L;
    uint64_t i;
    st_t c0_;
    st_t c1_;
    (void)private_state;

    // Make c0 have the largest number of steps
    if (c0->current_steps < c1->current_steps) {
        SwapStGen(&c0->initial_state, &c1->initial_state, private_state->NWORDS_STATE);
        L = (unsigned long)(c1->current_steps - c0->current_steps);
    } else {
        L = (unsigned long)(c0->current_steps - c1->current_steps);
    }

    // Catch up the trails
    for (i = 0; i < L; i++) {
        UpdateStGen(&c0->initial_state, &c0->initial_state, private_state);
        private_state->number_steps_locate += 1;
    }

    if (is_equal_st(&c0->initial_state, &c1->initial_state, private_state->NWORDS_STATE))
        return false; // Robin Hood

    c0_ = init_st(private_state->NWORDS_STATE);
    c1_ = init_st(private_state->NWORDS_STATE);

    for (i = 0; i < c1->current_steps + 1; i++) {
        UpdateStGen(&c0_, &c0->initial_state, private_state);
        private_state->number_steps_locate += 1;
        UpdateStGen(&c1_, &c1->initial_state, private_state);
        private_state->number_steps_locate += 1;

        if (is_equal_st(&c0_, &c1_, private_state->NWORDS_STATE)) {
            /* Record collision */
            private_state->collisions += 1;
            if (private_state->collect_vow_stats)
            {
#pragma omp critical
                {
                    insertTree(&S->dist_cols, c0->initial_state, c1->initial_state, private_state->NWORDS_STATE);
                }
            }

            if (is_equal_st(&c0_, &private_state->image, private_state->NWORDS_STATE)) {
                free_st(&c0_);
                free_st(&c1_);
                return true;
            } else {
                free_st(&c0_);
                free_st(&c1_);
                return false;
            }
        } else {
            copy_st(&c0->initial_state, &c0_, private_state->NWORDS_STATE);
            copy_st(&c1->initial_state, &c1_, private_state->NWORDS_STATE);
        }
    }
    // If multiprocessing, it may reach here if a thread backtracks while another changes function version
    free_st(&c0_);
    free_st(&c1_);
    return false;
}

static inline bool BacktrackGen_Hansel_Gretel(trip_t *c_mem, trip_t *c_crumbs, shared_state_t *S, private_state_t *private_state)
{
    uint64_t L;
    trip_t c0_, cmem;
    uint64_t i, k, index;
    uint64_t crumb;
    bool resp, equal;

    cmem = init_trip(private_state->NWORDS_STATE);
    copy_trip(&cmem, c_mem, private_state->NWORDS_STATE);

    // Make the memory trail (without crumbs) at most the length of the crumbs trail.
    if (cmem.current_steps > c_crumbs->current_steps) {
        L = cmem.current_steps - c_crumbs->current_steps;
        for (i = 0; i < L; i++) {
            UpdateStGen(&cmem.initial_state, &cmem.initial_state, private_state);
            private_state->number_steps_locate += 1;
        }
        cmem.current_steps = c_crumbs->current_steps;
    }
    // Check for Robin Hood
    if (is_equal_st(&cmem.initial_state, &c_crumbs->initial_state, private_state->NWORDS_STATE)) {
        return false;
    }

    // The memory path is L steps shorter than the crumbs path.
    L = c_crumbs->current_steps - cmem.current_steps;
    k = 0;
    // Since there has been at least one step, there is at least one crumb.
    // Crumbs only store intermediate points, not the initial state and not
    // necessarily the current state.
    index = private_state->crumbs.positions[0] + 1;

    while ((L > index) && (k + 1 < private_state->crumbs.num_crumbs)) {
        // There are still crumbs to check and we haven't found the next crumb to reach.
        k++;
        index = private_state->crumbs.positions[k] + 1;
    }
    // Either have found the next crumb or ran out of crumbs to check.
    if (L > index) {
        // Ran out of crumbs to check, i.e. already in the interval beyond the last crumb.
        // Trails collide after last crumb.
        // Call original BacktrackGen on memory trail and shortened crumbs trail.
        copy_uint642st(&c_crumbs->initial_state, &private_state->crumbs.crumbs[private_state->crumbs.index_crumbs[k]], private_state->NWORDS_STATE);
        c_crumbs->current_steps -= (private_state->crumbs.positions[k] + 1);
        resp = BacktrackGen_core(&cmem, c_crumbs, S, private_state);
    } else {
        // Next crumb to check lies before (or is) the last crumb.
        c0_ = init_trip(private_state->NWORDS_STATE);
        copy_trip(&c0_, &cmem, private_state->NWORDS_STATE);

        do
        {
            cmem.current_steps = c0_.current_steps;
            copy_st(&cmem.initial_state, &c0_.initial_state, private_state->NWORDS_STATE);
            crumb = private_state->crumbs.crumbs[private_state->crumbs.index_crumbs[k]];
            index = private_state->crumbs.positions[k] + 1;

            L = cmem.current_steps - (c_crumbs->current_steps - index);
            for (i = 0; i < L; i++) {
                UpdateStGen(&c0_.initial_state, &c0_.initial_state, private_state);
                private_state->number_steps_locate += 1;
            }
            c0_.current_steps -= L;
            k++;
            equal = is_equal_st_words(&c0_.initial_state, &crumb, private_state->NWORDS_STATE);
        } while (!equal && k < private_state->crumbs.num_crumbs);
        // Either found the colliding crumb or moved to the interval beyond the last crumb.

        if (equal) { // Have a colliding crumb.
            copy_uint642st(&cmem.current_state, &crumb, private_state->NWORDS_STATE);
            cmem.current_steps -= c0_.current_steps;
            if (k == 1) {
                c0_.current_steps = private_state->crumbs.positions[0] + 1;
                copy_uint642st(&c0_.initial_state, c_crumbs->initial_state.words, private_state->NWORDS_STATE);
            } else {
                c0_.current_steps = private_state->crumbs.positions[k - 1] - private_state->crumbs.positions[k - 2];
                copy_uint642st(&c0_.initial_state, &private_state->crumbs.crumbs[private_state->crumbs.index_crumbs[k - 2]], private_state->NWORDS_STATE);
            }
            copy_uint642st(&c0_.current_state, &crumb, private_state->NWORDS_STATE);
        } else { // Collision happens after the last crumb.
            cmem.current_steps = c0_.current_steps;
            copy_uint642st(&cmem.initial_state, &crumb, private_state->NWORDS_STATE);
        }
        resp = BacktrackGen_core(&cmem, &c0_, S, private_state);
        free_trip(&c0_);
    }
    free_trip(&cmem);
    return resp;
}

bool BacktrackGen(trip_t *c0, trip_t *c1, shared_state_t *S, private_state_t *private_state)
{ // Backtrack function selection

    if (private_state->HANSEL_GRETEL)
        return BacktrackGen_Hansel_Gretel(c0, c1, S, private_state);
    else
        return BacktrackGen_core(c0, c1, S, private_state);
}
