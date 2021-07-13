#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct block_t
{
    size_t  size;
    bool    blockFree;    
} block_t;

typedef struct heap_t
{
    size_t  pageSize;
    void*   firstBlock;
    void*   lastBlock;
} heap_t;

void heapPrintDebug(heap_t* heap);

block_t* findFreeBlock(heap_t* heap, size_t bytes);

void updateHeap(heap_t* heap, block_t* block);

void addBlock(heap_t* heap, block_t* block, size_t bytes);

void setNewBlock(block_t* block, size_t bytes);

block_t* splitBlock(heap_t* heap, block_t* block, size_t bytes);

block_t* getNextBlock(heap_t* heap, block_t* block, bool passLastBlock);

block_t* getPrevBlock(heap_t* heap, block_t* block);

block_t* getBlockFromDataPtr(void* dataPtr);

block_t* growBlock(heap_t* heap, block_t* block, size_t size);

block_t* checkFreeNonAdjBlocks(heap_t* heap, block_t* block, size_t size);

void checkFreeAdjacentBlocks(heap_t* heap, block_t* block);

void fuseWithNextBlock(heap_t* heap, block_t* block, block_t* nextBlock);

void fuseWithPrevBlock(heap_t* heap, block_t* block, block_t* prevBlock);

void fuseWithNonAdjBlock(heap_t* heap, block_t* block, block_t* nonAdjBlock);

void* heapRealloc(heap_t* heap, void*ptr, size_t size);

void* heapCalloc(heap_t* heap, size_t num, size_t size);