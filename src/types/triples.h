#pragma once
#include "../config.h"

typedef union {
    unsigned char *bytes; /* Byte representation of size NBYTES_STATE */
    digit_t *words;       /* Word representation of size NWORDS_STATE */
} st_t;

typedef struct
{
    st_t current_state;
    uint64_t current_steps;
    st_t initial_state;
} trip_t;