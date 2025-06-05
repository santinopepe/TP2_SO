#include <stddef.h>
#include <stdint.h>
#include <MemoryManager.h>
#include <memoryInfo.h>
#include <lib.h>
#include <video.h>
#include <string.h>

#define FREE 0
#define USED 1
#define BORDER 2
#define BLOCK_SIZE 64 // 64 bytes per block

static void *firstAddress;

typedef struct MemoryManagerCDT {
	uint8_t *bitmap;
	void *arena;
	uint64_t blocksQty;
	uint64_t usedBlocks;
	char memoryType[32]; 
} MemoryManagerCDT;

static MemoryManagerADT getMemoryManager();

void createMemoryManager(void *start, uint64_t size) {
	firstAddress = start;

	MemoryManagerADT memoryManager = (MemoryManagerADT) firstAddress;

	uint64_t sizeMM = sizeof(MemoryManagerCDT);

	memoryManager->blocksQty = (size - sizeMM) / BLOCK_SIZE;

	memoryManager->bitmap = (uint8_t *) (firstAddress + sizeMM);

	memoryManager->arena = memoryManager->bitmap + (memoryManager->blocksQty / 8);

	strcpy(memoryManager->memoryType, "Bitmap");	

	for (uint64_t i = 0; i < memoryManager->blocksQty; i++) {
		memoryManager->bitmap[i] = FREE;
	}


	memoryManager->usedBlocks = 0;
}

void *malloc(uint64_t size) {
	MemoryManagerADT memoryManager = getMemoryManager();

	if (size == 0) {
		return NULL;
	}

	uint64_t blocksNeeded = size / BLOCK_SIZE + ((size % BLOCK_SIZE) ? 1 : 0);

	uint64_t freeBlocks = 0;

	for (uint64_t i = 0; i < memoryManager->blocksQty; i++) {
		if (memoryManager->bitmap[i] == FREE) {
			freeBlocks++;
			if (freeBlocks == blocksNeeded) {
				memoryManager->bitmap[i - freeBlocks + 1] = BORDER;

				for (uint64_t j = i - freeBlocks + 2; j <= i; j++) {
					memoryManager->bitmap[j] = USED;
				}

				memoryManager->usedBlocks += blocksNeeded;

				


				return (uint8_t *) memoryManager->arena + (i - freeBlocks + 1) * BLOCK_SIZE;
			}
		}
		else {
			freeBlocks = 0;
		}
	}

	return NULL;
}

void free(void *p) {
	MemoryManagerADT memoryManager = getMemoryManager();

	if (p == NULL) {
		return;
	}

	uint64_t blockIndex = ((uint8_t *) p - (uint8_t *) memoryManager->arena) / BLOCK_SIZE;

	if (memoryManager->bitmap[blockIndex] != BORDER) {
		return;
	}

	memoryManager->bitmap[blockIndex] = FREE;
	memoryManager->usedBlocks--;

	for (uint64_t i = blockIndex + 1; i < memoryManager->blocksQty && memoryManager->bitmap[i] == USED; i++) {
		memoryManager->bitmap[i] = FREE;
		memoryManager->usedBlocks--;
	}

	

}

static MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) firstAddress;
}


char * getMemoryType(){
	MemoryManagerADT memoryManager = getMemoryManager();
	return memoryManager->memoryType;
}

int getUsedMemory(){
	MemoryManagerADT memoryManager = getMemoryManager();
	return memoryManager->usedBlocks * BLOCK_SIZE;	
}


int getFreeMemory(){
	MemoryManagerADT memoryManager = getMemoryManager();
	return (memoryManager->blocksQty - memoryManager->usedBlocks) * BLOCK_SIZE;	
}