#define VOW_SIDH

#include <stdio.h>
#include <time.h>
#include "settings.h"
#include "triples.h"
#include "sidh_vow_base.h"
#include "sidh_vow_base.c"
#include "memory.c"
#include "bintree.c"
#include "curve_math.h"
#include "vow.c"


static void LadderThreePtSIDH(point_proj_t R, const f2elm_t a24, const f2elm_t xp, const f2elm_t xq, const f2elm_t xpq, const unsigned char *m, unsigned char b, unsigned long nbits_state)
{ // Non-constant time version that depends on size of k
    point_proj_t R0 = {0}, R2 = {0};
    unsigned int i, bit, msb = nbits_state;

    fp2copy(xpq, R2->X);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R0->Z);
    fpzero((digit_t *)(R0->Z)[1]);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R->Z);
    fpzero((digit_t *)(R->Z)[1]);
    fpcopy((digit_t *)&Montgomery_one, (digit_t *)R2->Z);
    fpzero((digit_t *)(R2->Z)[1]);

    if (b == 0x02) { // LSB = 0 first
        fp2copy(xp, R0->X);
        fp2copy(xq, R->X);

        xDBLADD_SIDH(R0, R2, R->X, a24);
        fp2mul_mont(R2->X, R->Z, R2->X);
    } else { // Do scalarmult other way around
        fp2copy(xq, R0->X);
        fp2copy(xp, R->X);
    }

    if (b != 0x01) /* Can skip top zeroes of k */
        msb = GetMSBSIDH(m, nbits_state);

    for (i = 3; i < msb; i++) { // Ignore 3 lsb's c,b
        bit = (m[i >> 3] >> (i & 0x07)) & 1;

        if (bit) {
            xDBLADD_SIDH(R0, R, R2->X, a24);
            fp2mul_mont(R->X, R2->Z, R->X);
        } else {
            xDBLADD_SIDH(R0, R2, R->X, a24);
            fp2mul_mont(R2->X, R->Z, R2->X);
        }
    }

    if (b == 0x01) { // MSB b = 1
        xDBLADD_SIDH(R0, R, R2->X, a24);
        fp2mul_mont(R->X, R2->Z, R->X);
    }
}

static void GetIsogeny(f2elm_t jinv, const f2elm_t a24, const f2elm_t xp, const f2elm_t xq, const f2elm_t xpq, const unsigned char *k, const unsigned char b, const unsigned long *strat, const unsigned long lenstrat, unsigned long nbits_state)
{
    point_proj_t R, A24, pts[MAX_INT_POINTS_ALICE];
    f2elm_t coeff[3];
    unsigned long i, row, index = 0, ii = 0, m, npts = 0, pts_index[MAX_INT_POINTS_ALICE];

    /* Retrieve kernel point */
    LadderThreePtSIDH(R, a24, xp, xq, xpq, k, b, nbits_state);

    /* Treat the first row separately */
    while (index < lenstrat)
    {
        fp2copy(R->X, pts[npts]->X);
        fp2copy(R->Z, pts[npts]->Z);
        pts_index[npts++] = index;
        m = strat[ii++];
        xDBLe_affine(R, R, a24, (int)(2 * m));
        index += m;
    }

    if (fp2_is_equal(R->X, R->Z)) {
        for (i = 0; i < npts; i++) {
            EvalFourIsogenyWithKernelXeqZ(pts[i], a24);
        }
        GetFourIsogenyWithKernelXeqZ(A24, a24);
    } else {
        fp2copy(R->Z, A24->Z);
        fp2neg(A24->Z);
        if (fp2_is_equal(R->X, A24->Z)) {
            for (i = 0; i < npts; i++) {
                EvalFourIsogenyWithKernelXeqMinZ(pts[i], a24);
            }
            GetFourIsogenyWithKernelXeqMinZ(A24, a24);
        } else {
            GetFourIsogenyWithKernelXneZ(R, A24->X, A24->Z, coeff);
            for (i = 0; i < npts; i++) {
                EvalFourIsogenyWithKernelXneZ(pts[i], coeff);
            }
        }
    }

    fp2copy(pts[npts - 1]->X, R->X);
    fp2copy(pts[npts - 1]->Z, R->Z);
    index = pts_index[npts - 1];
    npts -= 1;

    /* All steps except the first */
    for (row = 2; row < lenstrat + 1; row++) {
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
}

static void PrecomputeJInvariants(shared_state_t *S)
{
    uint64_t i, j;
    f2elm_t jinv;
    uint64_t index;
    unsigned char c;
    unsigned char b;
    unsigned char k[4];

    for (i = 0; i < ((uint64_t)1 << S->NBITS_STATE); i++) {
        for (j = 0; j < 4; j++)
            k[j] = (i >> (8 * j)) & 0xFF;
        c = k[0] & 0x01;
        b = (k[0] & 0x06) >> 1;
        /* Get the j-invariant of the corresponding curve */
        if (b != 0x03) {
            GetIsogeny(jinv, S->E[c].a24, S->E[c].xp, S->E[c].xq, S->E[c].xpq,
                       k, b, S->strat, S->lenstrat, (unsigned long)S->NBITS_STATE);
            index = i * FP2_ENCODED_BYTES;
            fp2_encode(jinv, &(S->jinvLUT[index])); /* Unique encoding (includes fpcorrection..) */
        }
    }
}

static void PrecomputeJInvariants_DF(shared_state_t *S)
{
    uint64_t h, j;
    f2elm_t jinv;
    f2elm_t coeff[3];
    uint64_t index_r, index_w;
    unsigned char c;
    unsigned char k[4] = {0, 0, 0, 0}, l[4] = {0, 0, 0, 0};
    point_proj_t PpQ, A24, XP, XQ, XPQ, XR;
    point_proj_t A242, XP2, XQ2, XQ2_, XPQ2;
    CurveAndPointsSIDH *E;

    // Allocate memory for the curves and points computed on the way
    // 3 * 4 * 2^(e-1) Fp2 elements per curve
    E = calloc(3 * (1 << (S->NBITS_STATE - 3)) * 4 * 2 * NWORDS_FIELD, sizeof(digit_t));

    for (c = 0; c < 2; c++) {
        // Left and right curve have the same precomputation
        // get starting curve and points
        fp2copy(S->E[c].a24, A24->X);
        fpcopy((digit_t *)&Montgomery_one, A24->Z[0]);
        fpzero(A24->Z[1]);
        fp2copy(S->E[c].xp, XP->X);
        fpcopy((digit_t *)&Montgomery_one, XP->Z[0]); // P
        fpzero(XP->Z[1]);
        fp2copy(S->E[c].xq, XQ->X);
        fpcopy((digit_t *)&Montgomery_one, XQ->Z[0]);
        fpzero(XQ->Z[1]);
        fp2copy(S->E[c].xpq, XPQ->X);
        fpcopy((digit_t *)&Montgomery_one, XPQ->Z[0]); // P-Q
        fpzero(XPQ->Z[1]);

        fp2copy(S->E[c].xq, XQ2->X);
        fpcopy((digit_t *)&Montgomery_one, XQ2->Z[0]);
        fpzero(XQ2->Z[1]);

        fp2copy(S->E[c].xp, PpQ->X);
        fpcopy((digit_t *)&Montgomery_one, PpQ->Z[0]);
        fpzero(PpQ->Z[1]);

        // Sum PpQ = x(P + Q), XQ2 = x(2Q)
        xDBLADD(XQ2, PpQ, XPQ->X, A24->X);
        fp2inv_mont(PpQ->Z);
        fp2mul_mont(PpQ->X, PpQ->Z, PpQ->X);
        // XQ2 = x(4Q)
        xDBL_affine(XQ2, XQ2, A24->X);

        // First 4-isogeny for b in {0,1}
        for (j = 0; j < 4; j++) {
            k[0] = (unsigned char)(j << 3);
            l[0] = (unsigned char)((4 - j) << 3);

            fp2copy(XQ2->X, XQ2_->X);
            fp2copy(XQ2->Z, XQ2_->Z);

            // Compute points to be moved through the isogeny for new basis
            LadderThreePtSIDH(XP2, A24->X, XP->X, XQ->X, XPQ->X, k, 0x00, 5);
            LadderThreePtSIDH(XPQ2, A24->X, XP->X, XQ->X, PpQ->X, l, 0x00, 6);
            // Compute kernel point of order 4
            xDBLe_affine(XP2, XR, A24->X, (int)S->NBITS_STATE - 4);

            // Compute first 4-isogeny
            if (fp2_is_equal(XR->X, XR->Z)) {
                EvalFourIsogenyWithKernelXeqZ(XP2, A24->X);
                EvalFourIsogenyWithKernelXeqZ(XPQ2, A24->X);
                EvalFourIsogenyWithKernelXeqZ(XQ2_, A24->X);
                GetFourIsogenyWithKernelXeqZ(A242, A24->X);
            } else {
                fp2copy(XR->Z, A242->Z);
                fp2neg(A242->Z);
                if (fp2_is_equal(XR->X, A242->Z)) {
                    EvalFourIsogenyWithKernelXeqMinZ(XP2, A24->X);
                    EvalFourIsogenyWithKernelXeqMinZ(XPQ2, A24->X);
                    EvalFourIsogenyWithKernelXeqMinZ(XQ2_, A24->X);
                    GetFourIsogenyWithKernelXeqMinZ(A242, A24->X);
                } else {
                    GetFourIsogenyWithKernelXneZ(XR, A242->X, A242->Z, coeff);
                    EvalFourIsogenyWithKernelXneZ(XP2, coeff);
                    EvalFourIsogenyWithKernelXneZ(XPQ2, coeff);
                    EvalFourIsogenyWithKernelXneZ(XQ2_, coeff);
                }
            }

            // Store to allocated memory
            index_w = j;
            FourwayInv(A242->Z, XP2->Z, XQ2_->Z, XPQ2->Z);
            fp2mul_mont(A242->X, A242->Z, E[index_w].a24);
            fp2mul_mont(XP2->X, XP2->Z, E[index_w].xp);
            fp2mul_mont(XQ2_->X, XQ2_->Z, E[index_w].xq);
            fp2mul_mont(XPQ2->X, XPQ2->Z, E[index_w].xpq);
        }

        // Now (e-2)/2 more 4-isogenies for full precompuation
        for (h = 1; h < (S->NBITS_STATE - 2) / 2; h++) {
            for (index_r = 0; index_r < (uint64_t)1 << (2 * h); index_r++) {
                // Read current points from memory
                fp2copy(E[index_r].a24, A24->X);
                fp2copy(E[index_r].xp, XP->X);
                fp2copy(E[index_r].xq, XQ->X);
                fp2copy(E[index_r].xpq, XPQ->X);

                fp2copy(XQ->X, XQ2->X);
                fpcopy((digit_t *)&Montgomery_one, XQ2->Z[0]);
                fpzero(XQ2->Z[1]);

                fp2copy(XP->X, PpQ->X);
                fpcopy((digit_t *)&Montgomery_one, PpQ->Z[0]);
                fpzero(PpQ->Z[1]);

                // Sum PpQ = x(P + Q), XQ2 = x(2Q)
                xDBLADD(XQ2, PpQ, XPQ->X, A24->X);
                fp2inv_mont(PpQ->Z);
                fp2mul_mont(PpQ->X, PpQ->Z, PpQ->X);
                // XQ2 = x(4Q)
                xDBL_affine(XQ2, XQ2, A24->X);

                // b in {0,1}
                for (j = 0; j < 4; j++) {
                    k[0] = (unsigned char)(j << 3);
                    l[0] = (unsigned char)((4 - j) << 3);

                    fp2copy(XQ2->X, XQ2_->X);
                    fp2copy(XQ2->Z, XQ2_->Z);

                    LadderThreePtSIDH(XP2, A24->X, XP->X, XQ->X, XPQ->X, k, 0x00, 5);
                    LadderThreePtSIDH(XPQ2, A24->X, XP->X, XQ->X, PpQ->X, l, 0x00, 6);
                    xDBLe_affine(XP2, XR, A24->X, (int)(S->NBITS_STATE - 4 - 2 * h));

                    GetFourIsogenyWithKernelXneZ(XR, A242->X, A242->Z, coeff);
                    EvalFourIsogenyWithKernelXneZ(XP2, coeff);
                    EvalFourIsogenyWithKernelXneZ(XQ2_, coeff);
                    EvalFourIsogenyWithKernelXneZ(XPQ2, coeff);

                    // Store to allocated memory
                    if (2 * h + 2 != S->NBITS_STATE - 2) {
                        index_w = index_r + j * ((uint64_t)1 << (2 * h));

                        FourwayInv(A242->Z, XP2->Z, XQ2_->Z, XPQ2->Z);
                        fp2mul_mont(A242->X, A242->Z, E[index_w].a24);
                        fp2mul_mont(XP2->X, XP2->Z, E[index_w].xp);
                        fp2mul_mont(XQ2_->X, XQ2_->Z, E[index_w].xq);
                        fp2mul_mont(XPQ2->X, XPQ2->Z, E[index_w].xpq);
                    } else { /* Just store j-invariants to make it faster */
                        index_w = c + 2 * (j >> 1) + 8 * (index_r + (j & 0x01) * ((uint64_t)1 << (2 * h)));
                        fp2add(A242->X, A242->X, A242->X);
                        fp2sub(A242->X, A242->Z, A242->X);
                        fp2add(A242->X, A242->X, A242->X);
                        j_inv(A242->X, A242->Z, jinv);
                        fp2_encode(jinv, &(S->jinvLUT[index_w * FP2_ENCODED_BYTES])); /* Unique encoding (includes fpcorrection..) */
                    }
                }
            }
        }

        // Set up b = 2 case
        fp2copy(S->E[c].a24, A24->X);
        fpcopy((digit_t *)&Montgomery_one, A24->Z[0]);
        fpzero(A24->Z[1]);
        fp2copy(S->E[c].xp, XP->X);
        fpcopy((digit_t *)&Montgomery_one, XP->Z[0]); // P
        fpzero(XP->Z[1]);
        fp2copy(S->E[c].xq, XQ->X);
        fpcopy((digit_t *)&Montgomery_one, XQ->Z[0]);
        fpzero(XQ->Z[1]);
        fp2copy(S->E[c].xpq, XPQ->X);
        fpcopy((digit_t *)&Montgomery_one, XPQ->Z[0]); // P-Q
        fpzero(XPQ->Z[1]);

        fp2copy(S->E[c].xp, XQ2->X);
        fpcopy((digit_t *)&Montgomery_one, XQ2->Z[0]);
        fpzero(XQ2->Z[1]);

        fp2copy(S->E[c].xq, PpQ->X);
        fpcopy((digit_t *)&Montgomery_one, PpQ->Z[0]);
        fpzero(PpQ->Z[1]);

        // Sum PpQ = x(P + Q), XQ2 = x(2P)
        xDBLADD(XQ2, PpQ, XPQ->X, A24->X);
        fp2inv_mont(PpQ->Z);
        fp2mul_mont(PpQ->X, PpQ->Z, PpQ->X);
        // XQ2 = x(4P)
        xDBL_affine(XQ2, XQ2, A24->X);

        // First 4-isogeny for b = 2
        for (j = 0; j < 2; j++) {
            k[0] = (unsigned char)((2 * j) << 3);
            l[0] = (unsigned char)((4 - (2 * j)) << 3);

            fp2copy(XQ2->X, XQ2_->X);
            fp2copy(XQ2->Z, XQ2_->Z);

            // Compute points to be moved through the isogeny for new basis
            LadderThreePtSIDH(XP2, A24->X, XQ->X, XP->X, XPQ->X, k, 0x00, 5);
            LadderThreePtSIDH(XPQ2, A24->X, XQ->X, XP->X, PpQ->X, l, 0x00, 6);
            // Compute kernel point of order 4
            xDBLe_affine(XP2, XR, A24->X, (int)S->NBITS_STATE - 4);

            // Compute first 4-isogeny
            if (fp2_is_equal(XR->X, XR->Z)) {
                EvalFourIsogenyWithKernelXeqZ(XP2, A24->X);
                EvalFourIsogenyWithKernelXeqZ(XPQ2, A24->X);
                EvalFourIsogenyWithKernelXeqZ(XQ2_, A24->X);
                GetFourIsogenyWithKernelXeqZ(A242, A24->X);
            } else {
                fp2copy(XR->Z, A242->Z);
                fp2neg(A242->Z);
                if (fp2_is_equal(XR->X, A242->Z)) {
                    EvalFourIsogenyWithKernelXeqMinZ(XP2, A24->X);
                    EvalFourIsogenyWithKernelXeqMinZ(XPQ2, A24->X);
                    EvalFourIsogenyWithKernelXeqMinZ(XQ2_, A24->X);
                    GetFourIsogenyWithKernelXeqMinZ(A242, A24->X);
                } else {
                    GetFourIsogenyWithKernelXneZ(XR, A242->X, A242->Z, coeff);
                    EvalFourIsogenyWithKernelXneZ(XP2, coeff);
                    EvalFourIsogenyWithKernelXneZ(XPQ2, coeff);
                    EvalFourIsogenyWithKernelXneZ(XQ2_, coeff);
                }
            }

            // Store to allocated memory
            index_w = j;
            FourwayInv(A242->Z, XP2->Z, XQ2_->Z, XPQ2->Z);
            fp2mul_mont(A242->X, A242->Z, E[index_w].a24);
            fp2mul_mont(XP2->X, XP2->Z, E[index_w].xp);
            fp2mul_mont(XQ2_->X, XQ2_->Z, E[index_w].xq);
            fp2mul_mont(XPQ2->X, XPQ2->Z, E[index_w].xpq);
        }

        // Now (e-2)/2 more 4-isogenies for full precompuation
        for (h = 1; h < (S->NBITS_STATE - 2) / 2; h++) {
            for (index_r = 0; index_r < 2 * (1 << 2 * (h - 1)); index_r++) {
                // Read current points from memory
                fp2copy(E[index_r].a24, A24->X);
                fp2copy(E[index_r].xp, XP->X);
                fp2copy(E[index_r].xq, XQ->X);
                fp2copy(E[index_r].xpq, XPQ->X);

                fp2copy(XQ->X, XQ2->X);
                fpcopy((digit_t *)&Montgomery_one, XQ2->Z[0]);
                fpzero(XQ2->Z[1]);

                fp2copy(XP->X, PpQ->X);
                fpcopy((digit_t *)&Montgomery_one, PpQ->Z[0]);
                fpzero(PpQ->Z[1]);

                // Sum PpQ = x(P + Q), X2Q = x(2Q)
                xDBLADD(XQ2, PpQ, XPQ->X, A24->X);
                fp2inv_mont(PpQ->Z);
                fp2mul_mont(PpQ->X, PpQ->Z, PpQ->X);
                // XQ2 = x(4Q)
                xDBL_affine(XQ2, XQ2, A24->X);

                // b in {0,1}
                for (j = 0; j < 4; j++) {
                    k[0] = (unsigned char)(j << 3);
                    l[0] = (unsigned char)((4 - j) << 3);

                    fp2copy(XQ2->X, XQ2_->X);
                    fp2copy(XQ2->Z, XQ2_->Z);

                    LadderThreePtSIDH(XP2, A24->X, XP->X, XQ->X, XPQ->X, k, 0x00, 5);
                    LadderThreePtSIDH(XPQ2, A24->X, XP->X, XQ->X, PpQ->X, l, 0x00, 6);
                    xDBLe_affine(XP2, XR, A24->X, (int)(S->NBITS_STATE - 4 - 2 * h));

                    GetFourIsogenyWithKernelXneZ(XR, A242->X, A242->Z, coeff);
                    EvalFourIsogenyWithKernelXneZ(XP2, coeff);
                    EvalFourIsogenyWithKernelXneZ(XQ2_, coeff);
                    EvalFourIsogenyWithKernelXneZ(XPQ2, coeff);

                    // Store to allocated memory
                    if (2 * h + 2 != S->NBITS_STATE - 2) {
                        index_w = index_r + j * 2 * ((uint64_t)1 << (2 * (h - 1)));

                        FourwayInv(A242->Z, XP2->Z, XQ2_->Z, XPQ2->Z);
                        fp2mul_mont(A242->X, A242->Z, E[index_w].a24);
                        fp2mul_mont(XP2->X, XP2->Z, E[index_w].xp);
                        fp2mul_mont(XQ2_->X, XQ2_->Z, E[index_w].xq);
                        fp2mul_mont(XPQ2->X, XPQ2->Z, E[index_w].xpq);
                    } else { /* Just store j-invariants to make it faster */
                        index_w = c + 2 * 2 + 8 * (index_r + j * 2 * ((uint64_t)1 << (2 * (h - 1))));
                        fp2add(A242->X, A242->X, A242->X);
                        fp2sub(A242->X, A242->Z, A242->X);
                        fp2add(A242->X, A242->X, A242->X);
                        j_inv(A242->X, A242->Z, jinv);
                        fp2_encode(jinv, &(S->jinvLUT[index_w * FP2_ENCODED_BYTES])); /* Unique encoding (includes fpcorrection..) */
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
    unsigned long RAD = 64; /* TODO: Assume 64-bit arch?? */
    S->NBITS_STATE = inst->NBITS_K + 3;
    S->NBYTES_STATE = ((S->NBITS_STATE + 7) / 8);         /* Number of bytes needed for state */
    S->NWORDS_STATE = ((S->NBITS_STATE + RAD - 1) / RAD); /* Number of words need for state */
    S->NBITS_OVERFLOW = S->NBITS_STATE % 8;
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

    /* Initialize isogeny functions */
    fp2copy(inst->E[0].a24, S->E[0].a24);
    fp2copy(inst->E[0].xp, S->E[0].xp);
    fp2copy(inst->E[0].xq, S->E[0].xq);
    fp2copy(inst->E[0].xpq, S->E[0].xpq);
    fp2copy(inst->E[1].a24, S->E[1].a24);
    fp2copy(inst->E[1].xp, S->E[1].xp);
    fp2copy(inst->E[1].xq, S->E[1].xq);
    fp2copy(inst->E[1].xpq, S->E[1].xpq);
    fp2copy(inst->jinv, S->jinv);
    S->lenstrat = OptStrat(strat, (unsigned long)(inst->NBITS_K + 1) / 2, 1, 1); /* TODO: Take actual costs instead of balanced strategy.*/
    S->strat = calloc(S->lenstrat, sizeof(unsigned long));
    for (i = 0; i < S->lenstrat; i++)
        S->strat[i] = strat[i];

    /* Initialize pre-computed table of j-invariants */
    if (inst->PRECOMP) {
        printf("Precomputing...\n");
        S->jinvLUT = calloc((size_t)(1 << S->NBITS_STATE) * FP2_ENCODED_BYTES, 1);
        PrecomputeJInvariants_DF(S);
        printf("Precomputation done!\n");
    }
    S->PRECOMP = inst->PRECOMP;

    /* Initialise H&G */
    S->HANSEL_GRETEL = inst->HANSEL_GRETEL;
    S->MAX_CRUMBS = inst->MAX_CRUMBS;

    /* Initialize vOW params */
    double THETA = inst->ALPHA * sqrt((double)S->MEMORY_SIZE / (double)(3 * (1 << (inst->NBITS_K + 1))));
    S->MAX_STEPS = ceil(inst->GAMMA / THETA);
    S->MAX_DIST = (unsigned long)(inst->BETA * S->MEMORY_SIZE);
    S->MAX_FUNCTION_VERSIONS = 10000;
    // assumes we are not filling digit_t, should shift by min(sizeof(digit_t)*8, inst->NBITS_STATE) - 3 - S->MEMORY_LOG_SIZE
    S->DIST_BOUND = THETA * (1 << (S->NBITS_STATE - 3 - S->MEMORY_LOG_SIZE));
    assert(S->NBITS_STATE > 3 + S->MEMORY_LOG_SIZE); //  > -log(theta)); // we want some resolution for memory

    /* Statistics */
    S->collect_vow_stats = false; // by default don't collect stats (=> terminate run when successful)
    initTree(&S->dist_cols);      // we are initing even if not using
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
    if (S->PRECOMP) {
        free(S->jinvLUT);
    }

    if (S->dist_cols.size != 0) {
        freeTree(S->dist_cols.root);
    }

    free(S->resync_cores);
}

void SampleSIDH(private_state_t *private_state)
{
    unsigned char t;

    sample_prng(&private_state->prng_state, private_state->current.current_state.bytes, (unsigned long)private_state->NBYTES_STATE);

    private_state->current.current_steps = 0;
    if (GetB_SIDH(&private_state->current.current_state) == 0x03) { // b = 3
        do
        {
            sample_prng(&private_state->prng_state, &t, 1);
            t &= 0x03;
        } while (t == 0x03);
        SetB_SIDH(&private_state->current.current_state, t);
    }

    private_state->current.current_state.bytes[private_state->NBYTES_STATE - 1] &= (0xFF >> ((8 - private_state->NBITS_OVERFLOW) % 8)); /* Zero top bits */
    copy_st(&private_state->current.initial_state, &private_state->current.current_state, private_state->NWORDS_STATE);

    // Hansel & Gretel
    clean_private_state(private_state);
}

void UpdateStSIDH(unsigned char jinvariant[FP2_ENCODED_BYTES], st_t *r, const st_t *s, private_state_t *private_state)
{
    f2elm_t jinv;
    uint64_t i;
    unsigned char c = GetC_SIDH(s);
    unsigned char b = GetB_SIDH(s);

    if (private_state->PRECOMP) {
        for (i = 0; i < FP2_ENCODED_BYTES; i++) {
            jinvariant[i] = private_state->jinvLUT[(s->words[0]) * FP2_ENCODED_BYTES + i];
        }
    } else {
        /* Get the j-invariant of the corresponding curve */
        GetIsogeny(jinv, private_state->E[c].a24,
                   private_state->E[c].xp,
                   private_state->E[c].xq,
                   private_state->E[c].xpq,
                   s->bytes, b, private_state->strat, private_state->lenstrat, (unsigned long)private_state->NBITS_STATE);

        /* Hash j into (c,b,k) */
        fp2_encode(jinv, jinvariant); /* Unique encoding (includes fpcorrection..) */
    }
    assert(private_state->function_version < ((uint64_t)1 << 32)); /* Otherwise looping... */
    XOF(r->bytes, jinvariant, (unsigned long)private_state->NBYTES_STATE, FP2_ENCODED_BYTES, (unsigned long)private_state->function_version);

    /* Get b != 0x03 */
    c = 3; /* Use as a counter, ignore b and c */
    b = GetB_SIDH(r);
    if (b == 0x03) {
        while (b == 0x03)
        { /* Loop over bits from the right until we find 11 */
            XOF(&b, &c, 1, 1, (unsigned long)private_state->function_version);
            b &= 0x03;
            c++;
        }
        SetB_SIDH(r, b);
    }
    r->bytes[private_state->NBYTES_STATE - 1] &= (0xFF >> ((8 - private_state->NBITS_OVERFLOW) % 8)); /* Zero top bits */
}
