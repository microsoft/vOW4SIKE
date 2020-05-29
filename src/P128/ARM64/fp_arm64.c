/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: modular arithmetic optimized for 64-bit ARMv8 platforms for P128
*********************************************************************************************/

#include "../P128_internal.h"

// Global constants
extern const uint64_t p128[NWORDS_FIELD];
extern const uint64_t p128x2[NWORDS_FIELD];


__inline void fpadd128(const digit_t* a, const digit_t* b, digit_t* c)
{ // Modular addition, c = a+b mod p128.
  // Inputs: a, b in [0, 2*p128-1] 
  // Output: c in [0, 2*p128-1]

    fpadd128_asm(a, b, c);
} 


__inline void fpsub128(const digit_t* a, const digit_t* b, digit_t* c)
{ // Modular subtraction, c = a-b mod p128.
  // Inputs: a, b in [0, 2*p128-1] 
  // Output: c in [0, 2*p128-1] 

    fpsub128_asm(a, b, c);
}


__inline void fpneg128(digit_t* a)
{ // Modular negation, a = -a mod p128.
  // Input/output: a in [0, 2*p128-1] 
    unsigned int i, borrow = 0;

    for (i = 0; i < NWORDS_FIELD; i++) {
        SUBC(borrow, ((digit_t*)p128x2)[i], a[i], borrow, a[i]); 
    }
}


void fpdiv2_128(const digit_t* a, digit_t* c)
{ // Modular division by two, c = a/2 mod p128.
  // Input : a in [0, 2*p128-1] 
  // Output: c in [0, 2*p128-1] 
    unsigned int i, carry = 0;
    digit_t mask;
        
    mask = 0 - (digit_t)(a[0] & 1);    // If a is odd compute a+p521
    for (i = 0; i < NWORDS_FIELD; i++) {
        ADDC(carry, a[i], ((digit_t*)p128)[i] & mask, carry, c[i]); 
    }

    mp_shiftr1(c, NWORDS_FIELD);
} 


void fpcorrection128(digit_t* a)
{ // Modular correction to reduce field element a in [0, 2*p128-1] to [0, p128-1].
    unsigned int i, borrow = 0;
    digit_t mask;

    for (i = 0; i < NWORDS_FIELD; i++) {
        SUBC(borrow, a[i], ((digit_t*)p128)[i], borrow, a[i]);
    }
    mask = 0 - (digit_t)borrow;

    borrow = 0;
    for (i = 0; i < NWORDS_FIELD; i++) {
        ADDC(borrow, a[i], ((digit_t*)p128)[i] & mask, borrow, a[i]);
    }
}


void mp_mul(const digit_t* a, const digit_t* b, digit_t* c, const unsigned int nwords)
{ // Multiprecision multiply, c = a*b, where lng(a) = lng(b) = nwords.
        
    (void)nwords;
    mul128_asm(a, b, c);
}



void rdc_mont(digit_t* ma, digit_t* mc)
{ // Montgomery reduction exploiting special form of the prime.
  // mc = ma*R^-1 mod p128x2, where R = 2^128.
  // If ma < 2^128*p128, the output mc is in the range [0, 2*p128-1].
  // ma is assumed to be in Montgomery representation.
  
    rdc128_asm(ma, mc);
}
