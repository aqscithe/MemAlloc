
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lstring.h"
#include "sstring.h"

void printBuffer(unsigned char* bytesArray, int length)
{
    for (int i = 0; i < length; ++i)
        printf("0x%02x | ", bytesArray[i]);
    printf("\n");
}

void test1()
{
    char* str = strdup("a string");
    printf("string : %s (length=%d)\n", str, (unsigned int)strlen(str));
    free(str);
}

void test2()
{
    lstring_t str = lstringNew("a string");
    printf("lstring : %s (length=%d)\n", str.buffer, str.length);
}

void test3()
{
    lstring_t str1 = lstringNew("a string");
    lstring_t str2 = lstringNew(" and an other string");
    str1 = lstringCat(str1, str2);
    printf("lstring : %s (length=%d)\n", str1.buffer, str1.length);
    lstringFree(str2);
    lstringFree(str1);
}

void test4()
{
    sstring_t str = sstringNew("a string");
    printf("sstring : %s (length=%d)\n", str, sstringLength(str));
    fflush(stdout);
    sstringFree(str);
}

void test5()
{
    int bufferSize = sizeof(int) + sizeof(char) + sizeof(int);
    void* buffer = calloc(1, bufferSize);

    // Print buffer (emtpy)
    unsigned char* bufferAsBytes = buffer;
    printBuffer(bufferAsBytes, bufferSize);

    // Modify first int in buffer
    int* firstInt = (int*)(bufferAsBytes + 0);
    *firstInt = 1500;
    printBuffer(bufferAsBytes, bufferSize);

    // Modify char
    char* ch = (char*)(bufferAsBytes + sizeof(int));
    *ch = 'a';
    printBuffer(bufferAsBytes, bufferSize);

    int* ptr = 0x4;
    printf("ptr = 0x%x\n", ptr);
    ptr = ptr + 1;
    printf("ptr = 0x%x\n", ptr);
}

int main(int argc, char* argv[])
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
