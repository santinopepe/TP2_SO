#include <memoryInfo.h>
#include <MemoryManager.h>
#include <stdint.h>
#include <string.h>
#include <globals.h>


static struct MemoryInfoCDT staticMemoryInfo;
MemoryInfoADT memoryInfo = &staticMemoryInfo;

void createMemoryInfo() {
    memoryInfo = malloc(sizeof(MemoryInfoCDT));
    if (memoryInfo == NULL) {
        return; 
    }
    memoryInfo->totalMemory = 0;
    memoryInfo->freeMemory = 0;
    memoryInfo->usedMemory = 0;
    memoryInfo->totalPages = 0;
    memoryInfo->freePages = 0;
    memoryInfo->usedPages = 0;
    memoryInfo->pageSize = 0;
    memoryInfo->memoryType[0] = '\0'; 

}



MemoryInfoADT getMemoryInfo() {
   return memoryInfo ? memoryInfo : NULL;
}

void addTotalPages(uint64_t value) {
    if (memoryInfo) memoryInfo->totalPages += value;
}

void addTotalMemory(uint64_t value) {
    if (memoryInfo) memoryInfo->totalMemory += value;
}

void addFreePages(uint64_t value) {
    if (memoryInfo) memoryInfo->freePages += value;
}

void addUsedPages(uint64_t value) {
    if (memoryInfo) memoryInfo->usedPages += value;
}

void addFreeMemory(uint64_t value) {
    if (memoryInfo) memoryInfo->freeMemory -= value;
}

void addUsedMemory(uint64_t value) {
    if (memoryInfo) memoryInfo->usedMemory += value;
}

void setMemoryType(const char *type) {
    if (memoryInfo && type) strcpy(memoryInfo->memoryType, type);
}

void setTotalMemory(uint64_t value) {
    if (memoryInfo) memoryInfo->totalMemory = value;
}

void setPageSize(uint64_t value) {
    if (memoryInfo) memoryInfo->pageSize = value;
}

void setTotalPages(uint64_t value) {
    if (memoryInfo) memoryInfo->totalPages = value;
}
void setFreeMemory(uint64_t value) {
    if (memoryInfo) memoryInfo->freeMemory = value;
}

void setFreePages(uint64_t value) {
    if (memoryInfo) memoryInfo->freePages = value;
}