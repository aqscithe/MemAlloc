
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>

#include "vmem.h"

void* vmemInit(uint32_t* pageSizeOut)
{
    SYSTEM_INFO sysInfo = {};
    GetSystemInfo(&sysInfo);
    uint32_t pageSize = sysInfo.dwPageSize;

    void* baseAddress = VirtualAlloc(NULL, 32ULL * 1024 * 1024 * 1024, MEM_RESERVE, PAGE_READWRITE);

    if (pageSizeOut)
        *pageSizeOut = pageSize;
    
    return baseAddress;
}

void vmemShutdown(void* baseAddress)
{
    VirtualFree(baseAddress, 0, MEM_RELEASE);
}

void vmemCommit(void* address, size_t size)
{
    VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
}

void vmemDecommit(void* address, size_t size)
{
    VirtualFree(address, size, MEM_DECOMMIT);
}