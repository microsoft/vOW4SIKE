#pragma once
#include "triples.h"
#include "bintree.h"
#include "instance.h"
#include "../prng.h"
#include "../storage.h"
#include "../settings.h"
#if defined(VOW_SIDH) || defined(VOW_SIKE)
    #include "../curve_math.h"
#endif

typedef struct
{
    /* state */
    trip_t *memory; /* Of size MEMORY SIZE */
    uint64_t initial_function_version;

    /* resync */
    uint8_t *resync_cores;
    uint64_t resync_frequency;
    uint64_t resync_function_version;
    uint64_t resync_random_functions;

    /* State parameters */
    instance_t *instance;
    uint64_t NBITS_STATE;
    uint64_t NBYTES_STATE;
    uint64_t NWORDS_STATE;
    uint64_t NBITS_OVERFLOW;

    /* vOW parameters */
    uint64_t MEMORY_LOG_SIZE;
    uint64_t MEMORY_SIZE;
    uint64_t MEMORY_SIZE_MASK;
    double MAX_STEPS;
    uint64_t MAX_DIST;
    uint64_t MAX_FUNCTION_VERSIONS;
    double DIST_BOUND;

    /* Hansel&Gretel optimization*/
    bool HANSEL_GRETEL;
    uint64_t MAX_CRUMBS;

    /* OpenMP parameters */
    uint16_t N_OF_CORES;

/* Isogeny specific properties */
#if (defined(VOW_SIDH) || defined(VOW_SIKE))

#if defined(VOW_SIDH)
    bool PRECOMP;
    CurveAndPointsSIDH E[2];
#elif defined(VOW_SIKE)
    CurveAndPointsSIDH *E[2];
    bool external_E[2];
#endif

    unsigned long delta; /* Depth of pre-computation */
    unsigned long *strat;
    unsigned long lenstrat;
    f2elm_t jinv;           /* For verifying */
    unsigned char *jinvLUT; // Look-up table for precomputing j-invariants

#else

    /* Generic preimages */
    st_t image;
    st_t preimages[2];

#endif

    /* Statistics */
    bool collect_vow_stats; /* Never terminate the run if true */
    binTree_t dist_cols;
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
    int64_t cpu_cycles;

    /* Prng */
    unsigned long PRNG_SEED;

/* Storage */
#ifdef STORE_IN_DATABASE
    db_settings_t *db_settings;
    storage_state_t storage_state;
    int STORAGE_TRIP_BYTELEN;
#endif

/* Debug */
#ifdef COLLECT_DATABASE_STATS
    double debug_stats[4];
// debug_stats[0] = n of reads
// debug_stats[1] = total read time
// debug_stats[2] = n of writes
// debug_stats[3] = total write time
#endif
} shared_state_t;

/* Hansel & Gretel */
typedef struct
{
    uint64_t max_crumbs;
    uint64_t *positions;
    uint64_t *index_crumbs;
    uint64_t *crumbs; //// Each crumb will fit in several words.
    uint64_t swap_position;
    uint64_t scratch_position;
    uint64_t index_position;
    uint64_t max_dist;
    uint64_t real_dist;
    uint64_t position;
    uint64_t num_crumbs;
} CrumbStruct, *PCrumbStruct;

typedef struct
{
    int thread_id;

    /* State */
    trip_t current;
    uint64_t current_dist;
    uint64_t function_version;
    uint64_t random_functions;

    /* Prng */
    uint64_t PRNG_SEED;
    prng_state_t prng_state;

    /* State parameters */
    uint64_t NBITS_STATE;
    uint64_t NBYTES_STATE;
    uint64_t NWORDS_STATE;
    uint64_t NBITS_OVERFLOW;

    /* vOW parameters */
    uint64_t MEMORY_LOG_SIZE;
    uint64_t MEMORY_SIZE;
    uint64_t MEMORY_SIZE_MASK;
    double MAX_STEPS;
    uint64_t MAX_DIST;
    uint64_t MAX_FUNCTION_VERSIONS;
    double DIST_BOUND;

/* Isogeny specific properties */
#if (defined(VOW_SIDH) || defined(VOW_SIKE))

#if defined(VOW_SIDH)
    bool PRECOMP;
    CurveAndPointsSIDH E[2];
#elif defined(VOW_SIKE)
    CurveAndPointsSIDH *E[2];
    bool external_E[2];
#endif

    unsigned long delta; /* Depth of pre-computation */
    unsigned long *strat;
    unsigned long lenstrat;
    f2elm_t jinv;           /* For verifying */
    unsigned char *jinvLUT; // Look-up table for precomputing j-invariants

#else

    /* Generic preimages */
    st_t image;
    st_t preimages[2];

#endif

    /* Statistics */
    bool collect_vow_stats; /* Never terminate the run if true */
    binTree_t dist_cols;
    uint64_t iterations;
    uint64_t collisions;
    uint64_t mem_collisions;
    uint64_t dist_points;
    uint64_t number_steps_collect; /* Counts function evaluations for collecting distinguished points.*/
    uint64_t number_steps_locate;  /* Counts function evaluations during collision locating. */

    /* Storage */
    trip_t trip;
#ifdef STORE_IN_DATABASE
    unsigned char *storage_temp_buffer[1]; // each thread should have its own
    trip_t storage_dummy_trip;
#endif

    /* Hansel & Gretel */
    bool HANSEL_GRETEL;
    CrumbStruct crumbs;
} private_state_t;
