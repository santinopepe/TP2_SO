#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <memoryInfo.h>
#include <stdint.h>

#define HEAP_SIZE 0x10000000 // 256MB

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void * startMem, uint64_t totalSize); 

void * malloc(const size_t memoryToAllocate);

void free(void * memoryToFree); 

void getMemoryInfo(MemoryInfoADT * mem); 

#endif // MEMORY_MANAGER_H

