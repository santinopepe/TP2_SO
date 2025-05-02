#include <memoryInfo.h>
#include <MemoryManager.h>
/*
typedef struct MemoryManagerCDT {
	char *nextAddress;
	MemoryInfoADT *memoryInfo;

} MemoryManagerCDT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress = managedMemory;
	initMemoryInfo(memoryManager->memoryInfo);

	return memoryManager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {
	char *allocation = memoryManager->nextAddress;

	memoryManager->nextAddress += memoryToAllocate;

	return (void *) allocation;
}

void freeMemory(MemoryManagerADT const restrict memoryManager, void *const restrict memoryToFree) {
    memoryManager->memoryInfo;
}*/