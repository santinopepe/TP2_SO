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




void initMemoryInfo(MemoryInfoADT memoryInfo) {
    if (memoryInfo) {
        memset(memoryInfo, 0, sizeof(MemoryInfoCDT)); // Pone todo a cero
        // Puedes poner valores por defecto si quieres
        // strcpy(info->memoryType, "SimpleMM"); // Necesitas <string.h> o equivalente en kernel
    }
}

MemoryInfoADT createMemoryInfoCopy(MemoryInfoADT  memoryInfo) {
    MemoryInfoADT copy = (MemoryInfoADT) malloc(sizeof(MemoryInfoCDT));
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
   // strcpy(copy->memoryType, memoryInfo->memoryType);

    return copy;
}
