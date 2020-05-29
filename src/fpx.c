/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: core functions over GF(p) and GF(p^2)
*********************************************************************************************/


void clear_words(void* mem, digit_t nwords)
{ // Clear digits from memory. "nwords" indicates the number of digits to be zeroed.
  // This function uses the volatile type qualifier to inform the compiler not to optimize out the memory clearing.
    unsigned int i;
    volatile digit_t *v = mem; 

    for (i = 0; i < nwords; i++) {
        v[i] = 0;
    }
}

void fp2_encode(const f2elm_t x, unsigned char *enc)
{ // Conversion of GF(p^2) element from Montgomery to standard representation, and encoding by removing leading 0 bytes
    unsigned int i;
    f2elm_t t;

    from_fp2mont(x, t);
    for (i = 0; i < FP2_ENCODED_BYTES / 2; i++) {
        enc[i] = ((unsigned char*)t)[i];
        enc[i + FP2_ENCODED_BYTES / 2] = ((unsigned char*)t)[i + MAXBITS_FIELD / 8];
    }
}

void fp2_decode(const unsigned char *enc, f2elm_t x)
{ // Parse byte sequence back into GF(p^2) element, and conversion to Montgomery representation
    unsigned int i;

    for (i = 0; i < 2*(MAXBITS_FIELD / 8); i++) ((unsigned char *)x)[i] = 0;
    for (i = 0; i < FP2_ENCODED_BYTES / 2; i++) {
        ((unsigned char*)x)[i] = enc[i];
        ((unsigned char*)x)[i + MAXBITS_FIELD / 8] = enc[i + FP2_ENCODED_BYTES / 2];
    }
    to_fp2mont(x, x);
}

__inline void fpcopy(const felm_t a, felm_t c)
{ // Copy a field element, c = a.
    unsigned int i;

    for (i = 0; i < NWORDS_FIELD; i++)
        c[i] = a[i];
}

__inline void fpzero(felm_t a)
{ // Zero a field element, a = 0.
    unsigned int i;

    for (i = 0; i < NWORDS_FIELD; i++)
        a[i] = 0;
}

void to_mont(const felm_t a, felm_t mc)
{   // Conversion to Montgomery representation,
    // mc = a*R^2*R^(-1) mod p = a*R mod p, where a in [0, p-1].
    // The Montgomery constant R^2 mod p is the global value "Montgomery_R2".

    fpmul_mont(a, (digit_t *)&Montgomery_R2, mc);
}

void from_mont(const felm_t ma, felm_t c)
{   // Conversion from Montgomery representation to standard representation,
    // c = ma*R^(-1) mod p = a mod p, where ma in [0, p-1].
    digit_t one[NWORDS_FIELD] = {0};

    one[0] = 1;
    fpmul_mont(ma, one, c);
    fpcorrection(c);
}

void copy_words(const digit_t *a, digit_t *c, const unsigned int nwords)
{ // Copy wordsize digits, c = a, where lng(a) = nwords.
    unsigned int i;

    for (i = 0; i < nwords; i++)
    {
        c[i] = a[i];
    }
}

void fpmul_mont(const felm_t ma, const felm_t mb, felm_t mc)
{ // Multiprecision multiplication, c = a*b mod p.
    dfelm_t temp = {0};

    mp_mul(ma, mb, temp, NWORDS_FIELD);
    rdc_mont(temp, mc);
}

void fpsqr_mont(const felm_t ma, felm_t mc)
{ // Multiprecision squaring, c = a^2 mod p.
    dfelm_t temp = {0};

    mp_mul(ma, ma, temp, NWORDS_FIELD);
    rdc_mont(temp, mc);
}

void fp2copy(const f2elm_t a, f2elm_t c)
{ // Copy a GF(p^2) element, c = a.
    fpcopy(a[0], c[0]);
    fpcopy(a[1], c[1]);
}

void fp2zero(f2elm_t a)
{ // Zero a GF(p^2) element, a = 0.
    fpzero(a[0]);
    fpzero(a[1]);
}

void fp2neg(f2elm_t a)
{ // GF(p^2) negation, a = -a in GF(p^2).
    fpneg(a[0]);
    fpneg(a[1]);
}

__inline void fp2add(const f2elm_t a, const f2elm_t b, f2elm_t c)
{ // GF(p^2) addition, c = a+b in GF(p^2).
    fpadd(a[0], b[0], c[0]);
    fpadd(a[1], b[1], c[1]);
}

__inline void fp2sub(const f2elm_t a, const f2elm_t b, f2elm_t c)
{ // GF(p^2) subtraction, c = a-b in GF(p^2).
    fpsub(a[0], b[0], c[0]);
    fpsub(a[1], b[1], c[1]);
}

void fp2div2(const f2elm_t a, f2elm_t c)
{ // GF(p^2) division by two, c = a/2  in GF(p^2).
    fpdiv2(a[0], c[0]);
    fpdiv2(a[1], c[1]);
}

void fp2correction(f2elm_t a)
{ // Modular correction, a = a in GF(p^2).
    fpcorrection(a[0]);
    fpcorrection(a[1]);
}

__inline static void mp_addfast(const digit_t *a, const digit_t *b, digit_t *c)
{ // Multiprecision addition, c = a+b.
#if (OS_TARGET == OS_WIN) || defined(GENERIC_IMPLEMENTATION)

    mp_add(a, b, c, NWORDS_FIELD);

#elif (OS_TARGET == OS_LINUX)

    mp_add_asm(a, b, c);

#endif
}

void fp2sqr_mont(const f2elm_t a, f2elm_t c)
{   // GF(p^2) squaring using Montgomery arithmetic, c = a^2 in GF(p^2).
    // Inputs: a = a0+a1*i, where a0, a1 are in [0, 2*p-1]
    // Output: c = c0+c1*i, where c0, c1 are in [0, 2*p-1]
    felm_t t1, t2, t3;

    mp_addfast(a[0], a[1], t1); // t1 = a0+a1
    fpsub(a[0], a[1], t2);      // t2 = a0-a1
    mp_addfast(a[0], a[0], t3); // t3 = 2a0
    fpmul_mont(t1, t2, c[0]);   // c0 = (a0+a1)(a0-a1)
    fpmul_mont(t3, a[1], c[1]); // c1 = 2a0*a1
}

__inline unsigned int mp_sub(const digit_t *a, const digit_t *b, digit_t *c, const unsigned int nwords)
{ // Multiprecision subtraction, c = a-b, where lng(a) = lng(b) = nwords. Returns the borrow bit.
    unsigned int i, borrow = 0;

    for (i = 0; i < nwords; i++)
    {
        SUBC(borrow, a[i], b[i], borrow, c[i]);
    }

    return borrow;
}

__inline static void mp_subaddfast(const digit_t* a, const digit_t* b, digit_t* c)
{ // Multiprecision subtraction followed by addition with p*2^MAXBITS_FIELD, c = a-b+(p*2^MAXBITS_FIELD) if a-b < 0, otherwise c=a-b. 
#if (OS_TARGET == OS_WIN) || defined(GENERIC_IMPLEMENTATION)
    felm_t t1;

    digit_t mask = 0 - (digit_t)mp_sub(a, b, c, 2*NWORDS_FIELD);
    for (int i = 0; i < NWORDS_FIELD; i++)
        t1[i] = ((digit_t*)PRIME)[i] & mask;
    mp_addfast((digit_t*)&c[NWORDS_FIELD], t1, (digit_t*)&c[NWORDS_FIELD]);

#elif (OS_TARGET == OS_LINUX)               

    mp_subaddx2_asm(a, b, c);     

#endif
}

__inline static void mp_dblsubfast(const digit_t *a, const digit_t *b, digit_t *c)
{   // Multiprecision subtraction, c = c-a-b, where lng(a) = lng(b) = 2*NWORDS_FIELD.
    // Inputs should be s.t. c > a and c > b
#if (OS_TARGET == OS_WIN) || defined(GENERIC_IMPLEMENTATION)

    mp_sub(c, a, c, 2 * NWORDS_FIELD);
    mp_sub(c, b, c, 2 * NWORDS_FIELD);

#elif (OS_TARGET == OS_LINUX)

    mp_dblsubx2_asm(a, b, c);

#endif
}

void fp2mul_mont(const f2elm_t a, const f2elm_t b, f2elm_t c)
{   // GF(p^2) multiplication using Montgomery arithmetic, c = a*b in GF(p^2).
    // Inputs: a = a0+a1*i and b = b0+b1*i, where a0, a1, b0, b1 are in [0, 2*p-1]
    // Output: c = c0+c1*i, where c0, c1 are in [0, 2*p-1]
    felm_t t1, t2;
    dfelm_t tt1, tt2, tt3;

    mp_addfast(a[0], a[1], t1);            // t1 = a0+a1
    mp_addfast(b[0], b[1], t2);            // t2 = b0+b1
    mp_mul(a[0], b[0], tt1, NWORDS_FIELD); // tt1 = a0*b0
    mp_mul(a[1], b[1], tt2, NWORDS_FIELD); // tt2 = a1*b1
    mp_mul(t1, t2, tt3, NWORDS_FIELD);     // tt3 = (a0+a1)*(b0+b1)
    mp_dblsubfast(tt1, tt2, tt3);          // tt3 = (a0+a1)*(b0+b1) - a0*b0 - a1*b1
    mp_subaddfast(tt1, tt2, tt1);          // tt1 = a0*b0 - a1*b1 + p*2^MAXBITS_FIELD if a0*b0 - a1*b1 < 0, else tt1 = a0*b0 - a1*b1
    rdc_mont(tt3, c[1]);                   // c[1] = (a0+a1)*(b0+b1) - a0*b0 - a1*b1 
    rdc_mont(tt1, c[0]);                   // c[0] = a0*b0 - a1*b1
}

static __inline void power2_setup(digit_t *x, int mark, const unsigned int nwords)
{ // Set up the value 2^mark.
    unsigned int i;

    for (i = 0; i < nwords; i++)
        x[i] = 0;

    i = 0;
    while (mark >= 0)
    {
        if (mark < RADIX)
        {
            x[i] = (digit_t)1 << mark;
        }
        mark -= RADIX;
        i += 1;
    }
}

static __inline unsigned int is_felm_zero(const felm_t x)
{   // Is x = 0? return 1 (TRUE) if condition is true, 0 (FALSE) otherwise.
    // SECURITY NOTE: This function does not run in constant-time.
    unsigned int i;

    for (i = 0; i < NWORDS_FIELD; i++)
    {
        if (x[i] != 0)
            return false;
    }
    return true;
}

static __inline unsigned int is_felm_even(const felm_t x)
{ // Is x even? return 1 (TRUE) if condition is true, 0 (FALSE) otherwise.
    return (unsigned int)((x[0] & 1) ^ 1);
}

static __inline unsigned int is_felm_lt(const felm_t x, const felm_t y)
{   // Is x < y? return 1 (TRUE) if condition is true, 0 (FALSE) otherwise.
    // SECURITY NOTE: This function does not run in constant-time.
    int i;

    for (i = NWORDS_FIELD - 1; i >= 0; i--)
    {
        if (x[i] < y[i])
        {
            return true;
        }
        else if (x[i] > y[i])
        {
            return false;
        }
    }
    return false;
}

static __inline void fpinv_mont_bingcd_partial(const felm_t a, felm_t x1, unsigned int *k)
{ // Partial Montgomery inversion via the binary GCD algorithm.
    felm_t u, v, x2;
    unsigned int cwords; // Number of words necessary for x1, x2

    fpcopy(a, u);
    fpcopy((digit_t *)PRIME, v);
    fpzero(x1);
    x1[0] = 1;
    fpzero(x2);
    *k = 0;

    while (!is_felm_zero(v))
    {
        cwords = ((*k + 1) / RADIX) + 1;
        if ((cwords < NWORDS_FIELD))
        {
            if (is_felm_even(v))
            {
                mp_shiftr1(v, NWORDS_FIELD);
                mp_shiftl1(x1, cwords);
            }
            else if (is_felm_even(u))
            {
                mp_shiftr1(u, NWORDS_FIELD);
                mp_shiftl1(x2, cwords);
            }
            else if (!is_felm_lt(v, u))
            {
                mp_sub(v, u, v, NWORDS_FIELD);
                mp_shiftr1(v, NWORDS_FIELD);
                mp_add(x1, x2, x2, cwords);
                mp_shiftl1(x1, cwords);
            }
            else
            {
                mp_sub(u, v, u, NWORDS_FIELD);
                mp_shiftr1(u, NWORDS_FIELD);
                mp_add(x1, x2, x1, cwords);
                mp_shiftl1(x2, cwords);
            }
        }
        else
        {
            if (is_felm_even(v))
            {
                mp_shiftr1(v, NWORDS_FIELD);
                mp_shiftl1(x1, NWORDS_FIELD);
            }
            else if (is_felm_even(u))
            {
                mp_shiftr1(u, NWORDS_FIELD);
                mp_shiftl1(x2, NWORDS_FIELD);
            }
            else if (!is_felm_lt(v, u))
            {
                mp_sub(v, u, v, NWORDS_FIELD);
                mp_shiftr1(v, NWORDS_FIELD);
                mp_add(x1, x2, x2, NWORDS_FIELD);
                mp_shiftl1(x1, NWORDS_FIELD);
            }
            else
            {
                mp_sub(u, v, u, NWORDS_FIELD);
                mp_shiftr1(u, NWORDS_FIELD);
                mp_add(x1, x2, x1, NWORDS_FIELD);
                mp_shiftl1(x2, NWORDS_FIELD);
            }
        }
        *k += 1;
    }

    if (is_felm_lt((digit_t *)PRIME, x1))
    {
        mp_sub(x1, (digit_t *)PRIME, x1, NWORDS_FIELD);
    }
}

void fpinv_mont(felm_t a)
{   // Field inversion via the binary GCD using Montgomery arithmetic, a = a^-1*R mod p.
    // SECURITY NOTE: This function does not run in constant-time and is therefore only suitable for
    //                operations not involving any secret data.
    felm_t x, t;
    unsigned int k;

    fpinv_mont_bingcd_partial(a, x, &k);
    if (k <= MAXBITS_FIELD)
    {
        fpmul_mont(x, (digit_t *)&Montgomery_R2, x);
        k += MAXBITS_FIELD;
    }
    fpmul_mont(x, (digit_t *)&Montgomery_R2, x);
    power2_setup(t, 2 * MAXBITS_FIELD - k, NWORDS_FIELD);
    fpmul_mont(x, t, a);
}

void fp2inv_mont(f2elm_t a)
{ // GF(p^2) inversion using Montgomery arithmetic, a = (a0-i*a1)/(a0^2+a1^2).
    f2elm_t t1;

    fpsqr_mont(a[0], t1[0]);    // t10 = a0^2
    fpsqr_mont(a[1], t1[1]);    // t11 = a1^2
    fpadd(t1[0], t1[1], t1[0]); // t10 = a0^2+a1^2
    fpinv_mont(t1[0]);          // t10 = (a0^2+a1^2)^-1
    fpneg(a[1]);                // a = a0-i*a1
    fpmul_mont(a[0], t1[0], a[0]);
    fpmul_mont(a[1], t1[0], a[1]); // a = (a0-i*a1)*(a0^2+a1^2)^-1
}

bool fp2_is_equal(const f2elm_t a, const f2elm_t b)
{   // Is a = b? return 1 (TRUE) if condition is true, 0 (FALSE) otherwise.
    // SECURITY NOTE: This function does not run in constant-time.
    f2elm_t t0;

    fp2sub(a, b, t0);
    fp2correction(t0);

    if (is_felm_zero(t0[0]) == false || is_felm_zero(t0[1]) == false)
        return false;

    return true;
}

void to_fp2mont(const f2elm_t a, f2elm_t mc)
{   // Conversion of a GF(p^2) element to Montgomery representation,
    // mc_i = a_i*R^2*R^(-1) = a_i*R in GF(p^2).

    to_mont(a[0], mc[0]);
    to_mont(a[1], mc[1]);
}

void from_fp2mont(const f2elm_t ma, f2elm_t c)
{   // Conversion of a GF(p^2) element from Montgomery representation to standard representation,
    // c_i = ma_i*R^(-1) = a_i in GF(p^2).

    from_mont(ma[0], c[0]);
    from_mont(ma[1], c[1]);
}

__inline unsigned int mp_add(const digit_t *a, const digit_t *b, digit_t *c, const unsigned int nwords)
{ // Multiprecision addition, c = a+b, where lng(a) = lng(b) = nwords. Returns the carry bit.
    unsigned int i, carry = 0;

    for (i = 0; i < nwords; i++)
    {
        ADDC(carry, a[i], b[i], carry, c[i]);
    }

    return carry;
}

void mp_shiftleft(digit_t *x, unsigned int shift, const unsigned int nwords)
{
    unsigned int i, j = 0;

    while (shift > RADIX)
    {
        j += 1;
        shift -= RADIX;
    }

    for (i = 0; i < nwords - j; i++)
        x[nwords - 1 - i] = x[nwords - 1 - i - j];
    for (i = nwords - j; i < nwords; i++)
        x[nwords - 1 - i] = 0;
    if (shift != 0)
    {
        for (j = nwords - 1; j > 0; j--)
            SHIFTL(x[j], x[j - 1], shift, x[j], RADIX);
        x[0] <<= shift;
    }
}

void mp_shiftr1(digit_t *x, const unsigned int nwords)
{ // Multiprecision right shift by one.
    unsigned int i;

    for (i = 0; i < nwords - 1; i++)
    {
        SHIFTR(x[i + 1], x[i], 1, x[i], RADIX);
    }
    x[nwords - 1] >>= 1;
}

void mp_shiftl1(digit_t *x, const unsigned int nwords)
{ // Multiprecision left shift by one.
    int i;

    for (i = nwords - 1; i > 0; i--)
    {
        SHIFTL(x[i], x[i - 1], 1, x[i], RADIX);
    }
    x[0] <<= 1;
}