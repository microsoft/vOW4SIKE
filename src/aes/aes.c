/********************************************************************************************
* AES functions 
*********************************************************************************************/

#include <assert.h>
#include <string.h>
#include "aes.h"
#include "aes_local.h"


void AES128_load_schedule(const uint8_t *key, uint8_t *schedule) {
#ifdef AES_ENABLE_NI
    aes128_load_schedule_ni(key, schedule);
#else
    aes128_load_schedule_c(key, schedule);
#endif
}


static inline void aes128_enc(const uint8_t *plaintext, const uint8_t *schedule, uint8_t *ciphertext) {
#ifdef AES_ENABLE_NI
    aes128_enc_ni(plaintext, schedule, ciphertext);
#else
    aes128_enc_c(plaintext, schedule, ciphertext);
#endif
}
     

void AES128_enc(const uint8_t* plaintext, const uint8_t* schedule, uint8_t* ciphertext, const size_t ciphertext_len) 
{
    unsigned int i, offset = 0;
    unsigned int nblocks = (unsigned int)ciphertext_len/16;
    unsigned char input[16] = {0};

    memcpy(input, plaintext, 8);
    
    for (i = 0; i < nblocks; i++) {
        aes128_enc(input, schedule, ciphertext + offset); 
        memcpy(input, ciphertext + offset, 16); 
        offset += 16;
        nblocks--;
    }
        
    if (offset < ciphertext_len) {
        aes128_enc(input, schedule, input);
        for (i = 0; i < (ciphertext_len - offset); i++) {
            ciphertext[offset+i] = input[i];
        }
    }
}


void AES128_free_schedule(uint8_t *schedule) {
    memset(schedule, 0, 16*11);
}
