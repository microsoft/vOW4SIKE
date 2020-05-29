/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: utility functions for testing and benchmarking
*********************************************************************************************/

#include "test_extras.h"
#if (OS_TARGET == OS_WIN)
    #include <intrin.h>
    #include <windows.h>
#elif (OS_TARGET == OS_LINUX)
    #if (TARGET == TARGET_ARM64)
        #include <time.h>
    #endif
    #include <unistd.h>
#endif
#include <stdlib.h>     

#ifdef p_32_20           /* p128 = 2^32*3^20*23 - 1 */
    static uint64_t p128[2] = { 0xAC0E7A06FFFFFFFF, 0x0000000000000012 };
    #define NBITS_FIELD128     69
#elif defined p_36_22    /* p128 = 2^36*3^22*31 - 1 */
    static uint64_t p128[2] = { 0x02A0B06FFFFFFFFF, 0x0000000000000E28 };
    #define NBITS_FIELD128     76
#elif defined p_40_25    /* p128 = 2^40*3^25*71 - 1 */
    static uint64_t p128[2] = { 0xE5D334FFFFFFFFFF, 0x000000000036B681 };
    #define NBITS_FIELD128     87
#elif defined p_44_27    /* p128 = 2^44*3^27*37 - 1 */
    static uint64_t p128[2] = { 0x57606FFFFFFFFFFF, 0x000000001009C7E1 };
    #define NBITS_FIELD128     93
#elif defined p_48_30    /* p128 = 2^48*3^30*13 - 1 */
    static uint64_t p128[2] = { 0x2164FFFFFFFFFFFF, 0x000000098256F148 };
    #define NBITS_FIELD128    100
#elif defined p_52_33    /* p128 = 2^52*3^33*1 - 1 */
    static uint64_t p128[2] = { 0xB82FFFFFFFFFFFFF, 0x0000013BFEFA65AB };
    #define NBITS_FIELD128    105
#elif defined p_56_35    /* p128 = 2^56*3^35*57 - 1 */
    static uint64_t p128[2] = { 0x82FFFFFFFFFFFFFF, 0x0027939F3C5BD1C1 };
    #define NBITS_FIELD128    118
#elif defined p_60_38    /* p128 = 2^60*3^38*57 - 1 */
    static uint64_t p128[2] = { 0x0FFFFFFFFFFFFFFF, 0x42C91CB5DAF1F68D };
    #define NBITS_FIELD128    127
#else
    static uint64_t p128[2] = { 0, 0 };  // When no prime is used (in generic vOW)
    #define NBITS_FIELD128    0
#endif

static uint64_t p434[7]  = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFDC1767AE2FFFFFF, 
                             0x7BC65C783158AEA3, 0x6CFC5FD681C52056, 0x0002341F27177344 };
static uint64_t p503[8]  = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xABFFFFFFFFFFFFFF, 
                             0x13085BDA2211E7A0, 0x1B9BF6C87B7E7DAF, 0x6045C6BDDA77A4D0, 0x004066F541811E1E };
static uint64_t p751[12] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xEEAFFFFFFFFFFFFF,
                             0xE3EC968549F878A8, 0xDA959B1A13F7CC76, 0x084E9867D6EBE876, 0x8562B5045CB25748, 0x0E12909F97BADC66, 0x00006FE5D541F71C };

#define NBITS_FIELD434    434
#define NBITS_FIELD503    503
#define NBITS_FIELD751    751


int64_t cpucycles(void)
{ // Access system counter for benchmarking
#if (OS_TARGET == OS_WIN) && (TARGET == TARGET_AMD64)
    return __rdtsc();
#elif (OS_TARGET == OS_LINUX) && (TARGET == TARGET_AMD64)
    unsigned int hi, lo;

    asm volatile ("rdtsc\n\t" : "=a" (lo), "=d"(hi));
    return ((int64_t)lo) | (((int64_t)hi) << 32);
#elif (OS_TARGET == OS_LINUX) && (TARGET == TARGET_ARM64)
    struct timespec time;

    clock_gettime(CLOCK_REALTIME, &time);
    return (int64_t)(time.tv_sec*1e9 + time.tv_nsec);
#else
    return 0;            
#endif
}


int compare_words(digit_t* a, digit_t* b, unsigned int nwords)
{ // Comparing "nword" elements, a=b? : (1) a>b, (0) a=b, (-1) a<b
  // SECURITY NOTE: this function does not have constant-time execution. TO BE USED FOR TESTING ONLY.
    int i;

    for (i = nwords-1; i >= 0; i--)
    {
        if (a[i] > b[i]) return 1;
        else if (a[i] < b[i]) return -1;
    }

    return 0; 
}


static void sub_test(digit_t* a, digit_t* b, digit_t* c, unsigned int nwords)
{ // Subtraction without borrow, c = a-b where a>b
  // SECURITY NOTE: this function does not have constant-time execution. It is for TESTING ONLY.     
    unsigned int i;
    digit_t res, carry, borrow = 0;
  
    for (i = 0; i < nwords; i++)
    {
        res = a[i] - b[i];
        carry = (a[i] < b[i]);
        c[i] = res - borrow;
        borrow = carry || (res < borrow);
    } 
}


void fprandom128_test(digit_t* a)
{ // Generating a pseudo-random field element in [0, p128-1] 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.
    unsigned int i, diff = 128-NBITS_FIELD128, nwords = NBITS_TO_NWORDS(NBITS_FIELD128);                    
    unsigned char* string = NULL;

    string = (unsigned char*)a;
    for (i = 0; i < sizeof(digit_t)*nwords; i++) {
        *(string + i) = (unsigned char)rand();              // Obtain 128-bit number
    }
    a[nwords-1] &= (((digit_t)(-1) << diff) >> diff);

    while (compare_words((digit_t*)p128, a, nwords) < 1) {  // Force it to [0, modulus-1]
        sub_test(a, (digit_t*)p128, a, nwords);
    }
}


void fprandom434_test(digit_t* a)
{ // Generating a pseudo-random field element in [0, p434-1] 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.
    unsigned int i, diff = 448-NBITS_FIELD434, nwords = NBITS_TO_NWORDS(NBITS_FIELD434);
    unsigned char* string = NULL;

    string = (unsigned char*)a;
    for (i = 0; i < sizeof(digit_t)*nwords; i++) {
        *(string + i) = (unsigned char)rand();              // Obtain 448-bit number
    }
    a[nwords-1] &= (((digit_t)(-1) << diff) >> diff);

    while (compare_words((digit_t*)p434, a, nwords) < 1) {  // Force it to [0, modulus-1]
        sub_test(a, (digit_t*)p434, a, nwords);
    }
}


void fprandom503_test(digit_t* a)
{ // Generating a pseudo-random field element in [0, p503-1] 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.
    unsigned int i, diff = 512-NBITS_FIELD503, nwords = NBITS_TO_NWORDS(NBITS_FIELD503);
    unsigned char* string = NULL;

    string = (unsigned char*)a;
    for (i = 0; i < sizeof(digit_t)*nwords; i++) {
        *(string + i) = (unsigned char)rand();              // Obtain 512-bit number
    }
    a[nwords-1] &= (((digit_t)(-1) << diff) >> diff);

    while (compare_words((digit_t*)p503, a, nwords) < 1) {  // Force it to [0, modulus-1]
        sub_test(a, (digit_t*)p503, a, nwords);
    }
}


void fprandom751_test(digit_t* a)
{ // Generating a pseudo-random field element in [0, p751-1] 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.
    unsigned int i, diff = 768-NBITS_FIELD751, nwords = NBITS_TO_NWORDS(NBITS_FIELD751);
    unsigned char* string = NULL;

    string = (unsigned char*)a;
    for (i = 0; i < sizeof(digit_t)*nwords; i++) {
        *(string + i) = (unsigned char)rand();              // Obtain 768-bit number
    }
    a[nwords-1] &= (((digit_t)(-1) << diff) >> diff);

    while (compare_words((digit_t*)p751, a, nwords) < 1) {  // Force it to [0, modulus-1]
        sub_test(a, (digit_t*)p751, a, nwords);
    }
}


void fp2random128_test(digit_t* a)
{ // Generating a pseudo-random element in GF(p128^2) 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.

    fprandom128_test(a);
    fprandom128_test(a+NBITS_TO_NWORDS(NBITS_FIELD128));
}


void fp2random434_test(digit_t* a)
{ // Generating a pseudo-random element in GF(p434^2) 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.

    fprandom434_test(a);
    fprandom434_test(a+NBITS_TO_NWORDS(NBITS_FIELD434));
}


void fp2random503_test(digit_t* a)
{ // Generating a pseudo-random element in GF(p503^2) 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.

    fprandom503_test(a);
    fprandom503_test(a+NBITS_TO_NWORDS(NBITS_FIELD503));
}


void fp2random751_test(digit_t* a)
{ // Generating a pseudo-random element in GF(p751^2) 
  // SECURITY NOTE: distribution is not fully uniform. TO BE USED FOR TESTING ONLY.

    fprandom751_test(a);
    fprandom751_test(a+NBITS_TO_NWORDS(NBITS_FIELD751));
}


void sleep_ms(digit_t ms)
{
 #if (OS_TARGET == OS_WIN)
    Sleep((DWORD)ms);
#elif (OS_TARGET == OS_LINUX)
    usleep(ms*1000);
#endif
}