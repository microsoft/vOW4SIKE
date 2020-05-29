// Public domain AES implementation using AES-NI instructions
//
// This version includes AES128 and AES256, and only implements encryption.

#include <stdint.h>
#include <wmmintrin.h>
#include "../config.h"


static __m128i key_expand(__m128i key, __m128i keygened) {
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3, 3, 3, 3));
	return _mm_xor_si128(key, keygened);
}


static __m128i key_expand1(__m128i key, __m128i keygened) {
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(2, 2, 2, 2));
	return _mm_xor_si128(key, keygened);
}


#define key_exp(k, rcon) key_expand(k, _mm_aeskeygenassist_si128(k, rcon))
#define key_exp256(k, r, rcon) key_expand(k, _mm_aeskeygenassist_si128(r, rcon))
#define key_exp256b(k, r, rcon) key_expand1(k, _mm_aeskeygenassist_si128(r, rcon))

void aes128_load_schedule_ni(const uint8_t *key, uint8_t *_schedule) {
	__m128i *schedule = (__m128i *) _schedule;
	schedule[0] = _mm_loadu_si128((const __m128i *) key);
	schedule[1] = key_exp(schedule[0], 0x01);
	schedule[2] = key_exp(schedule[1], 0x02);
	schedule[3] = key_exp(schedule[2], 0x04);
	schedule[4] = key_exp(schedule[3], 0x08);
	schedule[5] = key_exp(schedule[4], 0x10);
	schedule[6] = key_exp(schedule[5], 0x20);
	schedule[7] = key_exp(schedule[6], 0x40);
	schedule[8] = key_exp(schedule[7], 0x80);
	schedule[9] = key_exp(schedule[8], 0x1b);
	schedule[10] = key_exp(schedule[9], 0x36);
}


void aes128_enc_ni(const uint8_t *plaintext, const uint8_t *_schedule, uint8_t *ciphertext) {
	__m128i *schedule = (__m128i *) _schedule;
	__m128i m = _mm_loadu_si128((__m128i *) plaintext);
	size_t i;

	m = _mm_xor_si128(m, schedule[0]);
	for (i = 1; i < 10; i++) {
		m = _mm_aesenc_si128(m, schedule[i]);
	}
	m = _mm_aesenclast_si128(m, schedule[10]);

	_mm_storeu_si128((__m128i *) ciphertext, m);
}
