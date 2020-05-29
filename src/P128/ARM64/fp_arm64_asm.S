//*******************************************************************************************
// SIDH: an efficient supersingular isogeny cryptography library
//
// Abstract: field arithmetic in 64-bit ARMv8 assembly for P128 on Linux
//*******************************************************************************************

.text

#ifdef p_32_20    /* p128 = 2^32*3^20*23 - 1 */
p128:
.quad 0xAC0E7A06FFFFFFFF
.quad 0x12
p128x2:
.quad 0x581CF40DFFFFFFFE
.quad 0x25
p128pp:
.quad 0xAC0E7A0700000001
.quad 0x96F0AD1DFAEEAC43
#elif defined p_36_22    /* p128 = 2^36*3^22*31 - 1 */
p128:
.quad 0x2A0B06FFFFFFFFF
.quad 0xE28
p128x2:
.quad 0x54160DFFFFFFFFE
.quad 0x1C50
p128pp:
.quad 0x2A0B07000000001
.quad 0x7AAFBA9EC59A3F28
#elif defined p_40_25    /* p128 = 2^40*3^25*71 - 1 */
p128:
.quad 0xE5D334FFFFFFFFFF
.quad 0x36B681
 p128x2:
.quad 0xE5D3350000000000
.quad 0x36B681
p128pp:
.quad 0xE5D3350000000001
.quad 0xA58AF512692FB681
#elif defined p_44_27    /* p128 = 2^44*3^27*37 - 1 */
 p128:
.quad 0x57606FFFFFFFFFFF
.quad 0x1009C7E1
p128x2:
.quad 0xAEC0DFFFFFFFFFFE
.quad 0x20138FC2
p128pp:
.quad 0x5760700000000001
.quad 0x837790744109C7E1
#elif defined p_48_30    /* p128 = 2^48*3^30*13 - 1 */
p128:
.quad 0x2164FFFFFFFFFFFF
.quad 0x98256F148 
 p128x2:
.quad 0x42C9FFFFFFFFFFFE
.quad 0x1304ADE290
p128pp:
.quad 0x2165000000000001
.quad 0xF72B31E28256F148
#elif defined p_52_33    /* p128 = 2^52*3^33*1 - 1 */
p128:
.quad 0xB82FFFFFFFFFFFFF
.quad 0x13BFEFA65AB
p128x2:
.quad 0x705FFFFFFFFFFFFE
.quad 0x277FDF4CB57
p128pp:
.quad 0xB830000000000001
.quad 0x74A50A3BFEFA65AB
#elif defined p_56_35    /* p128 = 2^56*3^35*57 - 1 */
p128:
.quad 0x82FFFFFFFFFFFFFF
.quad 0x27939F3C5BD1C1
 p128x2:
.quad 0x5FFFFFFFFFFFFFE
.quad 0x4F273E78B7A383
p128pp:
.quad 0x8300000000000001
.quad 0xC930939F3C5BD1C1
#elif defined p_60_38    /* p128 = 2^60*3^38*57 - 1 */
 p128:
.quad 0xFFFFFFFFFFFFFFF
.quad 0x42C91CB5DAF1F68D
p128x2:
.quad 0x1FFFFFFFFFFFFFFE
.quad 0x8592396BB5E3ED1A
p128pp:
.quad 0x1000000000000001
.quad 0xE3C91CB5DAF1F68D
#endif


//***********************************************************************
//  Field addition
//  Operation: c [x2] = a [x0] + b [x1]
//*********************************************************************** 
.global fpadd128_asm
fpadd128_asm:

    // Add a + b
    ldp     x3, x4,   [x0,#0]
    ldp     x11, x12, [x1,#0]
    ldr     x13, p128x2
    ldr     x14, p128x2 + 8
    adds    x3, x3, x11
    adc     x4, x4, x12
    
    //  Subtract 2xp128
    subs    x3, x3, x13
    sbcs    x4, x4, x14
    sbc     x10, xzr, xzr

    // Add 2xp128 anded with the mask in x10
    and     x13, x13, x10 
    and     x14, x14, x10
    adds    x3, x3, x13
    adc     x4, x4, x14
    stp     x3, x4,  [x2,#0]
    ret


//***********************************************************************
//  Field subtraction
//  Operation: c [x2] = a [x0] - b [x1]
//*********************************************************************** 
.global fpsub128_asm
fpsub128_asm:

    // Subtract a - b
    ldp     x3, x4,   [x0,#0]
    ldp     x11, x12, [x1,#0]
    subs    x3, x3, x11
    sbcs    x4, x4, x12
    ldr     x11, p128x2
    ldr     x12, p128x2 + 8
    sbc     x10, xzr, xzr
    
    // Add 2xp128 anded with the mask in x10
    and     x11, x11, x10 
    and     x12, x12, x10
    adds    x3, x3, x11
    adc     x4, x4, x12
    stp     x3, x4,  [x2,#0]
    ret


//***********************************************************************************
//  128-bit integer multiplication
//  Operation: c [x2] = a [x0] * b [x1]
//*********************************************************************************** 
.global mul128_asm
mul128_asm:
    ldp     x3, x4, [x0]
    ldp     x11, x12, [x1,#0]
    mul     x5, x3, x11
    umulh   x6, x3, x11
    mul     x7, x3, x12 
    umulh   x8, x3, x12 

    mul     x9, x4, x11
    umulh   x10, x4, x11
    adds    x7, x7, x6
    adc     x8, x8, xzr 
    
    mul     x11, x4, x12
    umulh   x12, x4, x12
    adds    x7, x7, x9
    adcs    x8, x8, x10
    stp     x5, x7, [x2]        // Output c0-c1
    adc     x6, xzr, xzr
    
    adds    x8, x8, x11
    adc     x6, x6, x12
    stp     x8, x6, [x2,#16]    // Output c2-c3
    ret
    
        
//////////////////////////////////////////// MACRO
.macro MUL64x128  A0, B0, B1, C0, C1, C2, T0
    mul     \C0, \A0, \B0
    umulh   \C1, \A0, \B0

    mul     \T0, \A0, \B1
    umulh   \C2, \A0, \B1 
    adds    \C1, \C1, \T0
    adc     \C2, \C2, xzr
.endm


//**************************************************************************************
//  Montgomery reduction
//  Based on method described in Faz-Hernandez et al. https://eprint.iacr.org/2017/1015  
//  Operation: mc [x1] = ma [x0]
//  NOTE: ma=mc is not allowed
//************************************************************************************** 
.global rdc128_asm
rdc128_asm:
    ldp     x2, x3, [x0,#0]       // a[0-1]
    ldp     x4, x5, [x0,#16]      // a[2-3]

    // Load the prime constants
    ldr     x15, p128pp
    ldr     x16, p128 + 0
    ldr     x17, p128 + 8

    mul     x8, x2, x15           // T mod 2^64 x p128pp
    MUL64x128 x8, x16, x17, x12, x13, x14, X10      
    adds    x2, x2, x12
    adcs    x3, x3, x13   
    adcs    x4, x4, x14
    adc     x5, x5, xzr

    mul     x8, x3, x15           // T mod 2^64 x p128pp
    MUL64x128 x8, x16, x17, x12, x13, x14, X10      
    adds    x3, x3, x12
    adcs    x4, x4, x13   
    adc     x5, x5, x14
    stp     x4, x5, [x1,#0]       // Final result
    ret


//***********************************************************************
//  128-bit multiprecision addition
//  Operation: c [x2] = a [x0] + b [x1]
//*********************************************************************** 
.global mp_add128_asm
mp_add128_asm:
    ldp     x3, x4,   [x0,#0]
    ldp     x11, x12, [x1,#0]
    adds    x3, x3, x11
    adc     x4, x4, x12
    stp     x3, x4,   [x2,#0]
    ret


//***********************************************************************
//  2x128-bit multiprecision subtraction/addition
//  Operation: c [x2] = a [x0] - b [x1]. If c < 0, add p128*2^128
//*********************************************************************** 
.global mp_subadd128x2_asm
mp_subadd128x2_asm:
    ldp     x3, x4,   [x0,#0]
    ldp     x11, x12, [x1,#0]
    ldp     x5, x6,   [x0,#16]
    ldp     x13, x14, [x1,#16]

    subs    x3, x3, x11
    sbcs    x4, x4, x12
    sbcs    x5, x5, x13
    sbcs    x6, x6, x14
    stp     x3, x4,   [x2,#0]	
    ldr     x11, p128
    ldr     x12, p128 + 8
    sbc     x0, xzr, xzr

    // Add p128 anded with the mask in x0 
    and     x11, x11, x0 
    and     x12, x12, x0 
	adds    x5, x5, x11   
	adc     x6, x6, x12 
    stp     x5, x6,   [x2,#16] 
    ret


//***********************************************************************
//  Double 2x434-bit multiprecision subtraction
//  Operation: c [x2] = c [x2] - a [x0] - b [x1]
//*********************************************************************** 
.global mp_dblsub128x2_asm
mp_dblsub128x2_asm:
    ldp     x3, x4,  [x2,#0]
    ldp     x5, x6,  [x2,#16]	
    ldp     x7, x8,  [x0,#0]
    ldp     x9, x10, [x0,#16]
    subs    x3, x3, x7
    sbcs    x4, x4, x8
    ldp     x11, x12, [x1,#0]
    ldp     x13, x14, [x1,#16]
    sbcs    x5, x5, x9
    sbc     x6, x6, x10

    subs    x3, x3, x11
    sbcs    x4, x4, x12
    sbcs    x5, x5, x13
    sbc     x6, x6, x14
    stp     x3, x4,   [x2,#0]
    stp     x5, x6,   [x2,#16]
    ret