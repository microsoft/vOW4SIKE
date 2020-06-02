#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void *bufcpy(void *dest, const void *src, size_t n);
uint_fast8_t bufcmp(const void *s1, const void *s2, size_t n);

#ifdef __cplusplus
}
#endif