#include "lcg.hpp"
#include <stdio.h>

LCG::LCG(unsigned long _seed)
{
    // POSIX.1-2001, see 'srand' man page
    A = 1103515245;
    B = 12345;
    rand_max = 32768;
    seed(_seed);
}

void LCG::seed(unsigned long _seed)
{
    sampled = _seed;
}

void LCG::sample(unsigned char *buffer, unsigned long nbytes)
{
    while (nbytes-- > 0)
    {
        sampled = sampled * A + B;
        buffer[nbytes] = (unsigned char)((unsigned)((sampled / rand_max) >> 1) % rand_max);
    }
}