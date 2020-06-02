#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "instance.hpp"
#include "../prng/xof.h"

GenInstance::GenInstance(GenInstance *inst)
{

    // parameters
    RAD = inst->RAD;
    NBITS_STATE = inst->NBITS_STATE;
    MEMORY_LOG_SIZE = inst->MEMORY_LOG_SIZE;
    PRNG_SEED = inst->PRNG_SEED;
    ALPHA = inst->ALPHA;
    BETA = inst->BETA;
    GAMMA = inst->GAMMA;
    N_OF_CORES = inst->N_OF_CORES;
    HANSEL_GRETEL = inst->HANSEL_GRETEL;
    MAX_CRUMBS = inst->MAX_CRUMBS;

    // processed parameters
    NBYTES_STATE = inst->NBYTES_STATE;
    NWORDS_STATE = inst->NWORDS_STATE;
    NBITS_OVERFLOW = inst->NBITS_OVERFLOW;
    MEMORY_SIZE = inst->MEMORY_SIZE;
    MEMORY_SIZE_MASK = inst->MEMORY_SIZE_MASK;
    MAX_STEPS = inst->MAX_STEPS;
    MAX_DIST = inst->MAX_DIST;
    MAX_FUNCTION_VERSIONS = inst->MAX_FUNCTION_VERSIONS;
    DIST_BOUND = inst->DIST_BOUND;
    THETA = inst->THETA;

    initial_function_version = inst->initial_function_version;
    collect_vow_stats = inst->collect_vow_stats;
};

void GenInstance::process()
{
    RAD = 64; /* TODO: Assume 64-bit arch?? */
    NBYTES_STATE = ((NBITS_STATE + 7) / 8);         /* Number of bytes needed for state */
    NWORDS_STATE = ((NBITS_STATE + RAD - 1) / RAD); /* Number of words need for state */
    NBITS_OVERFLOW = (NBITS_STATE % 8);

    /* Initialize memory */
    MEMORY_SIZE = (uint64_t)(1) << MEMORY_LOG_SIZE;
    MEMORY_SIZE_MASK = MEMORY_SIZE - 1;

    assert(MEMORY_LOG_SIZE < 64); /* Assumes that MEMORY_SIZE < 2^RADIX */
    /* Initialize vOW params */
    THETA = ALPHA * sqrt((double)MEMORY_SIZE / (double)pow(2, NBITS_STATE));
    MAX_DIST = (uint64_t)(BETA * MEMORY_SIZE);
    MAX_STEPS = ceil(GAMMA / THETA);
    MAX_FUNCTION_VERSIONS = 10000; // noqa
    if (NBITS_STATE <= MEMORY_LOG_SIZE)
    {
        printf("\nToo much memory for too small state, can't use last word to distinguish points.\n\n");
        exit(1);
    }
    if (NBITS_STATE - MEMORY_LOG_SIZE <= 64) {
        DIST_BOUND = THETA * pow(2, NBITS_STATE - MEMORY_LOG_SIZE);
    }
    else { /* Use max precision */
        DIST_BOUND = THETA * pow(2,64);
    }
    assert(NBITS_STATE > MEMORY_LOG_SIZE); // > -log(theta), we want some resolution

    XOF((unsigned char *)&initial_function_version,
		(unsigned char *)&PRNG_SEED,
		(unsigned long)sizeof(initial_function_version),
		(unsigned long)sizeof(PRNG_SEED),
		(unsigned long)0x72697175656c6d65);

    processed = true;
}

