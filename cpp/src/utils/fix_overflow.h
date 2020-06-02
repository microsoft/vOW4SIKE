#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

void fix_overflow(unsigned char *buf, const uint64_t len, const uint64_t nbits_overflow);

#ifdef __cplusplus
}
#endif
