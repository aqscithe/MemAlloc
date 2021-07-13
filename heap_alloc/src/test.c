
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <windows.h>

#include "perf.h"
#include "vmem.h"

#include <heap.h>
#include "heap_impl.h"

int random_int(int min, int max)
{ 
    
    return min + rand() % (max+1 - min);
}

void testVmem(void)
{
    uint32_t pageSize;
    void* baseAddress = vmemInit(&pageSize);
    printf("baseAddress: 0x%p (pageSize=%dKB)\n", baseAddress, pageSize / 1024);

    // Allocate an array of int of 32 elements
    int intArraySize = 32;
    int* intArray = baseAddress;
    //intArray[0] = 1; // Will segfault since mem has not been commit

    vmemCommit(baseAddress, intArraySize * sizeof(int));
    intArray[0] = 1;
    intArray[intArraySize+1] = 1; // Won't segfault since the committed size is at least equals to pageSize (4096 octets)

    vmemDecommit(baseAddress, intArraySize * sizeof(int)); // The entire page will be decommitted (even if size < pageSize)
    //intArray[0] = 1; // Will segfault since mem has been decommitted

    vmemShutdown(baseAddress);
}

void testVmemPerfs(void)
{
    uint32_t pageSize;
    void* baseAddress = vmemInit(&pageSize);
    printf("baseAddress: 0x%p (pageSize=%dKB)\n", baseAddress, pageSize / 1024);

    vmemCommit(baseAddress, 32);

    char* test = baseAddress;

    {
        uint64_t start = perfQuery();
        test[0] = 0;
        printf("[0] = %s (first write after commit)\n", perfGetDurationStr(perfQuery() - start));
    }

    {
        uint64_t start = perfQuery();
        test[1] = 1;
        printf("[1] = %s (second write)\n", perfGetDurationStr(perfQuery() - start));
    }
    vmemDecommit(baseAddress, 0);
    vmemCommit(baseAddress, 32);

    {
        uint64_t start = perfQuery();
        test[2] = 2;
        printf("[2] = %s (first write after commit)\n", perfGetDurationStr(perfQuery() - start));
    }

    {
        uint64_t start = perfQuery();
        test[3] = 3;
        printf("[3] = %s (second write)\n", perfGetDurationStr(perfQuery() - start));
    }

    {
        uint64_t start = perfQuery();
        for (int i = 0; i < pageSize; ++i)
        {
            test[i] = i;
        }
        printf("[4] = %s / iterations, for %d write\n", perfGetDurationItStr((perfQuery() - start), pageSize), pageSize);
    }

    vmemShutdown(baseAddress);
}


void testHeapAlloc()
{
    srand(time(0));
    heap_t* heap = heapCreate();
    int min = 4;
    int max = 64;

    void* addressArray[10];
    for (int i = 0; i < 10; ++i)
    {
        if(i == 8)
        {
            heapFree(heap, addressArray[random_int(0, i - 1)]);
            heapFree(heap, addressArray[random_int(0, i - 1)]);
        }
        int size = random_int(min, max + 1);
        size_t byteSize = size * sizeof(int);
        addressArray[i] = heapAlloc(heap, byteSize);
        block_t* block = (block_t*)(addressArray[i] - sizeof(block_t));
    }

    //heapFree(heap, addressArray[5]);
    //heapFree(heap, addressArray[4]);

    heapPrintDebug(heap);

    heapDestroy(heap);
}

void testHeapRealloc()
{
    srand(time(0));

    void*   addressArray[6];
    heap_t* heap = heapCreate();
    int     size = 32;
    size_t  byteSize = size * sizeof(int);
    
    for (int i = 0; i < 5; ++i)
    {
        if (i  == 3)
            addressArray[i] = heapAlloc(heap, byteSize * 2);
        else
            addressArray[i] = heapAlloc(heap, byteSize);
        block_t* block = (block_t*)(addressArray[i] - sizeof(block_t));
    }

    // testing realloc case: null ptr 
    // void*   ptr = NULL;
    // byteSize = 15 * sizeof(int);
    // void*   array = heapRealloc(heap, ptr, byteSize);

    // testing realloc case: size = 0
    //heapRealloc(heap, addressArray[2], 0);

    // testing realloc case: size > block->size, free block after
    // byteSize = 36 * sizeof(int);
    // heapFree(heap, addressArray[3]);
    // heapRealloc(heap, addressArray[1], byteSize);

    // testing realloc case: size > block->size, free block before
    // byteSize = 36 * sizeof(int);
    // heapFree(heap, addressArray[2]);
    // heapRealloc(heap, addressArray[3], byteSize);

    // testing realloc case: size > block->size, free block non adjacent
    byteSize = 36 * sizeof(int);
    heapFree(heap, addressArray[3]);
    heapRealloc(heap, addressArray[1], byteSize);

    addressArray[6] = heapAlloc(heap, byteSize);

    heapPrintDebug(heap);

    heapDestroy(heap);
}

int main(int argc, char* argv[])
{
    //perfInit();

    //testVmem();

    //testVmemPerfs();

    //testHeapAlloc();

    testHeapRealloc();

    return 0;
}
