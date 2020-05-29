#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include "../settings.h"

#if defined(VOW_SIKE) || defined(VOW_SIDH)
#include "../curve_math.h" // to get CurveandPointsSIDH
#endif

/* For testing instances */
#if defined(VOW_SIDH)
typedef struct
{
    char MODULUS[10];
    unsigned long NBITS_K;
    unsigned long MEMORY_LOG_SIZE;
    double ALPHA;
    double BETA;
    double GAMMA;
    uint64_t PRNG_SEED;
    uint16_t N_OF_CORES;
    CurveAndPointsSIDH E[2];
    f2elm_t jinv; /* For verifying */
    bool PRECOMP;
    bool HANSEL_GRETEL;
    uint64_t MAX_CRUMBS;
} instance_t;

#elif defined(VOW_SIKE)

typedef struct
{
    char MODULUS[10];
    unsigned long e;
    unsigned long MEMORY_LOG_SIZE;
    double ALPHA;
    double BETA;
    double GAMMA;
    uint64_t PRNG_SEED;
    uint16_t N_OF_CORES;
    CurveAndPointsSIDH E[2];
    unsigned long delta; /* Depth of pre-computation */
    f2elm_t jinv;        /* For verifying */
    bool HANSEL_GRETEL;
    uint64_t MAX_CRUMBS;
} instance_t;

#else // generic function

/* For testing instances */
typedef struct
{
    uint64_t NBITS_STATE;
    uint64_t MEMORY_LOG_SIZE;
    double ALPHA;
    double BETA;
    double GAMMA;
    unsigned long PRNG_SEED;
    uint16_t N_OF_CORES;
    bool HANSEL_GRETEL;
    uint64_t MAX_CRUMBS;
} instance_t;
#endif