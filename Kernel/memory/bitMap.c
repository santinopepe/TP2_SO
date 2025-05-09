#include <MemoryManager.h>
#include <memoryInfo.h> 
#include <stdint.h>
#include <lib.h>


#define PAGE_SIZE 64
#define USED 1
#define FREE 0
#define BORDER 2

static void *firstAdress; 


typedef struct MemoryManagerCDT {
    uint8_t *bitmap;
    void *allocation;
    MemoryInfoADT memoryInfo; // Puntero a la estructura de información
} MemoryManagerCDT;

static MemoryManagerADT getMemoryManagerInternal() {
    return (MemoryManagerADT) firstAdress;
}



void strcpy(char dest[], const char source[])
{
    int i = 0;
    while (1)
    {
        dest[i] = source[i];

        if (dest[i] == '\0')
        {
            break;
        }

        i++;
    } 
	return; 
}

MemoryManagerADT createMemoryManager(void * startMem, uint64_t totalSize) {
    if (totalSize < sizeof(MemoryManagerCDT) + sizeof(MemoryInfoCDT)) {
        return NULL;
    }

    firstAdress = startMem;

    
    MemoryManagerADT memoryManager = (MemoryManagerADT) startMem;

    
    memoryManager->memoryInfo = (MemoryInfoADT)((uintptr_t)memoryManager + sizeof(MemoryManagerCDT));

    

    
    initMemoryInfo(memoryManager->memoryInfo);

   
    uint64_t spaceForBitmapAndAllocation = totalSize - sizeof(MemoryManagerCDT) - sizeof(MemoryInfoCDT);

    if (PAGE_SIZE + 1 == 0) return NULL; 
    uint64_t numPages = spaceForBitmapAndAllocation / (PAGE_SIZE + 1);

    if (numPages == 0) {
        return NULL;
    }

    
    memoryManager->memoryInfo->totalPages = numPages;
    memoryManager->memoryInfo->pageSize = PAGE_SIZE;
    memoryManager->memoryInfo->totalMemory = totalSize; 
    strcpy(memoryManager->memoryInfo->memoryType, "bitmap"); 
    memoryManager->bitmap = (uint8_t *)((uintptr_t)memoryManager->memoryInfo + sizeof(MemoryInfoCDT));

    
    uint64_t bitmapSize = memoryManager->memoryInfo->totalPages;

    
    memoryManager->allocation = (void *)((uintptr_t)memoryManager->bitmap + bitmapSize);

    
    uint64_t allocationAreaSize = memoryManager->memoryInfo->totalPages * PAGE_SIZE;

    memoryManager->memoryInfo->freePages = memoryManager->memoryInfo->totalPages;
    memoryManager->memoryInfo->usedPages = 0;
    memoryManager->memoryInfo->freeMemory = allocationAreaSize;
    memoryManager->memoryInfo->usedMemory = 0;

    memset(memoryManager->bitmap, FREE, memoryManager->memoryInfo->totalPages);

    return memoryManager;
}

void * malloc( const size_t memoryToAllocate) {
    MemoryManagerADT memoryManager = getMemoryManagerInternal();
    if (memoryManager == NULL || memoryManager->memoryInfo == NULL || memoryToAllocate == 0) {
        return NULL;
    }

    uint64_t pagesNeeded = (memoryToAllocate + PAGE_SIZE - 1) / PAGE_SIZE;

    if (pagesNeeded > memoryManager->memoryInfo->freePages) {
        return NULL; 
    }

    uint64_t consecutiveFreePages = 0;
    uint64_t bestFitIndex = -1; 

    for (uint64_t i = 0; i < memoryManager->memoryInfo->totalPages; i++) {
        if (memoryManager->bitmap[i] == FREE) {
            consecutiveFreePages++;
            if (consecutiveFreePages == pagesNeeded) {
                bestFitIndex = i - pagesNeeded + 1; 
                break; 
            }
        } else {
            consecutiveFreePages = 0; 
        }
    }

    if (bestFitIndex == (uint64_t)-1) {
        return NULL;
    }

    memoryManager->bitmap[bestFitIndex] = BORDER; 
    for (uint64_t j = 1; j < pagesNeeded; j++) {
        memoryManager->bitmap[bestFitIndex + j] = USED; 
    }

    memoryManager->memoryInfo->usedPages += pagesNeeded;
    memoryManager->memoryInfo->freePages -= pagesNeeded;

    memoryManager->memoryInfo->usedMemory += pagesNeeded * PAGE_SIZE;
    memoryManager->memoryInfo->freeMemory -= pagesNeeded * PAGE_SIZE;


    void* blockAddress = (uint8_t *) memoryManager->allocation + bestFitIndex * PAGE_SIZE;
    return blockAddress;
}

void free(void *const memoryToFree) {
    MemoryManagerADT memoryManager = getMemoryManagerInternal();

    if (memoryToFree == NULL || memoryManager == NULL || memoryManager->allocation == NULL || memoryManager->bitmap == NULL || memoryManager->memoryInfo == NULL) {
        return; 
    }

    if ((uintptr_t)memoryToFree < (uintptr_t)memoryManager->allocation ||
        (uintptr_t)memoryToFree >= (uintptr_t)memoryManager->allocation + memoryManager->memoryInfo->totalPages * PAGE_SIZE) {
        return; 
    }

    uint64_t pageIndex = ((uintptr_t)memoryToFree - (uintptr_t)memoryManager->allocation) / PAGE_SIZE;

    if (pageIndex >= memoryManager->memoryInfo->totalPages || memoryManager->bitmap[pageIndex] != BORDER) {
        return; 
    }

    
    memoryManager->bitmap[pageIndex] = FREE;
    uint64_t pagesFreed = 1;

    
    for (uint64_t i = pageIndex + 1; i < memoryManager->memoryInfo->totalPages && memoryManager->bitmap[i] == USED; i++) {
        memoryManager->bitmap[i] = FREE;
        pagesFreed++;
    }

    memoryManager->memoryInfo->usedPages -= pagesFreed;
    memoryManager->memoryInfo->freePages += pagesFreed;
    memoryManager->memoryInfo->usedMemory -= pagesFreed * PAGE_SIZE;
    memoryManager->memoryInfo->freeMemory += pagesFreed * PAGE_SIZE;

    return;
}