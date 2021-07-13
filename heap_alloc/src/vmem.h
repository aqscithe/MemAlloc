#pragma once

#include <stdint.h>

void* vmemInit(uint32_t* pageSizeOut);
void vmemShutdown(void* baseAddress);
void vmemCommit(void* address, size_t size);
void vmemDecommit(void* address, size_t size);
