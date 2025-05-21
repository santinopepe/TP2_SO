#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <memoryInfo.h>
#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE 256 * 1024 * 1024 // 256MB
#define USED 1
#define PAGE_SIZE 64 // 4KB
#define FREE 0

typedef struct MemoryManagerCDT * MemoryManagerADT;

void createMemoryManager(void * startMem, uint64_t totalSize); 

void * malloc(const size_t memoryToAllocate);

void free(void * memoryToFree); 

MemoryInfoADT getMemoryInfo(); 

#endif // MEMORY_MANAGER_H