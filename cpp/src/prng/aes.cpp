#include "aes.hpp"

AESPRNG::AESPRNG(unsigned long _seed)
{
    seed(_seed);
}

void AESPRNG::seed(unsigned long _seed)
{
    unsigned char inp[16] = {0}, i;

    count = 0;
    for (i = 0; i < 4; i++)
    {
        inp[i] = (_seed >> 8 * i) & 0xFF; /* length of seed = 32 bits.. */
    }
    AES128_load_schedule(inp, aes_key_schedule);
}

/* TODO: Assumes that count doesnt exceed 64-bits */
void AESPRNG::sample(unsigned char *buffer, unsigned long nbytes)
{
    unsigned char inp[16] = {0};

    while(nbytes > 16) {
        *((uint64_t *)inp) = ++count;
        AES128_enc(inp, aes_key_schedule, buffer, 16);
        nbytes -= 16;
        buffer += 16;
    }
    *((uint64_t *)inp) = ++count;
    AES128_enc(inp, aes_key_schedule, buffer, nbytes);
}