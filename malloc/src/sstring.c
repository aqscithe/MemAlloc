#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sstring.h"

sstring_t   sstringNew(const char* str)
{
    sstring_t sstr = strdup(str);
    int strLength = strlen(sstr);
    int bufferSize = sizeof(int) + ((strLength + 1) * sizeof(char));
    sstring_t buffer = calloc(1, bufferSize);
    memcpy(buffer, &strLength, sizeof(int));
    memcpy(buffer + 4, sstr, strLength * sizeof(char));
    return buffer + 4;
}

unsigned int sstringLength(sstring_t sstr)
{
    int* lengthPtr = (int*) (sstr - 4);
    return *lengthPtr;
}

void sstringFree(sstring_t sstr)
{
    free(sstr - 4);
}