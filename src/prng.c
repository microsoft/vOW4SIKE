/********************************************************************************************
* PRNG based on AES
*********************************************************************************************/

#include <string.h>
#include "prng.h"

void init_prng(prng_state_t *state, unsigned long seed)
{
#if defined(USE_AES_PRNG)
    unsigned char inp[16] = {0}, i;

    state->count = 0;
    for (i = 0; i < 4; i++)
        inp[i] = (seed >> 8 * i) & 0xFF; /* length of seed = 32 bits.. */
    AES128_load_schedule(inp, state->aes_key_schedule);
#else
    // POSIX.1-2001, see 'srand' man page
    state->A = 1103515245;
    state->B = 12345;
    state->rand_max = 32768;
    state->sampled = seed;
#endif
}

/* Assumes that count doesn't exceed 64-bits */
void sample_prng(prng_state_t *state, unsigned char *buffer, unsigned long nbytes)
{
#if defined(USE_AES_PRNG)
    unsigned char inp[16] = {0};

    while(nbytes > 16) {
        *((uint64_t *)inp) = ++state->count;
        AES128_enc(inp, state->aes_key_schedule, buffer, 16);
        nbytes -= 16;
        buffer += 16;
    }
    *((uint64_t *)inp) = ++state->count;
    AES128_enc(inp, state->aes_key_schedule, buffer, nbytes);
#else
    while (nbytes-- > 0)
    {
        state->sampled = state->sampled * state->A + state->B;
        buffer[nbytes] = (unsigned char)((unsigned)((state->sampled / state->rand_max) >> 1) % state->rand_max);
    }
#endif
}

void XOF(unsigned char *output, unsigned char *input, unsigned long nbytes_output, unsigned long nbytes_input, unsigned long salt)
{
#ifdef USE_XXHASH_XOF
    // xxhash
    unsigned long long round_output = salt;
    int hash_round = 0;
    unsigned int i;

    for (i = 0; i < nbytes_output; i++) {
        switch (i & 7)
        {
        case 0:
            round_output = XXH64(input, nbytes_input, round_output);
            output[8 * hash_round] = ((unsigned char *)&round_output)[0];
            break;
        case 1:
            output[8 * hash_round + 1] = ((unsigned char *)&round_output)[1];
            break;
        case 2:
            output[8 * hash_round + 2] = ((unsigned char *)&round_output)[2];
            break;
        case 3:
            output[8 * hash_round + 3] = ((unsigned char *)&round_output)[3];
            break;
        case 4:
            output[8 * hash_round + 4] = ((unsigned char *)&round_output)[4];
            break;
        case 5:
            output[8 * hash_round + 5] = ((unsigned char *)&round_output)[5];
            break;
        case 6:
            output[8 * hash_round + 6] = ((unsigned char *)&round_output)[6];
            break;
        case 7:
            output[8 * hash_round + 7] = ((unsigned char *)&round_output)[7];
            hash_round++;
            break;
        }
    }

#elif defined(USE_AES_XOF)
    (void)nbytes_input;

    /* CBC-mode with IV = 0 */
    unsigned char aes_key_schedule[16 * 11];
    unsigned char key[16] = {0}, inp[16] = {0};
    unsigned char i,j;

    for (i = 0; i < 4; i++) 
        key[i] = ((unsigned char *)&salt)[i]; // Set key
    AES128_load_schedule(key, aes_key_schedule);

    if (nbytes_output < 17) {
        for (j = 0; j < nbytes_output; j++) inp[j] = input[j];
        AES128_enc(inp, aes_key_schedule, output, nbytes_output);
    } else {
        AES128_enc(input, aes_key_schedule, output, 16);
        nbytes_output -= 16;
        i = 0;
        while (nbytes_output > 16) {
            for (j = 0; j < 16; j++) inp[j] = input[16+i+j] ^ output[i+j];
            AES128_enc(inp, aes_key_schedule, output+16+i, 16);
            nbytes_output -= 16;
            i += 16;
        }
        for (j = 0; j < nbytes_output; j++) inp[j] = input[16+i+j] ^ output[i+j];
        for (j = (unsigned char)nbytes_output; j < 16; j++) inp[j] = output[i+j]; // Padding with zeroes
        AES128_enc(inp, aes_key_schedule, output+16+i, 16);
    }

#endif
}
