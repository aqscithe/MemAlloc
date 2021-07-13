#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "vmem.h"
#include "heap.h"
#include "heap_impl.h"

heap_t*  heapCreate(void)
{
    uint32_t    pageSize;
    void* baseAddress = vmemInit(&pageSize);
    vmemCommit(baseAddress, sizeof(heap_t));
    heap_t* heap = baseAddress;
    heap->pageSize = pageSize;
    heap->firstBlock = NULL;
    heap->lastBlock = NULL;
    return heap;
}

void     heapDestroy(heap_t* heap)
{
    vmemShutdown(heap->firstBlock);
}

block_t* getNextBlock(heap_t* heap, block_t* block, bool passLastBlock)
{
    if(!passLastBlock && block == heap->lastBlock)
        return NULL;
    return (block_t*)(block + 1 + block->size);
}

block_t* getPrevBlock(heap_t* heap, block_t* block)
{
    block_t*    loopBlock = heap->firstBlock;
    block_t*    tempBlock = NULL;

    if (heap->firstBlock == block)
        return NULL;
    while(loopBlock != block)
    {
        tempBlock = loopBlock;
        loopBlock = getNextBlock(heap, loopBlock, false);
    }
    return tempBlock;
}

// returns either next free block w/ enough space or NULL
block_t* findFreeBlock(heap_t* heap, size_t bytes)
{
    block_t*    block = (block_t*) (heap + 1);

    while(true)
    {
        if (block->blockFree && block->size > (bytes + sizeof(block_t))) 
            return block;
        if(block == heap->lastBlock)
            return NULL;
        block = getNextBlock(heap, block, false); 
    }
    return block;
}

void     updateHeap(heap_t* heap, block_t* block)
{
    if (heap->firstBlock == NULL)
        heap->firstBlock = block;
    if (heap->lastBlock == NULL)             
        heap->lastBlock = block;        
    else if (getNextBlock(heap, heap->lastBlock, true) == block)   
        heap->lastBlock = block;
}

void     setNewBlock(block_t* block, size_t bytes)
{
    block->size = bytes;
    block->blockFree = false;
}

void     addBlock(heap_t* heap, block_t* block, size_t bytes)
{
    vmemCommit(block, bytes + sizeof(block_t));
    setNewBlock(block, bytes);
    updateHeap(heap, block);
}

block_t* splitBlock(heap_t* heap, block_t* block, size_t bytes)
{
    block_t*    displacedBlock = NULL;
    block_t*    newBlock = NULL;
    size_t      oldBlockSize = block->size;
    size_t      calculatedSize = 0;

    newBlock = block;
    setNewBlock(newBlock, bytes);
    updateHeap(heap, newBlock);
    displacedBlock = getNextBlock(heap, newBlock, true);
    if (heap->lastBlock == newBlock)
        heap->lastBlock = displacedBlock;
    
    // the line I commented below only works if I don't use heapFree()
    // not sure why the 1 works
    calculatedSize = (size_t)(oldBlockSize - (newBlock->size + 1));
    //calculatedSize = (size_t)(oldBlockSize - (newBlock->size + sizeof(block_t))); 

    // sometimes if i don't do a vmemCommit i get a seg fault
    addBlock(heap, displacedBlock, calculatedSize);
    displacedBlock->blockFree = true;
    return displacedBlock;
}

block_t* getBlockFromDataPtr(void* dataPtr)
{
    return (block_t*)(dataPtr - sizeof(block_t));
}

void*    heapAlloc(heap_t* heap, size_t bytes)
{
    block_t*    block = NULL;

    if (heap->firstBlock != NULL)
    {
        block = findFreeBlock(heap, bytes);
        if (block != NULL)
        {
            block = splitBlock(heap, block, bytes);  
        }
        else
        {
            block = getNextBlock(heap, heap->lastBlock, true);
            addBlock(heap, block, bytes);
        }
    }  
    else
    {
        block = (block_t*) (heap + 1);
        addBlock(heap, block, bytes);
    }

    return (block_t*)(block + 1);
}

void*    heapCalloc(heap_t* heap, size_t num, size_t size)
{
    block_t*    block = NULL;
    void*       data = NULL;
    size_t bytes = num * size;

    if (heap->firstBlock != NULL)
    {
        block = findFreeBlock(heap, bytes);
        if (block != NULL)
            block->blockFree = false;
        else
        {
            block = getNextBlock(heap, heap->lastBlock, true);
            addBlock(heap, block, bytes);
        }
    }  
    else
    {
        block = (block_t*) (heap + 1);
        addBlock(heap, block, bytes);
    }
    data = (void*) (block + 1);
    memset(data, 0, bytes);

    return (block_t*)(block + 1);
}

block_t* growBlock(heap_t* heap, block_t* block, size_t size)
{
    block_t*    prevBlock = getPrevBlock(heap, block);
    block_t*    nextBlock = getNextBlock(heap, block, false);
    block_t*    nonAdjBlock = checkFreeNonAdjBlocks(heap, block, size);

    if (nextBlock != NULL && nextBlock->blockFree && (nextBlock->size + block->size + sizeof(block_t)) >= size)
    {
        fuseWithNextBlock(heap, block, nextBlock);
        block->blockFree = false;
        return block;
    }
    else if (prevBlock != NULL && prevBlock->blockFree && (prevBlock->size + block->size + sizeof(block_t)) >= size)
    {
        fuseWithPrevBlock(heap, block, prevBlock);
        prevBlock->blockFree = false;
        return prevBlock;
    }
    else if(nonAdjBlock != NULL)
    {
        fuseWithNonAdjBlock(heap, block, nonAdjBlock);
    }
    // else
    // {
    //     block_t* newBlock = 
    //     addBlock(heap, )
    // }
    return NULL;
    
}

void*    heapRealloc(heap_t* heap, void* ptr, size_t size)
{
    block_t*    block = getBlockFromDataPtr(ptr);
    if (ptr == NULL)
        return heapAlloc(heap, size);
    if (size == 0)
    {
        heapFree(heap, ptr);
        return NULL;
    }
    if (block->size > size)
        return NULL; //return shrinkBlock(heap, block, size);
    if (block->size < size)
        return growBlock(heap, block, size);
    return NULL;
}

block_t* checkFreeNonAdjBlocks(heap_t* heap, block_t* block, size_t size)
{
    block_t* loopBlock = heap->firstBlock;
    while (loopBlock != NULL && loopBlock->size > 0)
    {
        if (loopBlock == block)
        {
            loopBlock = getNextBlock(heap, block, false);
            continue;
        }
        if (loopBlock->blockFree && (loopBlock->size + block->size) >= (size + sizeof(block_t)))
        {
            return loopBlock;
        }
        loopBlock = getNextBlock(heap, loopBlock, false);
    }
    return loopBlock; 
}

void checkFreeAdjacentBlocks(heap_t* heap, block_t* block)
{
    block_t*    nextBlock = getNextBlock(heap, block, false);
    block_t*    prevBlock = getPrevBlock(heap, block);

    if (nextBlock != NULL && nextBlock->blockFree)
    {
        fuseWithNextBlock(heap, block, nextBlock);
    }
    if (prevBlock != NULL && prevBlock->blockFree)
    {
        fuseWithPrevBlock(heap, block, prevBlock);
    }
}

void fuseWithNonAdjBlock(heap_t* heap, block_t* block, block_t* nonAdjBlock)
{
    size_t      size = (size_t) (block->size + sizeof(block_t));
    size_t      nonAdjBlockSize = nonAdjBlock->size;

    memcpy(nonAdjBlock, block, size);
    if (heap->firstBlock == block)
        heap->firstBlock == getNextBlock(heap, block, false);
    if (heap->lastBlock == block)
        heap->lastBlock = getPrevBlock(heap, block);

    nonAdjBlock->size = nonAdjBlockSize;
    
    block_t*    loopBlock = block;
    block_t*    nextBlock = getNextBlock(heap, loopBlock, false);
    memcpy(loopBlock, nextBlock, nextBlock->size + sizeof(block_t));
    while(nextBlock != NULL && nextBlock->size > 0)
    {
        loopBlock = (block_t*)(loopBlock + 1 + loopBlock->size);
        nextBlock = getNextBlock(heap, nextBlock, false);
        if (nextBlock == NULL)
            break;
        memcpy(loopBlock, nextBlock, nextBlock->size + sizeof(block_t));
    }

    block_t*    lastBlock = heap->lastBlock;
    void*       toDecommit = (void*)(lastBlock + 1 + lastBlock->size);
    vmemDecommit(toDecommit, size);
}

void fuseWithNextBlock(heap_t* heap, block_t* block, block_t* nextBlock)
{
    //block->size += nextBlock->size + sizeof(block_t);
    block->size += nextBlock->size + 1;
   
    if (heap->lastBlock == nextBlock)
        heap->lastBlock = block;
}

void fuseWithPrevBlock(heap_t* heap, block_t* block, block_t* prevBlock)
{
    //prevBlock->size += block->size + sizeof(block_t);
    prevBlock->size += block->size + 1;
    if (heap->lastBlock == block)
        heap->lastBlock = prevBlock;
}

void heapFree(heap_t* heap, void* ptr)
{
    block_t*    block = getBlockFromDataPtr(ptr);
    block->blockFree = true;
    checkFreeAdjacentBlocks(heap, block);
}

void heapPrintDebug(heap_t* heap)
{
    block_t*    block = (block_t*) (heap + 1); 
    size_t      totalFree = 0;
    size_t      totalUsed = 0;  

    printf("Heap: ");
    printf("pageSize = %d | firstBlock @ 0x%p | lastBlock @ 0x%p \n\n", 
        heap->pageSize, heap->firstBlock, heap->lastBlock);
        
    if (heap->firstBlock != NULL)
    {
        int blockIndex = 0;
        while (block != NULL && block->size > 0)
        {
            if(block->blockFree)
                totalFree += block->size;
            else
                totalUsed += block->size;
            printf(" - Block %d ", blockIndex + 1);
            printf("@ 0x%p | free = %s | data @ 0x%p | size = %d\n", 
                block, block->blockFree ? "true" : "false", block + 1, block->size);
            block = getNextBlock(heap, block, false);
            ++blockIndex;
        }
        printf("\n\nTotal Free Space: %d | Total Used Space: %d", totalFree, totalUsed);
    }
    else
        printf("No blocks added...\n");    
}
