/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: supersingular isogeny parameters and generation of functions for P128
*********************************************************************************************/  

#include "P128_internal.h"


// Encoding of field elements, elements over Z_order, elements over GF(p^2) and elliptic curve points:
// --------------------------------------------------------------------------------------------------
// Elements over GF(p) and Z_order are encoded with the least significant octet (and digit) located at the leftmost position (i.e., little endian format). 
// Elements (a+b*i) over GF(p^2), where a and b are defined over GF(p), are encoded as {a, b}, with a in the least significant position.
// Elliptic curve points P = (x,y) are encoded as {x, y}, with x in the least significant position. 
// Internally, the number of digits used to represent all these elements is obtained by approximating the number of bits to the immediately greater multiple of 32.
// For example, a 128-bit field element is represented with Ceil(128 / 64) = 8 64-bit digits or Ceil(128 / 32) = 16 32-bit digits.

//
// Curve isogeny system "SIDHp128". Base curve: Montgomery curve By^2 = Cx^3 + Ax^2 + Cx defined over GF(p128^2), where A=0, B=1, C=1
//

/*
 * p128p1 = p128 + 1
 * p128x2 = 2*p128
 * Montgomery_rprime = -(p128)^-1 mod 2^128
 * Montgomery_R2 = (2^128)^2 mod p128
 * Montgomery_one = 2^128 mod p128
 */

#ifdef p_32_20
    /* p128 = 2^32*3^20*23 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0xAC0E7A06FFFFFFFF, 0x12 };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0xAC0E7A0700000000, 0x12 };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0x581CF40DFFFFFFFE, 0x25 };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0xAC0E7A0700000001, 0x96F0AD1DFAEEAC43 };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0x835010E3A34C2C1C, 0x3 };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0xD9FEFBEAD8BA0D2B, 0x4 };
#elif defined p_36_22
    /* p128 = 2^36*3^22*31 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0x2A0B06FFFFFFFFF, 0xE28 };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0x2A0B07000000000, 0xE28 };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0x54160DFFFFFFFFE, 0x1C50 };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0x2A0B07000000001, 0x7AAFBA9EC59A3F28 };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0x9AEB249E616945D3, 0xDDE };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0x68B83F2624F57D33, 0x486 };
#elif defined p_40_25
    /* p128 = 2^40*3^25*71 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0xE5D334FFFFFFFFFF, 0x36B681 };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0xE5D3350000000000, 0x36B681 };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0xCBA669FFFFFFFFFE, 0x6D6D03 };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0xE5D3350000000001, 0xA58AF512692FB681 };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0x5AA0B43B3D5B01AA, 0xCCB7 };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0xDB196FADD0D8D1E1, 0x534EE };
#elif defined p_44_27
    /* p128 = 2^44*3^27*37 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0x57606FFFFFFFFFFF, 0x1009C7E1 };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0x5760700000000000, 0x1009C7E1 };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0xAEC0DFFFFFFFFFFE, 0x20138FC2 };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0x5760700000000001, 0x837790744109C7E1 };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0xAA642DA60E97D90D, 0xAE20DAF };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0x329900FF63E15A1, 0xB436A22 };
#elif defined p_48_30
    /* p128 = 2^48*3^30*13 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0x2164FFFFFFFFFFFF, 0x98256F148 };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0x2165000000000000, 0x98256F148 };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0x42C9FFFFFFFFFFFE, 0x1304ADE290 };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0x2165000000000001, 0xF72B31E28256F148 };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0xDCA5F9809FE37F5D, 0xE4ED25EF };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0x4D9A00001AEBE56E, 0x16833E36B };
#elif defined p_52_33
    /* p128 = 2^52*3^33*1 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0xB82FFFFFFFFFFFFF, 0x13BFEFA65AB };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0xB830000000000000, 0x13BFEFA65AB };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0x705FFFFFFFFFFFFE, 0x277FDF4CB57 };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0xB830000000000001, 0x74A50A3BFEFA65AB };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0x846A8025CD80EB12, 0xC904FA0794 };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0xA00000000CF6520, 0x6F20BC9C0E };
#elif defined p_56_35
    /* p128 = 2^56*3^35*57 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0x82FFFFFFFFFFFFFF, 0x27939F3C5BD1C1 };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0x8300000000000000, 0x27939F3C5BD1C1 };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0x5FFFFFFFFFFFFFE, 0x4F273E78B7A383 };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0x8300000000000001, 0xC930939F3C5BD1C1 };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0xE8AD679AF3B8E9A7, 0x22F40EB68BA748 };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0x1B00000000000677, 0x24A590CA66F5FA };
#elif defined p_60_38
    /* p128 = 2^60*3^38*57 - 1 */
    const uint64_t p128[NWORDS64_FIELD] = { 0xFFFFFFFFFFFFFFF, 0x42C91CB5DAF1F68D };
    const uint64_t p128p1[NWORDS64_FIELD] = { 0x1000000000000000, 0x42C91CB5DAF1F68D };
    const uint64_t p128x2[NWORDS64_FIELD] = { 0x1FFFFFFFFFFFFFFE, 0x8592396BB5E3ED1A };
    const uint64_t Montgomery_rprime1[NWORDS64_ORDER] = { 0x1000000000000001, 0xE3C91CB5DAF1F68D };
    const uint64_t Montgomery_R2[NWORDS64_ORDER] = { 0xA753F4FB067D5E52, 0x1EFF2E1E8249D906 };
    const uint64_t Montgomery_one[NWORDS64_ORDER] = { 0xD000000000000003, 0x37A4A9DE6F2A1C58 };
#endif

// Setting up macro defines and including GF(p), GF(p^2), curve, isogeny and kex functions
#define fpcopy                        fpcopy128
#define fpzero                        fpzero128
#define fpadd                         fpadd128
#define fpsub                         fpsub128
#define fpneg                         fpneg128
#define fpdiv2                        fpdiv2_128
#define fpcorrection                  fpcorrection128
#define fpmul_mont                    fpmul128_mont
#define fpsqr_mont                    fpsqr128_mont
#define fpinv_mont                    fpinv128_mont
#define fpinv_chain_mont              fpinv128_chain_mont
#define fpinv_mont_bingcd             fpinv128_mont_bingcd
#define fp2copy                       fp2copy128
#define fp2zero                       fp2zero128
#define fp2add                        fp2add128
#define fp2sub                        fp2sub128
#define fp2neg                        fp2neg128
#define fp2div2                       fp2div2_128
#define fp2correction                 fp2correction128
#define fp2mul_mont                   fp2mul128_mont
#define fp2sqr_mont                   fp2sqr128_mont
#define fp2inv_mont                   fp2inv128_mont
#define fp2inv_mont_bingcd            fp2inv128_mont_bingcd
#define mp_add_asm                    mp_add128_asm
#define mp_subaddx2_asm               mp_subadd128x2_asm
#define mp_dblsubx2_asm               mp_dblsub128x2_asm
#define crypto_kem_keypair            crypto_kem_keypair_SIKEp128
#define crypto_kem_enc                crypto_kem_enc_SIKEp128
#define crypto_kem_dec                crypto_kem_dec_SIKEp128
#define random_mod_order_A            random_mod_order_A_SIDHp128
#define random_mod_order_B            random_mod_order_B_SIDHp128

#include "../fpx.c"
#include "../ec_isogeny.c"