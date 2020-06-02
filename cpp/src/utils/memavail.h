#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if (OS_TARGET == OS_LINUX) || (OS_TARGET == OS_WIN)

unsigned long long memavail();

#endif

#ifdef __cplusplus
}
#endif