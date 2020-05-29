#pragma once

#include "config.h"

// prng selection
#define USE_AES_PRNG    // If commented out it uses linear rand

// XOF selection
//#define USE_XXHASH_XOF
#define USE_AES_XOF

#if defined(USE_AES_PRNG) || defined(USE_AES_XOF)
#include "aes/aes.h"
#endif

#if defined(USE_XXHASH_XOF)
#include "xxhash/xxhash.h"
#else
#include "sha3/fips202.h"
#endif

typedef struct
{
#if defined(USE_AES_PRNG)
    unsigned char aes_key_schedule[16 * 11] ALIGN_FOOTER(32);
    uint64_t count;
#else
    unsigned long A;
    unsigned long B;
    unsigned long rand_max;
    unsigned long sampled;
#endif
} prng_state_t;

void init_prng(prng_state_t *state, unsigned long seed);
void sample_prng(prng_state_t *state, unsigned char *buffer, unsigned long nbytes);
void XOF(unsigned char *output, unsigned char *input, unsigned long nbytes_output, unsigned long nbytes_input, unsigned long salt);
