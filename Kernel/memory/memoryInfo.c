#include <memoryInfo.h>
#include <MemoryManager.h>
#include <stdint.h>
#include <string.h>

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


void initializeMemoryInfo(MemoryInfoCDT *memoryInfo) {
    if (memoryInfo == NULL) {
        return; // Manejar el caso de puntero nulo
    }

    memoryInfo->totalMemory = 0;
    memoryInfo->freeMemory = 0;
    memoryInfo->usedMemory = 0;
    memoryInfo->totalPages = 0;
    memoryInfo->freePages = 0;
    memoryInfo->usedPages = 0;
    memoryInfo->pageSize = 0;
    strcpy(memoryInfo->memoryType, "Unknown"); // Valor predeterminado para el tipo de memoria
}

MemoryInfoADT createMemoryInfoCopy(MemoryInfoADT  memoryInfo, MemoryManagerADT memoryManager) {
    MemoryInfoADT copy = (MemoryInfoADT ) allocMemory(memoryManager ,sizeof(MemoryInfoCDT));
    if (copy == NULL) {
        return NULL; // Handle memory allocation failure
    }
    copy->totalMemory = memoryInfo->totalMemory;
    copy->freeMemory = memoryInfo->freeMemory;
    copy->usedMemory = memoryInfo->usedMemory;
    copy->totalPages = memoryInfo->totalPages;
    copy->freePages = memoryInfo->freePages;
    copy->usedPages = memoryInfo->usedPages;
    copy->pageSize = memoryInfo->pageSize;
    strcpy(copy->memoryType, memoryInfo->memoryType);

    return copy;
}
