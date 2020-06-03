#include "buftools.h"
#include "../config.h"

void *bufcpy(void *dest, const void *src, size_t n)
{
    {
        uint64_t i;
        uint64_t nwords_minus_one = n / sizeof(digit_t);

        // first copy using word-sized values as far as possible
        // is there a wmemcheck equivalent for 64-bit words?
        for (i = 0; i < nwords_minus_one; i++)
        {
            ((digit_t *)dest)[i] = ((digit_t *)src)[i];
        }

        // then check for remaning bytes
        for (i = nwords_minus_one * sizeof(digit_t); i < n; i++)
        {
            ((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
        }
    }

    return dest;
}

uint_fast8_t bufcmp(const void *s1, const void *s2, size_t n)
{
    uint64_t i;
    uint64_t nwords_minus_one = n / sizeof(digit_t);

    // first check using word-sized values as far as possible
    // is there a wmemcheck equivalent for 64-bit words?
    for (i = 0; i < nwords_minus_one; i++)
    {
        if (((digit_t *)s1)[i] != ((digit_t *)s2)[i])
        {
            return 1;
        }
    }

    // then check for remaning bytes
    for (i = nwords_minus_one * sizeof(digit_t); i < n; i++)
    {
        if (((unsigned char *)s1)[i] != ((unsigned char *)s2)[i])
        {
            return 1;
        }
    }

    // alternatively, for the last check
    // return (memcmp(
    //     s1 + nwords_minus_one * sizeof(digit_t), 
    //     s2 + nwords_minus_one * sizeof(digit_t), 
    //     n % sizeof(digit_t)
    // ) == 0);

    // note the above is faster than checking byte by byte, while allowing
    // non-multiple-of-sizeof(digit_t) state

    return 0;
}