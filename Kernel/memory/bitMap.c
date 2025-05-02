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
    uint8_t *bitmap; // Bitmap para manejar las páginas
	void *allocation; // Comienzo de la zona de memoria útil para allocMemory
	MemoryInfoCDT * memoryInfo;
} MemoryManagerCDT;

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


MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, uint64_t totalSize) {
    firstAdress = managedMemory;
    MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;

	if (memoryManager == NULL) {
        return NULL;
    }

    initMemoryInfo(&memoryManager->memoryInfo);

    // Corrige el cálculo del bitmap
    memoryManager->bitmap = (uint8_t *) ((uintptr_t)memoryManager + sizeof(MemoryManagerCDT));
	
	memoryManager->allocation = memoryManager->bitmap + (memoryManager->memoryInfo->totalPages/ 8);

    memoryManager->memoryInfo->totalMemory = totalSize;
    memoryManager->memoryInfo->totalPages = (totalSize - sizeof(MemoryManagerCDT)) / PAGE_SIZE;

    for (int i = 0; i < memoryManager->memoryInfo->totalPages; i++) {
        memoryManager->bitmap[i] = FREE;
    }

    memoryManager->memoryInfo->usedPages = 0;

    return memoryManager;
}

void *allocMemory(MemoryManagerADT  memoryManager, const size_t memoryToAllocate) {

	uint8_t pagesNeeded = (memoryToAllocate / PAGE_SIZE) + 1; // +1 para redondear hacia arriba

	uint64_t freePages = 0;

	for (uint64_t i = 0; i < memoryManager->memoryInfo->totalPages ; i++) {
		if (memoryManager->bitmap[i] == FREE) {
			freePages++;
			if(freePages == pagesNeeded) {
				memoryManager->bitmap[i-pagesNeeded+1] = BORDER; // Marcar hasta donde se asigna la memoria
				for (uint64_t j = i; j > i - pagesNeeded; j--) {
					memoryManager->bitmap[j] = USED;
				}
				memoryManager->memoryInfo->usedPages += pagesNeeded;
				return (uint8_t *) memoryManager->allocation + (i - freePages + 1) * PAGE_SIZE;

			}
		}
		else{
			freePages = 0;
		}
	}

	return NULL;	
}



void freeMemory(MemoryManagerADT const restrict memoryManager, void *const memoryToFree) {
	
	uint64_t pageIndex = ((uint8_t *) memoryToFree - (uint8_t *) memoryManager->allocation) / PAGE_SIZE;

	if(memoryManager->bitmap[pageIndex] != BORDER) {
		return; // No se puede liberar memoria que no fue asignada
	}

	memoryManager->bitmap[pageIndex] = FREE; // Marcar la página como libre
	memoryManager->memoryInfo->usedPages--;

	for(uint64_t i = pageIndex + 1; memoryManager->bitmap[i] != BORDER; i++) {
		memoryManager->bitmap[i] = FREE; // Marcar las páginas como libres
		memoryManager->memoryInfo->usedPages--;
	}

	return; // Se liberó la memoria correctamente
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