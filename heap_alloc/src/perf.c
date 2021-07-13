
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>

#include "perf.h"

uint64_t perfCounterFreq = 0;

void perfInit(void)
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&perfCounterFreq);
}

uint64_t perfQuery(void)
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

uint64_t perfFreq(void)
{
    return perfCounterFreq;
}

const char* perfGetDurationItStr(uint64_t diff, uint32_t iterations)
{
    if (diff == 0)
        return "0 us";

    const char* units[] = { "s", "ms", "us", "ns", "ps" };
    int nbUnits = sizeof(units) / sizeof(units[0]);

    int divisor = 0;
    while (diff / iterations < perfCounterFreq && divisor < nbUnits-1)
    {
        divisor++;
        diff *= 1000;
    }

    static char buffer[128];
    sprintf(buffer, "%.3f %s", diff / iterations / (double)perfCounterFreq, units[divisor]);

    return buffer;
}

const char* perfGetDurationStr(uint64_t diff)
{
    return perfGetDurationItStr(diff, 1);
}