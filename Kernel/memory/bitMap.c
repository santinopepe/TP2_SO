#include <memoryManager.h>
#include <memoryInfo.h>

#define PAGE_SIZE 4096 * sizeof(char) // 4KB
#define USED 1
#define FREE 0
#define BITS_PER_BYTE 8

typedef struct MemoryManagerCDT {
    uint8_t *bitmap;       // Bitmap para manejar las páginas
    uint64_t totalPages;   // Total de páginas
    uint64_t totalSize;    // Tamaño total de la memoria
	uint64_t usedBytes;    // Bytes usados
	MemoryInfoADT memoryInfo;

} MemoryManagerCDT;



MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory, uint64_t totalSize) {
    MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	initMemoryInfo(&memoryManager->memoryInfo);

	memoryManager->bitmap = (uint8_t *) (memoryManager + sizeof(MemoryManagerCDT));

    memoryManager->totalSize = totalSize;
    memoryManager->totalPages = totalSize / PAGE_SIZE;

	for(int i = 0; i < memoryManager->totalPages; i++) {
		memoryManager->bitmap[i] = FREE;
	}

	memoryManager->usedBytes = 0;

    return memoryManager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {

	uint64_t totalPages = (memoryManager->totalSize / PAGE_SIZE) + 1;
	uint8_t *allocation; 
	Page * page = (Page *) memoryManager->firstPage;

	uint8_t * currentPage = memoryManager->firstPage;
	for (uint64_t i = 0; currentPage == NULL && i < totalPages; i++) {
		currentPage = page->nextPage;
	}


	char *allocation = memoryManager->nextAddress;
	memoryManager->nextAddress += memoryToAllocate;
	return (void *) allocation;
}



void freeMemory(MemoryManagerADT const restrict memoryManager, void *const memoryToFree) {
	
    
}

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