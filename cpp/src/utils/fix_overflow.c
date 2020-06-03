
#include "fix_overflow.h"

void fix_overflow(unsigned char *buf, const uint64_t len, const uint64_t nbits_overflow)
{
    if (nbits_overflow != 0)
    {
        buf[len - 1] &= (0xFF >> (8 - nbits_overflow));
    }
}