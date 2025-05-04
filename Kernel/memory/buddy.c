#include <MemoryManager.h>
#include <memoryInfo.h> 
#include <stdint.h>
#include <lib.h> 


typedef struct MemoryBlock{
    uint8_t state; // 0 = free, 1 = allocated
    struct MemoryBlock *next; //Puntero al proximo bloque
    struct MemoryBlock *prev; //Puntero al bloque anterior
    uint8_t orden; //Orden del bloque
} MemoryBlock;

typedef struct MemoryManagerCDT {
    MemoryInfoADT  memoryInfo;
    struct MemoryBlock * memoryBlockMap; //ver el tema del maximo de bloques
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
    memoryManager->memoryBlockMap->orden = 0; 

    

    
}

void * malloc(const size_t memoryToAllocate){

    if(memoryToAllocate == 0) return NULL;

    MemoryManagerADT memoryManager = getMemoryManager();

    
}

void free(void * memoryToFree); 

void getMemoryInfo(MemoryInfoADT * mem); 

void * merge(void *allocatedMemory){

}

void * split(void *allocatedMemory, uint8_t idx){
    
}