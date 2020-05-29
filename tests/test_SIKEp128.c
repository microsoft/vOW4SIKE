/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: benchmarking/testing isogeny-based key encapsulation mechanism SIKEp128
*********************************************************************************************/ 

#include <stdio.h>
#include <string.h>
#include "test_extras.h"
#include "../src/P128/P128_api.h"


#define SCHEME_NAME    "SIKEp128"

#define crypto_kem_keypair            crypto_kem_keypair_SIKEp128
#define crypto_kem_enc                crypto_kem_enc_SIKEp128
#define crypto_kem_dec                crypto_kem_dec_SIKEp128

#include "test_sike.c"