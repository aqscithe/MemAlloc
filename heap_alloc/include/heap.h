#pragma once

typedef struct heap_t heap_t;

// Create the heap allocator
heap_t* heapCreate(void);
// Destroy the heap
void heapDestroy(heap_t* heap);

// Alloc memory
void* heapAlloc(heap_t* heap, size_t bytes);
// Free memory
void heapFree(heap_t* heap, void* ptr);