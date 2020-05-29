#include <assert.h>
#include "sidh_vow_base.h"
#include "types/triples.h"
#include "types/state.h"
#include "curve_math.h"
#include "bintree.h"
#include "triples.h"

// Functions for swig interface
#include "swig_helpers.c"
#include "state.c"

/* Initializations */
static st_t init_st(uint64_t nwords_state)
{
    st_t s;
    s.words = calloc(nwords_state, sizeof(digit_t));
    return s;
}

static void free_st(st_t *s)
{
    free(s->words);
}

static void ConcatArray(unsigned long *cat, unsigned long *A, unsigned long lenA, unsigned long *B, unsigned long lenB)
{
    unsigned int i;

    for (i = 1; i < lenA + 1; i++)
        cat[i] = A[i - 1];
    for (i = 0; i < lenB; i++)
        cat[lenA + i + 1] = B[i];
}

static unsigned long OptStrat(unsigned long *strat, const unsigned long n, const double p, const double q)
{
    unsigned int i, j, b, ctr = 2;
    /* Maximum size of strategy = 250 */
    double C[250], newCpqs[250], newCpq, tmpCpq;
    unsigned long S[250][250];
    unsigned long lens[250];

    lens[1] = 0;
    S[1][0] = 0;
    C[1] = 0;
    for (i = 2; i < n + 1; i++) {
        for (b = 1; b < i; b++)
            newCpqs[b] = C[i - b] + C[b] + b * p + (i - b) * q;
        newCpq = newCpqs[1];
        b = 1;
        for (j = 2; j < i; j++) {
            tmpCpq = newCpqs[j];
            if (newCpq > tmpCpq) {
                newCpq = tmpCpq;
                b = j;
            }
        }
        S[ctr][0] = b;
        ConcatArray(S[ctr], S[i - b], lens[i - b], S[b], lens[b]);
        C[ctr] = newCpqs[b];
        lens[ctr] = 1 + lens[i - b] + lens[b];
        ctr++;
    }

    for (i = 0; i < lens[ctr - 1]; i++)
        strat[i] = S[ctr - 1][i];

    return lens[ctr - 1];
}

static void xDBL_affine(const point_proj_t P, point_proj_t Q, const f2elm_t a24)
{
    f2elm_t t0, t1;

    fp2sub(P->X, P->Z, t0);      // t0 = X1-Z1
    fp2add(P->X, P->Z, t1);      // t1 = X1+Z1
    fp2sqr_mont(t0, t0);         // t0 = (X1-Z1)^2
    fp2sqr_mont(t1, t1);         // t1 = (X1+Z1)^2
    fp2mul_mont(t0, t1, Q->X);   // X2 = C24*(X1-Z1)^2*(X1+Z1)^2
    fp2sub(t1, t0, t1);          // t1 = (X1+Z1)^2-(X1-Z1)^2
    fp2mul_mont(a24, t1, Q->Z);  // t0 = A24plus*[(X1+Z1)^2-(X1-Z1)^2]
    fp2add(Q->Z, t0, Q->Z);      // Z2 = A24plus*[(X1+Z1)^2-(X1-Z1)^2] + C24*(X1-Z1)^2
    fp2mul_mont(Q->Z, t1, Q->Z); // Z2 = [A24plus*[(X1+Z1)^2-(X1-Z1)^2] + C24*(X1-Z1)^2]*[(X1+Z1)^2-(X1-Z1)^2]
}

static void xDBLe_affine(const point_proj_t P, point_proj_t Q, const f2elm_t a24, const int e)
{ // Computes [2^e](X:Z) on Montgomery curve with projective constant via e repeated doublings.
  // Input: projective Montgomery x-coordinates P = (XP:ZP), such that xP=XP/ZP and Montgomery curve constants A+2C and 4C.
  // Output: projective Montgomery x-coordinates Q <- (2^e)*P.

    copy_words((digit_t *)P, (digit_t *)Q, 2 * 2 * NWORDS_FIELD);

    for (int i = 0; i < e; i++)
        xDBL_affine(Q, Q, a24);
}

static void GetFourIsogenyWithKernelXeqZ(point_proj_t A24, const f2elm_t a24)
{
    fp2copy(a24, A24->X);
    fp2copy(a24, A24->Z);
    fpsub((digit_t *)A24->Z, (digit_t *)&Montgomery_one, (digit_t *)A24->Z);
}

static void EvalFourIsogenyWithKernelXeqZ(point_proj_t S, const f2elm_t a24)
{
    f2elm_t T0, T1, T2;

    fpzero(T1[1]); // Set RZ = 1
    fpcopy((digit_t *)&Montgomery_one, T1[0]);
    fp2add(S->X, S->Z, T0);
    fp2sub(S->X, S->Z, T2);
    fp2sqr_mont(T0, T0);
    fp2sqr_mont(T2, T2);
    fp2sub(T1, a24, T1);
    fp2add(T1, T1, T1);
    fp2add(T1, T1, T1);
    fp2mul_mont(T1, S->X, T1);
    fp2mul_mont(T1, S->Z, T1);
    fp2mul_mont(T1, T2, S->Z);
    fp2sub(T0, T1, T1);
    fp2mul_mont(T0, T1, S->X);
}

static void GetFourIsogenyWithKernelXeqMinZ(point_proj_t A24, const f2elm_t a24)
{
    fp2copy(a24, A24->X);
    fp2copy(a24, A24->Z);
    fpsub((digit_t *)A24->X, (digit_t *)&Montgomery_one, (digit_t *)A24->X);
}

static void EvalFourIsogenyWithKernelXeqMinZ(point_proj_t S, const f2elm_t a24)
{
    f2elm_t T0, T1, T2;

    fp2add(S->X, S->Z, T2);
    fp2sub(S->X, S->Z, T0);
    fp2sqr_mont(T2, T2);
    fp2sqr_mont(T0, T0);
    fp2add(a24, a24, T1);
    fp2add(T1, T1, T1);
    fp2mul_mont(T1, S->X, T1);
    fp2mul_mont(T1, S->Z, T1);
    fp2mul_mont(T1, T2, S->Z);
    fp2neg(S->Z);
    fp2add(T0, T1, T1);
    fp2mul_mont(T0, T1, S->X);
}

static void FourwayInv(f2elm_t X, f2elm_t Y, f2elm_t Z, f2elm_t T)
{
    f2elm_t Xt, Zt, XY, ZT, XYZT;

    fp2copy(X, Xt);
    fp2copy(Z, Zt);
    fp2mul_mont(X, Y, XY);
    fp2mul_mont(Z, T, ZT);
    fp2mul_mont(XY, ZT, XYZT);
    fp2inv_mont(XYZT);         /* 1 / XYZT */
    fp2mul_mont(ZT, XYZT, ZT); /* 1 / XY */
    fp2mul_mont(XY, XYZT, XY); /* 1 / ZT */
    fp2mul_mont(ZT, Y, X);
    fp2mul_mont(ZT, Xt, Y);
    fp2mul_mont(XY, T, Z);
    fp2mul_mont(XY, Zt, T);
}

/* Simple functions on states */
static unsigned char GetC_SIDH(const st_t *s)
{
    return (s->bytes[0] & 0x01);
}

static unsigned char GetB_SIDH(const st_t *s)
{
    return ((s->bytes[0] & 0x06) >> 1);
}

static void SetB_SIDH(st_t *s, const unsigned char t)
{
    /* Assumes that b is set to 0x03 */
    s->bytes[0] &= ((t << 1) | 0xF9);
}

static void copy_st(st_t *r, const st_t *s, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
        r->words[i] = s->words[i];
}

static void copy_st2uint64(uint64_t *r, const st_t *s, const uint64_t nwords_state)
{

    for (unsigned int i = 0; i < nwords_state; i++)
        r[i] = s->words[i];
}

static void copy_uint642st(st_t *r, const uint64_t *s, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
        r->words[i] = s[i];
}

static void SwapStSIDH(st_t *r, st_t *s, uint64_t nwords_state)
{
    st_t t = init_st(nwords_state);

    copy_st(&t, r, nwords_state);
    copy_st(r, s, nwords_state);
    copy_st(s, &t, nwords_state);
    free_st(&t);
}

bool is_equal_st(const st_t *s, const st_t *t, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
    {
        if (s->words[i] != t->words[i])
            return false;
    }
    return true;
}

static bool is_equal_st_words(const st_t *s, const uint64_t *r, const uint64_t nwords_state)
{
    for (unsigned int i = 0; i < nwords_state; i++)
    {
        if (s->words[i] != r[i])
            return false;
    }
    return true;
}

bool IsEqualJinvSIDH(unsigned char j0[FP2_ENCODED_BYTES], unsigned char j1[FP2_ENCODED_BYTES])
{
    for (unsigned int i = 0; i < FP2_ENCODED_BYTES; i++)
    {
        if (j0[i] != j1[i])
            return false;
    }
    return true;
}

void copy_trip(trip_t *s, const trip_t *t, const uint64_t nwords_state)
{
    copy_st(&s->current_state, &t->current_state, nwords_state);
    s->current_steps = t->current_steps;
    copy_st(&s->initial_state, &t->initial_state, nwords_state);
}

/* Functions for vOW */
bool DistinguishedSIDH(private_state_t *private_state)
{
    /* Divide distinguishedness over interval to avoid bad cases */
    assert(private_state->MEMORY_LOG_SIZE > EXTRA_MEM_LOG_SIZE);
    uint64_t val;

    val = private_state->current.current_state.words[0] >> (private_state->MEMORY_LOG_SIZE + EXTRA_MEM_LOG_SIZE);
    val += (uint64_t)private_state->function_version * (uint64_t)private_state->DIST_BOUND;
    val &= (((uint64_t)1 << (private_state->NBITS_STATE - EXTRA_MEM_LOG_SIZE - private_state->MEMORY_LOG_SIZE)) - 1);

    return (val <= (uint64_t)private_state->DIST_BOUND);
}

uint64_t MemIndexSIDH(private_state_t *private_state)
{
    assert(private_state->MEMORY_SIZE <= (pow(2, RADIX - 3) - 1)); /* Assumes that MEMORY_SIZE <= 2^RADIX */
    return (uint64_t)(((private_state->current.current_state.words[0] >> EXTRA_MEM_LOG_SIZE) + private_state->random_functions) & private_state->MEMORY_SIZE_MASK);
}

static unsigned int GetMSBSIDH(const unsigned char *m, unsigned long nbits_state)
{
    int msb = nbits_state;
    int bit = (m[(msb - 1) >> 3] >> ((msb - 1) & 0x07)) & 1;

    while ((bit == 0) && (msb > 0))
    {
        msb--;
        bit = (m[(msb - 1) >> 3] >> ((msb - 1) & 0x07)) & 1;
    }

    return msb;
}

void UpdateSIDH(private_state_t *private_state)
{
    uint64_t i, temp;
    unsigned char j[FP2_ENCODED_BYTES];

    UpdateStSIDH(j, &private_state->current.current_state, &private_state->current.current_state, private_state);
    private_state->number_steps_collect += 1;

    if (private_state->HANSEL_GRETEL) {
        if (private_state->crumbs.num_crumbs < private_state->crumbs.max_crumbs) {
            copy_st2uint64(&private_state->crumbs.crumbs[private_state->crumbs.position], &private_state->current.current_state, private_state->NWORDS_STATE);
            private_state->crumbs.positions[private_state->crumbs.position] = private_state->crumbs.position;
            private_state->crumbs.index_crumbs[private_state->crumbs.position] = private_state->crumbs.position;
            private_state->crumbs.num_crumbs++;
        } else if (private_state->crumbs.position - private_state->crumbs.positions[private_state->crumbs.max_crumbs - 1] == private_state->crumbs.max_dist) {
            temp = private_state->crumbs.index_crumbs[private_state->crumbs.index_position];
            for (i = private_state->crumbs.index_position; i < private_state->crumbs.max_crumbs - 1; i++) { 
                // Updating table with crumb indices for the crump table
                private_state->crumbs.index_crumbs[i] = private_state->crumbs.index_crumbs[i + 1];
            }
            private_state->crumbs.index_crumbs[private_state->crumbs.max_crumbs - 1] = temp;
            private_state->crumbs.index_position++;
            if (private_state->crumbs.index_position > private_state->crumbs.max_crumbs - 1)
                private_state->crumbs.index_position = 0;
            copy_st2uint64(&private_state->crumbs.crumbs[temp], &private_state->current.current_state, private_state->NWORDS_STATE); // Inserting a new crumb at the end of the crumb table

            for (i = private_state->crumbs.scratch_position; i < private_state->crumbs.max_crumbs - 1; i++) { 
                // Updating table with crumb positions
                private_state->crumbs.positions[i] = private_state->crumbs.positions[i + 1];
            }
            private_state->crumbs.positions[private_state->crumbs.max_crumbs - 1] = private_state->crumbs.position;
            private_state->crumbs.swap_position += 2 * private_state->crumbs.real_dist;
            private_state->crumbs.scratch_position++;
            if (private_state->crumbs.swap_position > private_state->crumbs.max_crumbs - 1) { 
                // Kind of cumbersome, maybe this can be simplified (but not time critical)
                private_state->crumbs.swap_position = 0;
                private_state->crumbs.real_dist <<= 1;
            }
            if (private_state->crumbs.scratch_position > private_state->crumbs.max_crumbs - 1) {
                private_state->crumbs.scratch_position = 0;
                private_state->crumbs.max_dist <<= 1;
                private_state->crumbs.swap_position = private_state->crumbs.max_dist;
            }
        }
        private_state->crumbs.position++;
    }
}

void UpdateRandomFunctionSIDH(shared_state_t *S, private_state_t *private_state)
{
    private_state->function_version++;
    // reset "resync done" flag
    if (private_state->thread_id == 0) {
        S->resync_cores[0] = 0;
    }
}

static inline bool BacktrackSIDH_core(trip_t *c0, trip_t *c1, shared_state_t *S, private_state_t *private_state)
{
    unsigned long L, i;
    st_t c0_, c1_;
    unsigned char jinv0[FP2_ENCODED_BYTES], jinv1[FP2_ENCODED_BYTES];
    f2elm_t jinv;
    (void)private_state;

    // Make c0 have the largest number of steps
    if (c0->current_steps < c1->current_steps) {
        SwapStSIDH(&c0->initial_state, &c1->initial_state, private_state->NWORDS_STATE);
        L = (unsigned long)(c1->current_steps - c0->current_steps);
    } else {
        L = (unsigned long)(c0->current_steps - c1->current_steps);
    }

    // Catch up the trails
    for (i = 0; i < L; i++) {
        UpdateStSIDH(jinv0, &c0->initial_state, &c0->initial_state, private_state);
        private_state->number_steps_locate += 1;
    }

    if (is_equal_st(&c0->initial_state, &c1->initial_state, private_state->NWORDS_STATE))
        return false; // Robin Hood

    c0_ = init_st(private_state->NWORDS_STATE);
    c1_ = init_st(private_state->NWORDS_STATE);

    for (i = 0; i < c1->current_steps + 1; i++) {
        UpdateStSIDH(jinv0, &c0_, &c0->initial_state, private_state);
        private_state->number_steps_locate += 1;
        UpdateStSIDH(jinv1, &c1_, &c1->initial_state, private_state);
        private_state->number_steps_locate += 1;

        if (IsEqualJinvSIDH(jinv0, jinv1)) {
            /* Record collision */
            private_state->collisions += 1;
            if (private_state->collect_vow_stats) {
#pragma omp critical
                {
                    insertTree(&S->dist_cols, c0->initial_state, c1->initial_state, private_state->NWORDS_STATE);
                }
            }

            // free tmp states
            free_st(&c0_);
            free_st(&c1_);

            if (GetC_SIDH(&c0->initial_state) == GetC_SIDH(&c1->initial_state)) {
                return false;
            } else {
                fp2_decode(jinv0, jinv);
                assert(fp2_is_equal(jinv, private_state->jinv)); /* Verify that we found the right one*/
                return true;
            }
        } else {
            copy_st(&c0->initial_state, &c0_, private_state->NWORDS_STATE);
            copy_st(&c1->initial_state, &c1_, private_state->NWORDS_STATE);
        }
    }
    /* Should never reach here */
    return false;
}

static inline bool BacktrackSIDH_Hansel_Gretel(trip_t *c_mem, trip_t *c_crumbs, shared_state_t *S, private_state_t *private_state)
{
    uint64_t L;
    trip_t c0_, cmem;
    uint64_t i, k, index;
    uint64_t crumb;
    bool resp, equal;
    unsigned char j[FP2_ENCODED_BYTES];

    cmem = init_trip(private_state->NWORDS_STATE);
    copy_trip(&cmem, c_mem, private_state->NWORDS_STATE);

    // Make the memory trail (without crumbs) at most the length of the crumbs trail.
    if (cmem.current_steps > c_crumbs->current_steps) {
        L = cmem.current_steps - c_crumbs->current_steps;
        for (i = 0; i < L; i++) {
            UpdateStSIDH(j, &cmem.initial_state, &cmem.initial_state, private_state);
            private_state->number_steps_locate += 1;
        }
        cmem.current_steps = c_crumbs->current_steps;
    }
    // Check for Robin Hood
    if (is_equal_st(&cmem.initial_state, &c_crumbs->initial_state, private_state->NWORDS_STATE))
        return false;

    // The memory path is L steps shorter than the crumbs path.
    L = c_crumbs->current_steps - cmem.current_steps;
    k = 0;
    // Since there has been at least one step, there is at least one crumb.
    // Crumbs only store intermediate points, not the initial state and not
    // necessarily the current state.
    index = private_state->crumbs.positions[0] + 1;

    while ((L > index) && (k + 1 < private_state->crumbs.num_crumbs)) {
        // There are still crumbs to check and we haven't found the next crumb to reach.
        k++;
        index = private_state->crumbs.positions[k] + 1;
    }
    // Either have found the next crumb or ran out of crumbs to check.
    if (L > index) {
        // Ran out of crumbs to check, i.e. already in the interval beyond the last crumb.
        // Trails collide after last crumb.
        // Call original BacktrackGen on memory trail and shortened crumbs trail.
        copy_uint642st(&c_crumbs->initial_state, &private_state->crumbs.crumbs[private_state->crumbs.index_crumbs[k]], private_state->NWORDS_STATE);
        c_crumbs->current_steps -= (private_state->crumbs.positions[k] + 1);
        resp = BacktrackSIDH_core(&cmem, c_crumbs, S, private_state);
    } else {
        // Next crumb to check lies before (or is) the last crumb.
        c0_ = init_trip(private_state->NWORDS_STATE);
        copy_trip(&c0_, &cmem, private_state->NWORDS_STATE);

        do
        {
            cmem.current_steps = c0_.current_steps;
            copy_st(&cmem.initial_state, &c0_.initial_state, private_state->NWORDS_STATE);
            crumb = private_state->crumbs.crumbs[private_state->crumbs.index_crumbs[k]];
            index = private_state->crumbs.positions[k] + 1;

            L = cmem.current_steps - (c_crumbs->current_steps - index);
            for (i = 0; i < L; i++) {
                UpdateStSIDH(j, &c0_.initial_state, &c0_.initial_state, private_state);
                private_state->number_steps_locate += 1;
            }
            c0_.current_steps -= L;
            k++;
            equal = is_equal_st_words(&c0_.initial_state, &crumb, private_state->NWORDS_STATE);
        } while (!equal && k < private_state->crumbs.num_crumbs);
        // Either found the colliding crumb or moved to the interval beyond the last crumb.

        if (equal) { // Have a colliding crumb
            copy_uint642st(&cmem.current_state, &crumb, private_state->NWORDS_STATE);
            cmem.current_steps -= c0_.current_steps;
            if (k == 1) {
                c0_.current_steps = private_state->crumbs.positions[0] + 1;
                copy_uint642st(&c0_.initial_state, c_crumbs->initial_state.words, private_state->NWORDS_STATE);
            } else {
                c0_.current_steps = private_state->crumbs.positions[k - 1] - private_state->crumbs.positions[k - 2];
                copy_uint642st(&c0_.initial_state, &private_state->crumbs.crumbs[private_state->crumbs.index_crumbs[k - 2]], private_state->NWORDS_STATE);
            }
            copy_uint642st(&c0_.current_state, &crumb, private_state->NWORDS_STATE);
        } else { // Collision happens after the last crumb.
            cmem.current_steps = c0_.current_steps;
            copy_uint642st(&cmem.initial_state, &crumb, private_state->NWORDS_STATE);
        }
        resp = BacktrackSIDH_core(&cmem, &c0_, S, private_state);
        free_trip(&c0_);
    }
    free_trip(&cmem);
    return resp;
}

bool BacktrackSIDH(trip_t *c0, trip_t *c1, shared_state_t *S, private_state_t *private_state)
{ // Backtrack function selection

    if (private_state->HANSEL_GRETEL)
        return BacktrackSIDH_Hansel_Gretel(c0, c1, S, private_state);
    else
        return BacktrackSIDH_core(c0, c1, S, private_state);
}
