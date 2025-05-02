#include <MemoryManager.h>
#include <stdint.h>
#include <memoryInfo.h>

#define PAGE_SIZE 4096 * sizeof(char) // 4KB
#define USED 1
#define FREE 0
#define BITS_PER_BYTE 8
#define BORDER 2 

static void *firstAdress;

typedef struct MemoryManagerCDT {
    uint8_t *bitmap;       // Bitmap para manejar las páginas
	MemoryInfoADT memoryInfo;
} MemoryManagerCDT;



MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, uint64_t totalSize) {
    firstAdress = managedMemory;
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	initMemoryInfo(&memoryManager->memoryInfo);

	memoryManager->bitmap = (uint8_t *) (memoryManager + sizeof(MemoryManagerCDT));

    memoryManager->memoryInfo->totalSize = totalSize;
    memoryManager->memoryInfo->totalPages = (totalSize-sizeof(MemoryManagerCDT)) / PAGE_SIZE;

	for(int i = 0; i < memoryManager->memoryInfo->totalPages; i++) {
		memoryManager->bitmap[i] = FREE;
	}

	memoryManager->memoryInfo->usedPages = 0;

    return memoryManager;
}

void *allocMemory(MemoryManagerADT  memoryManager, const size_t memoryToAllocate) {
	uint8_t *allocation; 

	uint8_t pagesNeeded = (memoryToAllocate / PAGE_SIZE) + 1; // +1 para redondear hacia arriba

	uint64_t freePages = 0;

	for (uint64_t i = 0; i < memoryManager->memoryInfo->totalPages ; i++) {
		if (memoryManager->bitmap[i] == FREE) {
			freePages++;
			if(freePages == pagesNeeded) {
				allocation = memoryManager->bitmap + (i - pagesNeeded + 1) * PAGE_SIZE;
				memoryManager->bitmap[i-pagesNeeded+1] = BORDER; // Marcar hasta donde se asigna la memoria
				for (uint64_t j = i; j > i - pagesNeeded; j--) {
					memoryManager->bitmap[j] = USED;
				}
				memoryManager->memoryInfo->usedPages += pagesNeeded;
				return (void *) allocation;
			}
		}
		else{
			freePages = 0;
		}
	}

	return NULL;	
}



void freeMemory(MemoryManagerADT const restrict memoryManager, void *const memoryToFree) {
	
    
}

/*
// Obtener el estado de un bit en el bitmap
static inline int getBit(uint8_t *bitmap, uint64_t index) {
    return (bitmap[index / BITS_PER_BYTE] >> (index % BITS_PER_BYTE)) & 1;
}

// Marcar un bit como ocupado (1)
static inline void setBit(uint8_t *bitmap, uint64_t index) {
    bitmap[index / BITS_PER_BYTE] |= (1 << (index % BITS_PER_BYTE));
}

// Marcar un bit como libre (0)
static inline void clearBit(uint8_t *bitmap, uint64_t index) {
    bitmap[index / BITS_PER_BYTE] &= ~(1 << (index % BITS_PER_BYTE));
}
*/