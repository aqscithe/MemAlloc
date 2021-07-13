#pragma once

#include <stdint.h>

void perfInit(void);
uint64_t perfQuery(void);
uint64_t perfFreq(void);
const char* perfGetDurationItStr(uint64_t diff, uint32_t iterations);
const char* perfGetDurationStr(uint64_t diff);
