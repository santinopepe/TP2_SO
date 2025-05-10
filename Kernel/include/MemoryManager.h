#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <memoryInfo.h>
#include <stdint.h>

#define HEAP_SIZE 16 * 1024 * 1024 // 16MB
#define PAGE_SIZE 64
#define USED 1
#define FREE 0

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void * startMem, uint64_t totalSize); 

void * malloc(const size_t memoryToAllocate);

void free(void * memoryToFree); 

void getMemoryInfo(MemoryInfoADT * mem); 

#endif // MEMORY_MANAGER_H

