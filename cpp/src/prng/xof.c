#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xof.h"
#include "../dependencies/aes/aes.h"
#include "../utils/buftools.h"

void print_n_buf(unsigned char *s, unsigned long n)
{
    unsigned long i;

    if (s == NULL)
    {
        for (i = 0; i < n; i++)
            printf("--");
        return;
    }
    for (i = 0; i < n; i++)
        printf("%02x", s[i]);
}


void _XOF(unsigned char *output, unsigned char *input, unsigned long nbytes_output, unsigned long nbytes_input, unsigned long salt)
{
    /* Merkle-Damgard with IV = salt */
    unsigned char aes_key_schedule[16 * 11];
    unsigned char key[16] = {0}, inp[16] = {0};
    unsigned int i, nbytes_input_overflow = ((nbytes_input % 16 == 0 ? 16 : nbytes_input % 16));

    for (i = 0; i < 4; i++)
        key[i] = ((unsigned char *)&salt)[i]; // Set IV
    AES128_load_schedule(key, aes_key_schedule);

    for (i = 0; i < (nbytes_input - 1) / 16; i++)
    {
        AES128_enc(input + 16 * i, aes_key_schedule, inp, 16);
        AES128_load_schedule(inp, aes_key_schedule);
    }
    for (i = 0; i < nbytes_input_overflow; i++)
        inp[i] = input[i + 16 * ((nbytes_input - 1) / 16)]; // Last block
    for (i = nbytes_input_overflow; i < 16; i++)
        inp[i] = 0;
    AES128_enc(inp, aes_key_schedule, output, nbytes_output);
}

/* Wrapper to avoid shared input and output pointers */
void XOF(unsigned char *output, unsigned char *input, unsigned long nbytes_output, unsigned long nbytes_input, unsigned long salt)
{
    unsigned char *cinput = (unsigned char *)malloc(nbytes_input);
    bufcpy(cinput, input, nbytes_input);
    _XOF(output, cinput, nbytes_output, nbytes_input, salt);
	free(cinput);
}
