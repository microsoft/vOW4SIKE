#define VOW_SIKE

#include <stdio.h>
#include <time.h>
#include "sidh_vow_base.h"
#include "memory.c"
#include "bintree.c"
#include "sidh_vow_base.c"
#include "curve_math.h"
#include "vow.c"


void LadderThreePtSIKE(point_proj_t R, const f2elm_t a24, const f2elm_t xp, const f2elm_t xq, const f2elm_t xpq, const unsigned char *m, unsigned long nbits_state)
{ // Non-constant time version that depends on size of k
    point_proj_t R0 = {0}, R2 = {0};
    unsigned int i, bit;

    fp2copy(xp, R->X);
    fp2copy(xq, R0->X);
    fp2copy(xpq, R2->X);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R0->Z);
    fpzero((digit_t *)(R0->Z)[1]);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R->Z);
    fpzero((digit_t *)(R->Z)[1]);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R2->Z);
    fpzero((digit_t *)(R2->Z)[1]);

    for (i = 0; i < nbits_state; i++) { // Ignore 3 lsb's c,b
        bit = (m[i >> 3] >> (i & 0x07)) & 1;

        if (bit) {
            xDBLADD_SIDH(R0, R, R2->X, a24);
            fp2mul_mont(R->X, R2->Z, R->X);
        } else {
            xDBLADD_SIDH(R0, R2, R->X, a24);
            fp2mul_mont(R2->X, R->Z, R2->X);
        }
    }
}

static void IsogenyWithPoints(point_proj_t A24, point_proj_t XP, point_proj_t XQ, point_proj_t XPQ,
                              const f2elm_t a24, const point_proj_t kernel_point, const point_proj_t XQinp,
                              const unsigned long *strat, const unsigned long lenstrat)
{
    point_proj_t R, pts[MAX_INT_POINTS_ALICE];
    f2elm_t coeff[3];
    unsigned long i, row, index = 0, ii = 0, m, npts = 0, pts_index[MAX_INT_POINTS_ALICE];

    fp2copy(kernel_point->X, R->X);
    fp2copy(kernel_point->Z, R->Z);

    fp2copy(XQinp->X, XQ->X);
    fp2copy(XQinp->Z, XQ->Z);

    fp2copy(a24, A24->X);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)A24->Z);
    fpzero((digit_t *)(A24->Z)[1]);

    /* All steps except the first */
    for (row = 1; row < lenstrat + 1; row++) {
        while (index < lenstrat + 1 - row)
        {
            fp2copy(R->X, pts[npts]->X);
            fp2copy(R->Z, pts[npts]->Z);
            pts_index[npts++] = index;
            m = strat[ii++];
            xDBLe(R, R, A24->X, A24->Z, (int)(2 * m));
            index += m;
        }
        GetFourIsogenyWithKernelXneZ(R, A24->X, A24->Z, coeff);
        EvalFourIsogenyWithKernelXneZ(XP, coeff);
        EvalFourIsogenyWithKernelXneZ(XQ, coeff);
        EvalFourIsogenyWithKernelXneZ(XPQ, coeff);

        for (i = 0; i < npts; i++) {
            EvalFourIsogenyWithKernelXneZ(pts[i], coeff);
        }

        fp2copy(pts[npts - 1]->X, R->X);
        fp2copy(pts[npts - 1]->Z, R->Z);
        index = pts_index[npts - 1];
        npts -= 1;
    }

    GetFourIsogenyWithKernelXneZ(R, A24->X, A24->Z, coeff);
    EvalFourIsogenyWithKernelXneZ(XP, coeff);
    EvalFourIsogenyWithKernelXneZ(XQ, coeff);
    EvalFourIsogenyWithKernelXneZ(XPQ, coeff);
}

void PrecompRightCurve(CurveAndPointsSIDH *E1, CurveAndPointsSIDH *RightE, unsigned long delta, unsigned long e)
{
    /* First 1 step with kernel x = 1 */
    unsigned long h, j, index_r, index_w;
    unsigned char k[4] = {4, 0, 0, 0}, l[4] = {0, 0, 0, 0};
    point_proj_t PpQ, A24, XP, XQ, XPQ, XR;
    point_proj_t A242, XP2, XQ2, XQ2_, XPQ2;

    fp2copy(RightE->xp, XP->X);
    fpcopy((digit_t *)&Montgomery_one, XP->Z[0]);
    fpzero(XP->Z[1]);

    fp2copy(RightE->xq, PpQ->X);
    fpcopy((digit_t *)&Montgomery_one, PpQ->Z[0]);
    fpzero(PpQ->Z[1]);

    xDBLADD(XP, PpQ, RightE->xpq, RightE->a24); // 2P, P+Q
    fp2inv_mont(PpQ->Z);
    fp2mul_mont(PpQ->X, PpQ->Z, PpQ->X); // P+Q
    xDBL_affine(XP, XP, RightE->a24);    // 4P

    LadderThreePtSIKE(XPQ, RightE->a24, RightE->xq, RightE->xp, PpQ->X, k, 3);

    fp2copy(RightE->xq, XQ->X);
    fpcopy((digit_t *)&Montgomery_one, XQ->Z[0]);
    fpzero(XQ->Z[1]);

    GetFourIsogenyWithKernelXeqZ(A24, RightE->a24);
    EvalFourIsogenyWithKernelXeqZ(XP, RightE->a24);
    EvalFourIsogenyWithKernelXeqZ(XQ, RightE->a24);
    EvalFourIsogenyWithKernelXeqZ(XPQ, RightE->a24);

    FourwayInv(A24->Z, XP->Z, XQ->Z, XPQ->Z);
    fp2mul_mont(A24->X, A24->Z, E1[0].a24);
    fp2mul_mont(XP->X, XP->Z, E1[0].xq);
    fp2mul_mont(XQ->X, XQ->Z, E1[0].xp); /* Swapping the values of P and Q */
    fp2mul_mont(XPQ->X, XPQ->Z, E1[0].xpq);

    /* Now delta sized pre-computation */

    if (delta != 0) {
        for (h = 0; h < delta / 2; h++) {
            for (index_r = 0; index_r < (unsigned long)(1 << 2 * h); index_r++) {
                // Get curve and points P, Q, P-Q at index_r
                fp2copy(E1[index_r].a24, A24->X);
                fpcopy((digit_t *)&Montgomery_one, A24->Z[0]);
                fpzero(A24->Z[1]);
                fp2copy(E1[index_r].xp, XP->X);
                fpcopy((digit_t *)&Montgomery_one, XP->Z[0]); // P
                fpzero(XP->Z[1]);
                fp2copy(E1[index_r].xq, XQ->X);
                fpcopy((digit_t *)&Montgomery_one, XQ->Z[0]);
                fpzero(XQ->Z[1]);
                fp2copy(E1[index_r].xpq, XPQ->X);
                fpcopy((digit_t *)&Montgomery_one, XPQ->Z[0]); // P-Q
                fpzero(XPQ->Z[1]);

                fp2copy(XQ->X, XQ2->X);
                fpcopy((digit_t *)&Montgomery_one, XQ2->Z[0]);
                fpzero(XQ2->Z[1]);

                fp2copy(XP->X, PpQ->X);
                fpcopy((digit_t *)&Montgomery_one, PpQ->Z[0]);
                fpzero(PpQ->Z[1]);

                // Sum PpQ = x(P + Q)
                xDBLADD(XQ2, PpQ, XPQ->X, A24->X);
                fp2inv_mont(PpQ->Z);
                fp2mul_mont(PpQ->X, PpQ->Z, PpQ->X);

                xDBL_affine(XQ2, XQ2, A24->X);

                for (j = 0; j < 4; j++) {
                    k[0] = (unsigned char)j;
                    l[0] = (unsigned char)(4 - j);

                    LadderThreePtSIKE(XP2, A24->X, XP->X, XQ->X, XPQ->X, k, 2);
                    LadderThreePtSIKE(XPQ2, A24->X, XP->X, XQ->X, PpQ->X, l, 3);
                    xDBLe_affine(XP2, XR, A24->X, e - 2 - 2 - 2 * h);

                    IsogenyWithPoints(A242, XP2, XQ2_, XPQ2, A24->X, XR, XQ2, NULL, 0);

                    index_w = index_r + j * (1 << (2 * h));
                    // Write curve and points at index_w
                    if (2 * h + 2 != e - 2) {
                        FourwayInv(A242->Z, XP2->Z, XQ2_->Z, XPQ2->Z);
                        fp2mul_mont(A242->X, A242->Z, E1[index_w].a24);
                        fp2mul_mont(XP2->X, XP2->Z, E1[index_w].xp);
                        fp2mul_mont(XQ2_->X, XQ2_->Z, E1[index_w].xq);
                        fp2mul_mont(XPQ2->X, XPQ2->Z, E1[index_w].xpq);
                    } else { /* Just store j-invariants to make it faster */
                        fp2add(A242->X, A242->X, A242->X);
                        fp2sub(A242->X, A242->Z, A242->X);
                        fp2add(A242->X, A242->X, A242->X);
                        j_inv(A242->X, A242->Z, E1[index_w].a24);

                        /* Frobenius */
                        fp2correction(E1[index_w].a24);
                        if ((E1[index_w].a24)[1][0] & 1)
                            fpneg((E1[index_w].a24)[1]);
                    }
                }
            }
        }
    }
}

static void GetTwoIsogenyWithXneZ(const point_proj_t R, point_proj_t A24)
{
    fp2sqr_mont(R->X, A24->X);
    fp2sqr_mont(R->Z, A24->Z);
    fp2sub(A24->Z, A24->X, A24->X);
}

static void EvalTwoIsogenyWithXneZ(const point_proj_t R, point_proj_t P)
{
    f2elm_t T0, T1, T2;

    fp2add(P->X, P->Z, T0);
    fp2sub(R->Z, R->X, T1);
    fp2mul_mont(T0, T1, T0);
    fp2sub(P->X, P->Z, T1);
    fp2add(R->X, R->Z, T2);
    fp2mul_mont(T1, T2, T1);
    fp2sub(T1, T0, T2);
    fp2add(T1, T0, T1);
    fp2mul_mont(P->X, T2, P->X);
    fp2mul_mont(P->Z, T1, P->Z);
}

void PrecompLeftCurve(CurveAndPointsSIDH *E0, CurveAndPointsSIDH *LeftE, unsigned long delta, unsigned long e)
{
    unsigned long h, j, index_r, index_w;
    unsigned char k[4], l[4]; /* Some max length.... should not be more */
    point_proj_t PpQ, A24, XP, XQ, XPQ, XR;
    point_proj_t A242, XP2, XQ2, XQ2_, XPQ2;

    /* First step bit = 0 */
    fp2copy(LeftE->xp, XP->X);
    fp2copy(LeftE->xq, XQ->X);
    fp2copy(LeftE->xpq, XPQ->X);

    fpcopy((digit_t *)&Montgomery_one, XP->Z[0]);
    fpzero(XP->Z[1]);
    fpcopy((digit_t *)&Montgomery_one, XQ->Z[0]);
    fpzero(XQ->Z[1]);
    fpcopy((digit_t *)&Montgomery_one, XPQ->Z[0]);
    fpzero(XPQ->Z[1]);

    xDBLADD(XQ, XPQ, LeftE->xp, LeftE->a24);
    xDBLe_affine(XP, XR, LeftE->a24, e - 2);

    GetTwoIsogenyWithXneZ(XR, A24);
    EvalTwoIsogenyWithXneZ(XR, XP);
    EvalTwoIsogenyWithXneZ(XR, XQ);
    EvalTwoIsogenyWithXneZ(XR, XPQ);

    FourwayInv(A24->Z, XP->Z, XQ->Z, XPQ->Z);
    fp2mul_mont(A24->X, A24->Z, E0[0].a24);
    fp2mul_mont(XP->X, XP->Z, E0[0].xp);
    fp2mul_mont(XQ->X, XQ->Z, E0[0].xq);
    fp2mul_mont(XPQ->X, XPQ->Z, E0[0].xpq);

    /* First step bit = 1 */
    /* Notice almost the same as above.. could share code */
    fp2copy(LeftE->xp, XP->X);
    fp2copy(LeftE->xq, XQ->X);
    fp2copy(LeftE->xpq, XPQ->X);

    fpcopy((digit_t *)&Montgomery_one, XP->Z[0]);
    fpzero(XP->Z[1]);
    fpcopy((digit_t *)&Montgomery_one, XQ->Z[0]);
    fpzero(XQ->Z[1]);
    fpcopy((digit_t *)&Montgomery_one, XPQ->Z[0]);
    fpzero(XPQ->Z[1]);

    xDBLADD(XQ, XP, LeftE->xpq, LeftE->a24);
    xDBLe_affine(XP, XR, LeftE->a24, e - 2);

    GetTwoIsogenyWithXneZ(XR, A24);
    EvalTwoIsogenyWithXneZ(XR, XP);
    EvalTwoIsogenyWithXneZ(XR, XQ);
    EvalTwoIsogenyWithXneZ(XR, XPQ);

    FourwayInv(A24->Z, XP->Z, XQ->Z, XPQ->Z);
    fp2mul_mont(A24->X, A24->Z, E0[1].a24);
    fp2mul_mont(XP->X, XP->Z, E0[1].xp);
    fp2mul_mont(XQ->X, XQ->Z, E0[1].xq);
    fp2mul_mont(XPQ->X, XPQ->Z, E0[1].xpq);

    if (delta != 0) {
        for (h = 0; h < delta / 2; h++) {
            for (index_r = 0; index_r < (unsigned long)(2 * (1 << 2 * h)); index_r++) {
                // Get curve and points P, Q, P-Q at index_r
                fp2copy(E0[index_r].a24, A24->X);
                fpcopy((digit_t *)&Montgomery_one, A24->Z[0]);
                fpzero(A24->Z[1]);
                fp2copy(E0[index_r].xp, XP->X);
                fpcopy((digit_t *)&Montgomery_one, XP->Z[0]); // P
                fpzero(XP->Z[1]);
                fp2copy(E0[index_r].xq, XQ->X);
                fpcopy((digit_t *)&Montgomery_one, XQ->Z[0]);
                fpzero(XQ->Z[1]);
                fp2copy(E0[index_r].xpq, XPQ->X);
                fpcopy((digit_t *)&Montgomery_one, XPQ->Z[0]); // P-Q
                fpzero(XPQ->Z[1]);

                fp2copy(XQ->X, XQ2->X);
                fpcopy((digit_t *)&Montgomery_one, XQ2->Z[0]);
                fpzero(XQ2->Z[1]);

                fp2copy(XP->X, PpQ->X);
                fpcopy((digit_t *)&Montgomery_one, PpQ->Z[0]);
                fpzero(PpQ->Z[1]);

                // Sum PpQ = x(P + Q)
                xDBLADD(XQ2, PpQ, XPQ->X, A24->X);
                fp2inv_mont(PpQ->Z);
                fp2mul_mont(PpQ->X, PpQ->Z, PpQ->X);

                xDBL_affine(XQ2, XQ2, A24->X);

                for (j = 0; j < 4; j++) {
                    k[0] = (unsigned char)j;
                    l[0] = (unsigned char)(4 - j);

                    LadderThreePtSIKE(XP2, A24->X, XP->X, XQ->X, XPQ->X, k, 2);
                    LadderThreePtSIKE(XPQ2, A24->X, XP->X, XQ->X, PpQ->X, l, 3);
                    xDBLe_affine(XP2, XR, A24->X, e - 2 - 2 - 2 * h);

                    IsogenyWithPoints(A242, XP2, XQ2_, XPQ2, A24->X, XR, XQ2, NULL, 0);

                    index_w = index_r + j * 2 * (1 << (2 * h));
                    // write curve and points at index_w
                    if (2 * h + 2 != e - 2) {
                        FourwayInv(A242->Z, XP2->Z, XQ2_->Z, XPQ2->Z);
                        fp2mul_mont(A242->X, A242->Z, E0[index_w].a24);
                        fp2mul_mont(XP2->X, XP2->Z, E0[index_w].xp);
                        fp2mul_mont(XQ2_->X, XQ2_->Z, E0[index_w].xq);
                        fp2mul_mont(XPQ2->X, XPQ2->Z, E0[index_w].xpq);
                    } else { /* Just store j-invariants to make it faster */
                        fp2add(A242->X, A242->X, A242->X);
                        fp2sub(A242->X, A242->Z, A242->X);
                        fp2add(A242->X, A242->X, A242->X);
                        j_inv(A242->X, A242->Z, E0[index_w].a24);

                        /* Frobenius */
                        fp2correction(E0[index_w].a24);
                        if ((E0[index_w].a24)[1][0] & 1)
                            fpneg((E0[index_w].a24)[1]);
                    }
                }
            }
        }
    }
}

void init_shared_state(instance_t *inst, shared_state_t *S
#ifdef STORE_IN_DATABASE
                       ,
                       db_settings_t *db_settings
#endif
)
{
    unsigned int i;
    unsigned long strat[250];

    /* Initialize state */
    S->instance = inst;
    S->NBITS_STATE = inst->e - 1;                                 /* Walk of size e from 2 sides*/
    S->NBYTES_STATE = ((S->NBITS_STATE + 7) / 8);                 /* Number of bytes needed for state */
    S->NWORDS_STATE = ((S->NBITS_STATE + RADIX64 - 1) / RADIX64); /* Number of words need for state, assumes 64-bit arch */
    S->NBITS_OVERFLOW = (S->NBITS_STATE % 8) == 0 ? 8 : (S->NBITS_STATE % 8);
    S->PRNG_SEED = (unsigned long)inst->PRNG_SEED;

    /* Initialize memory */
    S->MEMORY_LOG_SIZE = inst->MEMORY_LOG_SIZE;
    S->MEMORY_SIZE = (uint64_t)(1 << S->MEMORY_LOG_SIZE);
    S->MEMORY_SIZE_MASK = S->MEMORY_SIZE - 1;
    if (initialize_shared_memory(S) == EXIT_FAILURE) {
        printf("Error initialising shared memory\n");
        assert(0);
    }

    /* Initialize omp params */
    S->N_OF_CORES = inst->N_OF_CORES;

    /* Initialize pre-computed tables */
    S->delta = inst->delta;

    printf("Precomputing...\n");
    S->E[0] = calloc(((size_t)1 << (S->delta + 1)) * 4 * 2 * (size_t)NWORDS_FIELD, sizeof(digit_t));
    PrecompLeftCurve(S->E[0], &inst->E[0], S->delta, inst->e);
    S->external_E[0] = false;

    S->E[1] = calloc(((size_t)1 << S->delta) * 4 * 2 * (size_t)NWORDS_FIELD, sizeof(digit_t));
    PrecompRightCurve(S->E[1], &inst->E[1], S->delta, inst->e);
    S->external_E[1] = false;
    printf("Precomputation done!\n");

    S->lenstrat = OptStrat(strat, (unsigned long)(inst->e - 2 - S->delta) / 2, 1, 1);
    S->strat = calloc(S->lenstrat, sizeof(unsigned long));
    for (i = 0; i < S->lenstrat; i++) {
        S->strat[i] = strat[i];
    }

    /* Initialize Hansel & Gretel */
    S->HANSEL_GRETEL = inst->HANSEL_GRETEL;
    S->MAX_CRUMBS = inst->MAX_CRUMBS;

    /* Initialize vOW params */
    fp2copy(inst->jinv, S->jinv); /* The solution */
    double THETA = inst->ALPHA * sqrt((double)S->MEMORY_SIZE / (double)((unsigned long)1 << (inst->e - 1)));
    S->MAX_STEPS = ceil(inst->GAMMA / THETA);
    S->MAX_DIST = (unsigned long)(inst->BETA * S->MEMORY_SIZE);
    S->MAX_FUNCTION_VERSIONS = 10000;
    S->DIST_BOUND = THETA * (1 << (S->NBITS_STATE - S->MEMORY_LOG_SIZE));
    // assumes we are not filling digit_t, should shift by min(sizeof(digit_t)*8, inst->NBITS_STATE) - S->MEMORY_LOG_SIZE
    assert(S->NBITS_STATE - S->MEMORY_LOG_SIZE > 0);

    /* Statistics */
    S->collect_vow_stats = false; // By default don't collect stats (=> terminate run when successful)
    initTree(&S->dist_cols);      // Initing even if not using
    S->success = false;
    S->wall_time = 0.;
    S->collisions = 0;
    S->mem_collisions = 0;
    S->dist_points = 0;
    S->number_steps_collect = 0;
    S->number_steps_locate = 0;
    S->number_steps = 0;
    S->initial_function_version = 1;
    S->final_avg_random_functions = 0.;

    /* resync */
    S->resync_frequency = 10;
    S->resync_cores = (uint8_t *)calloc(S->N_OF_CORES, sizeof(uint8_t));
}

void free_shared_state(shared_state_t *S)
{
    cleanup_shared_memory(S);
    free(S->strat);

    // Free internally allocated precomputation tables
    for (unsigned int i = 0; i < 2; i++) {
        if (!S->external_E[i]) {
            free(S->E[i]);
        }
    }

    if (S->dist_cols.size != 0) {
        freeTree(S->dist_cols.root);
    }

    free(S->resync_cores);
}

void SampleSIDH(private_state_t *private_state)
{
    sample_prng(&private_state->prng_state, private_state->current.current_state.bytes, (unsigned long)private_state->NBYTES_STATE);

    private_state->current.current_steps = 0;
    fix_overflow(&private_state->current.current_state, private_state->NBYTES_STATE, private_state->NBITS_OVERFLOW);
    copy_st(&private_state->current.initial_state, &private_state->current.current_state, private_state->NWORDS_STATE);

    // Hansel & Gretel
    clean_private_state(private_state);
}

static void LadderThreePtSIDH(point_proj_t R,
                              const f2elm_t a24,
                              const f2elm_t xp,
                              const f2elm_t xq,
                              const f2elm_t xpq,
                              const unsigned char c,
                              const unsigned char *m,
                              unsigned long nbits_state,
                              unsigned long delta)
{ // Non-constant time version that depends on size of k
    point_proj_t R0 = {0}, R2 = {0};
    unsigned int i, bit, msb;

    fp2copy(xq, R0->X);
    fp2copy(xp, R->X);
    fp2copy(xpq, R2->X);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R0->Z);
    fpzero((digit_t *)(R0->Z)[1]);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R->Z);
    fpzero((digit_t *)(R->Z)[1]);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R2->Z);
    fpzero((digit_t *)(R2->Z)[1]);

    msb = GetMSBSIDH(m, nbits_state); /* Can skip top zeroes of k */
    for (i = 2 - c + delta; i < msb; i++) { // Ignore c
        bit = (m[i >> 3] >> (i & 0x07)) & 1;

        if (bit) {
            xDBLADD_SIDH(R0, R, R2->X, a24);
            fp2mul_mont(R->X, R2->Z, R->X);
        } else {
            xDBLADD_SIDH(R0, R2, R->X, a24);
            fp2mul_mont(R2->X, R->Z, R2->X);
        }
    }
}

static void GetIsogeny(f2elm_t jinv,
                       const f2elm_t a24,
                       const f2elm_t xp,
                       const f2elm_t xq,
                       const f2elm_t xpq,
                       const unsigned char c,
                       const unsigned char *k,
                       const unsigned long *strat,
                       const unsigned long lenstrat,
                       unsigned long nbits_state,
                       unsigned long delta)
{
    point_proj_t R, A24, pts[MAX_INT_POINTS_ALICE];
    f2elm_t coeff[3];
    unsigned long i, row, index = 0, ii = 0, m, npts = 0, pts_index[MAX_INT_POINTS_ALICE];

    if (delta + 2 != nbits_state + 1) {
        /* Retrieve kernel point */
        LadderThreePtSIDH(R, a24, xp, xq, xpq, c, k, nbits_state, delta);

        fp2copy(a24, A24->X);
        fpcopy((digit_t *)&Montgomery_one, (digit_t *)A24->Z);
        fpzero((digit_t *)(A24->Z)[1]);

        /* NOTE: Make the first step use the fact that a24 is affine? */
        for (row = 1; row < lenstrat + 1; row++) {
            while (index < lenstrat + 1 - row)
            {
                fp2copy(R->X, pts[npts]->X);
                fp2copy(R->Z, pts[npts]->Z);
                pts_index[npts++] = index;
                m = strat[ii++];
                xDBLe(R, R, A24->X, A24->Z, (int)(2 * m));
                index += m;
            }
            GetFourIsogenyWithKernelXneZ(R, A24->X, A24->Z, coeff);
            for (i = 0; i < npts; i++) {
                EvalFourIsogenyWithKernelXneZ(pts[i], coeff);
            }

            fp2copy(pts[npts - 1]->X, R->X);
            fp2copy(pts[npts - 1]->Z, R->Z);
            index = pts_index[npts - 1];
            npts -= 1;
        }
        GetFourIsogenyWithKernelXneZ(R, A24->X, A24->Z, coeff);

        fp2add(A24->X, A24->X, A24->X);
        fp2sub(A24->X, A24->Z, A24->X);
        fp2add(A24->X, A24->X, A24->X);
        j_inv(A24->X, A24->Z, jinv);

        /* Frobenius */
        fp2correction(jinv);
        if (jinv[1][0] & 1) {
            fpneg(jinv[1]);
        }
    } else {
        fp2copy(a24, jinv);
    }
}

void UpdateStSIDH(unsigned char jinvariant[FP2_ENCODED_BYTES], st_t *r, const st_t *s, private_state_t *private_state)
{
    f2elm_t jinv;
    unsigned char c = GetC_SIDH(s);
    unsigned long index;

    /* Get the j-invariant of the corresponding curve */
    if (c == 0) {
        index = (s->words[0] >> 1) & ((1 << (private_state->delta + 1)) - 1);
        GetIsogeny(jinv, private_state->E[0][index].a24,
                   private_state->E[0][index].xp,
                   private_state->E[0][index].xq,
                   private_state->E[0][index].xpq,
                   c, s->bytes, private_state->strat, private_state->lenstrat, (unsigned long)private_state->NBITS_STATE, private_state->delta);
    } else {
        index = (s->words[0] >> 1) & ((1 << private_state->delta) - 1);
        GetIsogeny(jinv, private_state->E[1][index].a24,
                   private_state->E[1][index].xp,
                   private_state->E[1][index].xq,
                   private_state->E[1][index].xpq,
                   c, s->bytes, private_state->strat, private_state->lenstrat, (unsigned long)private_state->NBITS_STATE, private_state->delta);
    }

    /* Hash j into (c,b,k) */
    fp2_encode(jinv, jinvariant); /* Unique encoding (includes fpcorrection) */
    XOF(r->bytes, jinvariant, (unsigned long)private_state->NBYTES_STATE, FP2_ENCODED_BYTES, (unsigned long)private_state->function_version);
    fix_overflow(r, private_state->NBYTES_STATE, private_state->NBITS_OVERFLOW);
}
