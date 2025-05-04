#include <MemoryManager.h>
#include <memoryInfo.h> 
#include <stdint.h>
#include <lib.h> 

#define MIN_ORDER 4 // 16 bytes
#define MAX_BLOCKS 64 // 2^6 = 64 blocks

typedef struct MemoryBlock{
    uint8_t state; // 0 = free, 1 = allocated
    MemoryBlock *next; //Puntero al proximo bloque
    MemoryBlock *prev; //Puntero al bloque anterior
    uint8_t order; //Orden del bloque
} MemoryBlock;

typedef struct MemoryManagerCDT {
    MemoryInfoADT  memoryInfo;
    MemoryBlock * memoryBlockMap[MAX_BLOCKS]; //ver el tema del maximo de bloques
    uint8_t maxExp; //Maximo exponente de la lista de bloques
} MemoryManagerCDT; 
 

MemoryManagerADT createMemoryManager(void * startMem, uint64_t totalSize){
    if (totalSize < sizeof(MemoryManagerCDT) + sizeof(MemoryInfoCDT)) {
        return NULL;
    }

    MemoryManagerADT memoryManager = (MemoryManagerADT) startMem;

    memoryManager->memoryInfo = (MemoryInfoADT)((uintptr_t)memoryManager + sizeof(MemoryManagerCDT));

    initMemoryInfo(memoryManager->memoryInfo);

    MemoryBlock * firstMemoryBlock = (MemoryBlock *)((uintptr_t)memoryManager->memoryInfo + sizeof(MemoryInfoCDT));

    memoryManager->memoryBlockMap = firstMemoryBlock;
    memoryManager->memoryBlockMap->state = FREE; 
    memoryManager->memoryBlockMap->next = NULL; 
    memoryManager->memoryBlockMap->prev = NULL; 
    memoryManager->memoryBlockMap->order = 0; 
  
}

void * malloc(const size_t memoryToAllocate){

    if(memoryToAllocate == 0) return NULL;

    MemoryManagerADT memoryManager = getMemoryManager();

    uint8_t idx = log(memoryToAllocate - sizeof(MemoryBlock), 2);


}

void free(void * memoryToFree); 

void getMemoryInfo(MemoryInfoADT * mem); 

void * merge(void *allocatedMemory){

}

void * split(MemoryManagerADT memoryManager, uint8_t idx){
    MemoryBlock *block = memoryManager->memoryBlockMap[idx];
    removeMemoryBlock(memoryManager->memoryBlockMap, block);

    MemoryBlock *buddyBlock = (MemoryBlock *) ((void *) block + (1L << idx));
    
    createMemoryBlock((void *) buddyBlock, idx, memoryManager->memoryBlockMap[idx - 1]);  

    createMemoryBlock((void *) buddyBlock, idx, memoryManager->memoryBlockMap[idx - 1]);
	memoryManager->memoryBlockMap[idx - 1] = createMemoryBlock((void *) block, idx, buddyBlock);

	MemoryInfoCDT *memoryInfo = &(memoryManager->memoryInfo);
	memoryInfo->freePages++;
	memoryInfo->totalPages++;
}

void * removeMemoryBlock(MemoryBlock **memoryBlockMap, MemoryBlock *block){
    if(block->prev != NULL){
        block->prev->next = block->next;
    }
    else{
        memoryBlockMap[block->order-1] = block->next;
    }
    if(block->next != NULL){
        block->next->prev = block->prev;
    }
    return block->next;
}

void * createMemoryBlock(void *memoryToAllocate, uint8_t order, MemoryBlock *nextBlock){
    MemoryBlock *block = (MemoryBlock *)memoryToAllocate;
    block->state = FREE;
    block->prev = NULL;
    block->order = order;
    block->next = nextBlock;
    if(nextBlock != NULL){
        nextBlock->prev = block;
    }   
    return block;

}