#include <string.h>
#include <stdlib.h>

#include "lstring.h"

unsigned int    getStrLen(const char* str)
{
    unsigned int size = 0;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        ++size;
    }
    return size;
}

lstring_t   lstringNew(const char* str)
{
    lstring_t lstr;
    lstr.length = getStrLen(str);
    lstr.buffer = calloc(lstr.length + 1, sizeof(char));
    memcpy(lstr.buffer, str, lstr.length * sizeof(char));
    return lstr;
}

lstring_t   lstringCat(lstring_t dest, lstring_t lstr)
{
    dest.buffer = realloc(dest.buffer, (dest.length + lstr.length + 1) * sizeof(char));
    memcpy(dest.buffer + dest.length, lstr.buffer, (lstr.length + 1) * sizeof(char));
    dest.length += lstr.length;
    return dest;
}

void   lstringFree(lstring_t lstr)
{
    free(lstr.buffer);
}