//*******************************************************************************************
// SIDH: an efficient supersingular isogeny cryptography library
//
// Abstract: field arithmetic in x64 assembly for P128 on Linux
//*******************************************************************************************  

.intel_syntax noprefix 

// Registers that are used for parameter passing:
#define reg_p1  rdi
#define reg_p2  rsi
#define reg_p3  rdx

.text

#ifdef p_32_20  
// p128 = 2^32*3^20*23 - 1
p128:
.quad     0xAC0E7A06FFFFFFFF
.quad     0x0000000000000012
// p128 x 2
p128x2:
.quad   0x581CF40DFFFFFFFE
.quad   0x0000000000000025
// -(p128)^-1 mod 2^128
p128pp:
.quad   0xAC0E7A0700000001
.quad   0x96F0AD1DFAEEAC43

#elif defined p_36_22
// p128 = 2^36*3^22*31 - 1
p128:
.quad     0x02A0B06FFFFFFFFF
.quad     0x0000000000000E28
// p128 x 2
p128x2:
.quad   0x054160DFFFFFFFFE
.quad   0x0000000000001C50
// -(p128)^-1 mod 2^128
p128pp:
.quad   0x02A0B07000000001
.quad   0x7AAFBA9EC59A3F28

#elif defined p_40_25
// p128 = 2^40*3^25*71 - 1
p128:
.quad     0xE5D334FFFFFFFFFF
.quad     0x000000000036B681
// p128 x 2
p128x2:
.quad   0xCBA669FFFFFFFFFE
.quad   0x00000000006D6D03
// -(p128)^-1 mod 2^128
p128pp:
.quad   0xE5D3350000000001
.quad   0xA58AF512692FB681

#elif defined p_44_27
// p128 = 2^44*3^27*37 - 1
p128:
.quad     0x57606FFFFFFFFFFF
.quad     0x000000001009C7E1
// p128 x 2
p128x2:
.quad   0xAEC0DFFFFFFFFFFE
.quad   0x0000000020138FC2
// -(p128)^-1 mod 2^128
p128pp:
.quad   0x5760700000000001
.quad   0x837790744109C7E1

#elif defined p_48_30
// p128 = 2^48*3^30*13 - 1
p128:
.quad     0x2164FFFFFFFFFFFF
.quad     0x000000098256F148
// p128 x 2
p128x2:
.quad   0x42C9FFFFFFFFFFFE
.quad   0x0000001304ADE290
// -(p128)^-1 mod 2^128
p128pp:
.quad   0x2165000000000001
.quad   0xF72B31E28256F148

#elif defined p_52_33
// p128 = 2^52*3^33*1 - 1
p128:
.quad     0xB82FFFFFFFFFFFFF
.quad     0x0000013BFEFA65AB
// p128 x 2
p128x2:
.quad   0x705FFFFFFFFFFFFE
.quad   0x00000277FDF4CB57
// -(p128)^-1 mod 2^128
p128pp:
.quad   0xB830000000000001
.quad   0x74A50A3BFEFA65AB

#elif defined p_56_35
// p128 = 2^56*3^35*57 - 1
p128:
.quad     0x82FFFFFFFFFFFFFF
.quad     0x0027939F3C5BD1C1
// p128 x 2
p128x2:
.quad   0x05FFFFFFFFFFFFFE
.quad   0x004F273E78B7A383
// -(p128)^-1 mod 2^128
p128pp:
.quad   0x8300000000000001
.quad   0xC930939F3C5BD1C1

#elif defined p_60_38
// p128 = 2^60*3^38*57 - 1
p128:
.quad     0xFFFFFFFFFFFFFFF
.quad     0x42C91CB5DAF1F68D
// p128 x 2
p128x2:
.quad   0x1FFFFFFFFFFFFFFE
.quad   0x8592396BB5E3ED1A
// -(p128)^-1 mod 2^128
p128pp:
.quad   0x1000000000000001
.quad   0xE3C91CB5DAF1F68D
#endif


//***********************************************************************
//  Field addition
//  Operation: c [reg_p3] = a [reg_p1] + b [reg_p2]
//*********************************************************************** 
.global fpadd128_asm
fpadd128_asm:  
  xor    rax, rax
  mov    r8, [reg_p1]
  mov    r9, [reg_p1+8]
  add    r8, [reg_p2] 
  adc    r9, [reg_p2+8]

  mov    rcx, [rip+p128x2]
  sub    r8, rcx
  mov    rcx, [rip+p128x2+8]
  sbb    r9, rcx
  sbb    rax, 0
  
  mov    rdi, [rip+p128x2]
  and    rdi, rax
  mov    rsi, [rip+p128x2+8]
  and    rsi, rax
  
  add    r8, rdi  
  adc    r9, rsi
  mov    [reg_p3], r8
  mov    [reg_p3+8], r9
  ret


//***********************************************************************
//  Field subtraction
//  Operation: c [reg_p3] = a [reg_p1] - b [reg_p2]
//*********************************************************************** 
.global fpsub128_asm
fpsub128_asm:  
  xor    rax, rax
  mov    r8, [reg_p1]
  mov    r9, [reg_p1+8]
  sub    r8, [reg_p2] 
  sbb    r9, [reg_p2+8] 
  sbb    rax, 0
  
  mov    rdi, [rip+p128x2]
  and    rdi, rax
  mov    rsi, [rip+p128x2+8]
  and    rsi, rax
  
  add    r8, rdi  
  adc    r9, rsi
  mov    [reg_p3], r8
  mov    [reg_p3+8], r9
  ret


//***********************************************************************
//  Integer multiplication
//  Operation: c [reg_p3] = a [reg_p1] * b [reg_p2]
//  NOTE: a=c or b=c are not allowed
//***********************************************************************
.global mul128_asm
mul128_asm:
  xor    r10, r10
  mov    rcx, reg_p3
  
  mov    rax, [reg_p2]
  mov    r8, [reg_p1]
  mul    r8
  mov    [rcx], rax        // c0
  mov    r9, rdx
  
  mov    rax, [reg_p2+8]
  mul    r8
  add    r9, rax
  adc    r10, rdx
  
  xor    r11, r11
  mov    rax, [reg_p2]
  mov    r8, [reg_p1+8] 
  mul    r8
  add    r9, rax
  mov    [rcx+8], r9       // c1 
  adc    r10, rdx
  adc    r11, 0
  
  mov    rax, [reg_p2+8]
  mul    r8
  add    r10, rax          
  adc    r11, rdx         
  mov    [rcx+16], r10      // c2 
  mov    [rcx+24], r11      // c3 
  ret

  
//***********************************************************************
//  Montgomery reduction
//  Operation: c [reg_p2] = a [reg_p1]
//  NOTE: a=c is not allowed
//*********************************************************************** 
.global rdc128_asm
rdc128_asm: 
  mov    rax, [rip+p128pp]
  mov    r8, [reg_p1]
  mul    r8
  mov    [reg_p2], rax     // c0
  mov    r9, rdx
  
  mov    rax, [rip+p128pp+8]
  mul    r8
  add    r9, rax
  
  mov    rax, [rip+p128pp]
  mov    r8, [reg_p1+8]
  mul    r8
  add    r9, rax
  mov    [reg_p2+8], r9    // c1
    
  // mc = (ma + mc x p128)/2^128
  xor    r10, r10  
  mov    rax, [rip+p128]
  mov    r8, [reg_p2]
  mul    r8
  mov    rcx, rax          // mc0
  mov    r9, rdx
  
  mov    rax, [rip+p128+8]
  mul    r8
  add    r9, rax
  adc    r10, rdx
  
  xor    r11, r11
  mov    rax, [rip+p128] 
  mov    r8, [reg_p2+8]
  mul    r8
  add    r9, rax           // mc1 
  adc    r10, rdx
  adc    r11, 0
  
  mov    rax, [rip+p128+8]
  mul    r8
  add    r10, rax          // mc2          
  adc    r11, rdx          // mc3

  add    rcx, [reg_p1]
  adc    r9, [reg_p1+8]
  adc    r10, [reg_p1+16]
  adc    r11, [reg_p1+24]
  mov    [reg_p2], r10
  mov    [reg_p2+8], r11
  ret


//***********************************************************************
//  128-bit multiprecision addition
//  Operation: c [reg_p3] = a [reg_p1] + b [reg_p2]
//*********************************************************************** 
.global mp_add128_asm
mp_add128_asm: 
  mov    r8, [reg_p1]
  mov    r9, [reg_p1+8]
  add    r8, [reg_p2] 
  adc    r9, [reg_p2+8]
  mov    [reg_p3], r8
  mov    [reg_p3+8], r9
  ret


//***************************************************************************
//  2x128-bit multiprecision subtraction/addition
//  Operation: c [reg_p3] = a [reg_p1] - b [reg_p2]. If c < 0, add p128*2^128
//*************************************************************************** 
.global mp_subadd128x2_asm
mp_subadd128x2_asm:
  xor    rax, rax
  mov    r8, [reg_p1]
  mov    r9, [reg_p1+8]
  mov    r10, [reg_p1+16]
  mov    r11, [reg_p1+24]
  sub    r8, [reg_p2] 
  sbb    r9, [reg_p2+8] 
  sbb    r10, [reg_p2+16] 
  sbb    r11, [reg_p2+24]
  mov    [reg_p3], r8
  mov    [reg_p3+8], r9
  sbb    rax, 0
  
  // Add p128 anded with the mask in rax 
  mov    r8, [rip+p128]
  mov    r9, [rip+p128+8]
  and    r8, rax
  and    r9, rax
  add    r10, r8
  adc    r11, r9  
  mov    [reg_p3+16], r10
  mov    [reg_p3+24], r11
  ret


//***********************************************************************
//  Double 2x128-bit multiprecision subtraction
//  Operation: c [reg_p3] = c [reg_p3] - a [reg_p1] - b [reg_p2]
//*********************************************************************** 
.global mp_dblsub128x2_asm
mp_dblsub128x2_asm:
  mov    r8, [reg_p3]
  mov    r9, [reg_p3+8]
  mov    r10, [reg_p3+16]
  mov    r11, [reg_p3+24]
  sub    r8, [reg_p1]
  sbb    r9, [reg_p1+8] 
  sbb    r10, [reg_p1+16] 
  sbb    r11, [reg_p1+24]
  sub    r8, [reg_p2]
  sbb    r9, [reg_p2+8] 
  sbb    r10, [reg_p2+16] 
  sbb    r11, [reg_p2+24]
  mov    [reg_p3], r8
  mov    [reg_p3+8], r9
  mov    [reg_p3+16], r10
  mov    [reg_p3+24], r11
  ret