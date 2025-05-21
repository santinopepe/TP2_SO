#include <stddef.h>
#include <stdint.h>
#include <MemoryManager.h>
#include <memoryInfo.h>
#include <lib.h>
#include <video.h>
#include <string.h>

#define FREE 0
#define USED 1

static void *firstAddress = NULL;

typedef struct MemoryManagerCDT {
    uint8_t *bitmap;
    void *allocation;
    MemoryInfoADT memoryInfo;
} MemoryManagerCDT;

static MemoryManagerADT getMemoryManagerInternal() {
    if (firstAddress == NULL) {
        print("Error: MemoryManager not initialized\n");
        return NULL;
    }
    return (MemoryManagerADT)firstAddress;
}


void createMemoryManager(void *startMem, uint64_t totalSize) {

    if ((uint64_t)startMem % 8 != 0) {
      return ;
    }

    if (totalSize < sizeof(MemoryManagerCDT) + sizeof(MemoryInfoCDT)) {
       return ;
    }

    firstAddress = startMem;
    

    MemoryManagerADT memoryManager = (MemoryManagerADT)startMem;
    memset(memoryManager, 0, sizeof(MemoryManagerCDT));

    memoryManager->memoryInfo = (MemoryInfoADT)(((uintptr_t)memoryManager + sizeof(MemoryManagerCDT) + 7) & ~7);
    initMemoryInfo(memoryManager->memoryInfo);

    uint64_t spaceForBitmapAndAllocation = totalSize - ((uintptr_t)memoryManager->memoryInfo + sizeof(MemoryInfoCDT) - (uintptr_t)startMem);
    uint64_t bitmapSize = (spaceForBitmapAndAllocation / PAGE_SIZE + 7) / 8; // 1 bit por página
    uint64_t numPages = bitmapSize * 8;

    if (numPages == 0) {
        return;
    }

    memoryManager->memoryInfo->totalPages = numPages;
    memoryManager->memoryInfo->pageSize = PAGE_SIZE;
    memoryManager->memoryInfo->totalMemory = totalSize;
    strcpy(memoryManager->memoryInfo->memoryType, "bitmap");

    memoryManager->bitmap = (uint8_t *)(((uintptr_t)memoryManager->memoryInfo + sizeof(MemoryInfoCDT) + 7) & ~7);
    memoryManager->allocation = (void *)(((uintptr_t)memoryManager->bitmap + bitmapSize + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

    uint64_t allocationAreaSize = numPages * PAGE_SIZE;
    memoryManager->memoryInfo->freePages = numPages;
    memoryManager->memoryInfo->usedPages = 0;
    memoryManager->memoryInfo->freeMemory = allocationAreaSize;
    memoryManager->memoryInfo->usedMemory = 0;

    memset(memoryManager->bitmap, 0, bitmapSize);
}

void *malloc(const size_t memoryToAllocate) {
    MemoryManagerADT memoryManager = getMemoryManagerInternal();
    if (memoryManager == NULL || memoryManager->memoryInfo == NULL || memoryManager->bitmap == NULL) {
        return NULL;
    }
    if (memoryToAllocate == 0) {
        return NULL;
    }

    uint64_t pagesNeeded = (memoryToAllocate + PAGE_SIZE - 1) / PAGE_SIZE;
    if (pagesNeeded > memoryManager->memoryInfo->freePages) {
        return NULL;
    }

    uint64_t consecutiveFreePages = 0;
    uint64_t bestFitIndex = -1;

    for (uint64_t i = 0; i < memoryManager->memoryInfo->totalPages; i++) {
        if (memoryManager->bitmap[i / 8] & (1 << (i % 8))) {
            consecutiveFreePages = 0;
        } else {
            consecutiveFreePages++;
            if (consecutiveFreePages == pagesNeeded) {
                bestFitIndex = i - pagesNeeded + 1;
                break;
            }
        }
    }

    if (bestFitIndex == (uint64_t)-1) {
        return NULL;
    }

    for (uint64_t j = 0; j < pagesNeeded; j++) {
        memoryManager->bitmap[(bestFitIndex + j) / 8] |= (1 << ((bestFitIndex + j) % 8));
    }

    memoryManager->memoryInfo->usedPages += pagesNeeded;
    memoryManager->memoryInfo->freePages -= pagesNeeded;
    memoryManager->memoryInfo->usedMemory += pagesNeeded * PAGE_SIZE;
    memoryManager->memoryInfo->freeMemory -= pagesNeeded * PAGE_SIZE;

    void *blockAddress = (uint8_t *)memoryManager->allocation + bestFitIndex * PAGE_SIZE;
    return blockAddress;
}

void free(void *const memoryToFree) {
    MemoryManagerADT memoryManager = getMemoryManagerInternal();
    if (memoryToFree == NULL || memoryManager == NULL || memoryManager->allocation == NULL ||
        memoryManager->bitmap == NULL || memoryManager->memoryInfo == NULL) {
        return;
    }

    if ((uintptr_t)memoryToFree < (uintptr_t)memoryManager->allocation ||
        (uintptr_t)memoryToFree >= (uintptr_t)memoryManager->allocation + memoryManager->memoryInfo->totalPages * PAGE_SIZE) {
        return;
    }

    uint64_t pageIndex = ((uintptr_t)memoryToFree - (uintptr_t)memoryManager->allocation) / PAGE_SIZE;
    if (pageIndex >= memoryManager->memoryInfo->totalPages) {
        return;
    }

    if (!(memoryManager->bitmap[pageIndex / 8] & (1 << (pageIndex % 8)))) {
        return;
    }

    uint64_t pagesFreed = 0;
    for (uint64_t i = pageIndex; i < memoryManager->memoryInfo->totalPages; i++) {
        if (!(memoryManager->bitmap[i / 8] & (1 << (i % 8)))) {
            break;
        }
        memoryManager->bitmap[i / 8] &= ~(1 << (i % 8));
        pagesFreed++;
    }

    memoryManager->memoryInfo->usedPages -= pagesFreed;
    memoryManager->memoryInfo->freePages += pagesFreed;
    memoryManager->memoryInfo->usedMemory -= pagesFreed * PAGE_SIZE;
    memoryManager->memoryInfo->freeMemory += pagesFreed * PAGE_SIZE;

   
}

MemoryInfoADT getMemoryInfo(){
    MemoryManagerADT MemoryManager = getMemoryManagerInternal();
    return MemoryManager->memoryInfo;
}