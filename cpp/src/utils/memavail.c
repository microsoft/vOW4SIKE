#include "memavail.h"
#include "../config.h"

// https://stackoverflow.com/a/2513561

#if (OS_TARGET == OS_LINUX)

// On UNIX-like operating systems, there is sysconf.
#include <unistd.h>

unsigned long long memavail()
{
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

#elif (OS_TARGET == OS_WIN)

// On Windows, there is GlobalMemoryStatusEx:
#include <windows.h>

unsigned long long memavail()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#endif
