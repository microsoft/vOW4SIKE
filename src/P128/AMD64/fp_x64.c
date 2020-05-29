/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: modular arithmetic optimized for x64 platforms for P128
*********************************************************************************************/

#include "../P128_internal.h"

// Global constants
extern const uint64_t p128[NWORDS_FIELD];
extern const uint64_t Montgomery_rprime1[NWORDS_FIELD]; 
extern const uint64_t p128x2[NWORDS_FIELD]; 


__inline void fpadd128(const digit_t* a, const digit_t* b, digit_t* c)
{ // Modular addition, c = a+b mod p128.
  // Inputs: a, b in [0, 2*p128-1] 
  // Output: c in [0, 2*p128-1] 
    
#if (OS_TARGET == OS_WIN)
    unsigned int i, carry = 0;
    digit_t mask;

    for (i = 0; i < NWORDS_FIELD; i++) {
        ADDC(carry, a[i], b[i], carry, c[i]); 
    }

    carry = 0;
    for (i = 0; i < NWORDS_FIELD; i++) {
        SUBC(carry, c[i], ((digit_t*)p128x2)[i], carry, c[i]); 
    }
    mask = 0 - (digit_t)carry;

    carry = 0;
    for (i = 0; i < NWORDS_FIELD; i++) {
        ADDC(carry, c[i], ((digit_t*)p128x2)[i] & mask, carry, c[i]); 
    } 
    
#elif (OS_TARGET == OS_LINUX)                 
    
    fpadd128_asm(a, b, c);    

#endif
} 


__inline void fpsub128(const digit_t* a, const digit_t* b, digit_t* c)
{ // Modular subtraction, c = a-b mod p128.
  // Inputs: a, b in [0, 2*p128-1] 
  // Output: c in [0, 2*p128-1] 
    
#if (OS_TARGET == OS_WIN)
    unsigned int i, borrow = 0;
    digit_t mask;

    for (i = 0; i < NWORDS_FIELD; i++) {
        SUBC(borrow, a[i], b[i], borrow, c[i]); 
    }
    mask = 0 - (digit_t)borrow;

    borrow = 0;
    for (i = 0; i < NWORDS_FIELD; i++) {
        ADDC(borrow, c[i], ((digit_t*)p128x2)[i] & mask, borrow, c[i]); 
    }
    
#elif (OS_TARGET == OS_LINUX)                 
    
    fpsub128_asm(a, b, c);    

#endif
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
        
    mask = 0 - (digit_t)(a[0] & 1);    // If a is odd compute a+p128
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

#if (OS_TARGET == OS_WIN)
    digit_t t = 0;
    uint128_t uv;
    unsigned int carry;
        
    MUL128(a[0], b[0], uv);
    c[0] = uv[0];
    uv[0] = uv[1];
    uv[1] = 0;
    
    MULADD128(a[0], b[1], uv, carry, uv);
    t += carry;
    MULADD128(a[1], b[0], uv, carry, uv);
    t += carry;
    c[1] = uv[0];
    uv[0] = uv[1];
    uv[1] = t;
    
    MULADD128(a[1], b[1], uv, carry, uv);
    c[2] = uv[0];
    c[3] = uv[1];

#elif (OS_TARGET == OS_LINUX)
    
    mul128_asm(a, b, c);

#endif
}


void rdc_mont(digit_t* ma, digit_t* mc)
{ // Efficient Montgomery reduction using comba and exploiting the special form of the prime p128.
  // mc = ma*R^-1 mod p128x2, where R = 2^128.
  // If ma < 2^128*p128, the output mc is in the range [0, 2*p128-1].
  // ma is assumed to be in Montgomery representation.
        
#if (OS_TARGET == OS_WIN)
    digit_t t = 0;
    uint128_t uv, temp;
    unsigned int carry;
        
    // mc = ma x p128' mod 2^128 
    MUL128(ma[0], ((digit_t*)Montgomery_rprime1)[0], uv);
    mc[0] = uv[0];
    uv[0] = uv[1];
    uv[1] = 0;
    
    MULADD128(ma[0], ((digit_t*)Montgomery_rprime1)[1], uv, carry, uv);
    MULADD128(ma[1], ((digit_t*)Montgomery_rprime1)[0], uv, carry, uv);
    mc[1] = uv[0];
        
    // mc = (ma + mc x p128)/2^128
    MUL128(mc[0], ((digit_t*)p128)[0], uv);
    temp[0] = uv[0];
    uv[0] = uv[1];
    uv[1] = 0;
    
    MULADD128(mc[0], ((digit_t*)p128)[1], uv, carry, uv);
    t += carry;
    MULADD128(mc[1], ((digit_t*)p128)[0], uv, carry, uv);
    t += carry;
    temp[1] = uv[0];
    uv[0] = uv[1];
    uv[1] = t;
    
    MULADD128(mc[1], ((digit_t*)p128)[1], uv, carry, uv);
    carry = _addcarry_u64(0, ma[0], temp[0], &temp[0]);
    carry = _addcarry_u64(carry, ma[1], temp[1], &temp[1]); 
    carry = _addcarry_u64(carry, ma[2], uv[0], &mc[0]); 
    carry = _addcarry_u64(carry, ma[3], uv[1], &mc[1]); 
    
#elif (OS_TARGET == OS_LINUX)                 
    
    rdc128_asm(ma, mc);    

#endif
}