#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdint.h>

typedef struct MemoryInfoCDT{
    uint64_t totalMemory;
    uint64_t freeMemory;
    uint64_t usedMemory;
    uint64_t totalPages;
    uint64_t freePages;
    uint64_t usedPages;
    uint64_t pageSize;
    char memoryType[16];
} MemoryInfoCDT;

#endif