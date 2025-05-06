#include <memoryInfo.h>
#include <MemoryManager.h>
#include <stdint.h>
#include <string.h>

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
