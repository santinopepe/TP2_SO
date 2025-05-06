#include <MemoryManager.h>
#include <memoryInfo.h>
#include <stdint.h>
#include <lib.h>

#define FREE 0
#define ALLOCATED 1
#define MIN_ORDER 4
#define MAX_ORDERS 64

static void * firstAddress;

typedef struct MemoryBlock{
    uint8_t state;
    struct MemoryBlock *next;
    struct MemoryBlock *prev;
    uint8_t order;
} MemoryBlock;

typedef struct MemoryManagerCDT {
    MemoryInfoADT  memoryInfo;
    MemoryBlock * memoryBlockMap[MAX_ORDERS];
    uint8_t maxExp;
} MemoryManagerCDT;



static uint8_t log2_floor(uint64_t n);

static uint8_t get_required_order(size_t size, uint8_t max_order);

static MemoryBlock * removeMemoryBlock(MemoryBlock **memoryBlockMap, MemoryBlock *block);

static MemoryBlock *createMemoryBlock(void *memoryToAllocate, uint8_t order);

static MemoryManagerADT getMemoryManagerInternal();

static MemoryBlock * merge_blocks(MemoryManagerADT memoryManager, MemoryBlock *block);

static void split_block(MemoryManagerADT memoryManager, MemoryBlock *block_to_split);



static uint8_t log2_floor(uint64_t n) {
    if (n == 0){
        return 0;
    }

    uint8_t log_val = 0;
    while ((1 << (log_val + 1)) <= n) { 
        log_val++;
    }
    return log_val;
}

static uint8_t get_required_order(size_t size, uint8_t max_order) {
    size_t required_size = size + sizeof(MemoryBlock);

    uint8_t order = MIN_ORDER;

    size_t block_size = 1 << MIN_ORDER; // 2^MIN_ORDER 

    while (block_size < required_size && order < max_order) {
        order++;
        block_size <<= 1; // double the size
    }
    if (block_size < required_size){
        return -1;
    }

    return order;
}

static MemoryBlock * removeMemoryBlock(MemoryBlock **memoryBlockMap, MemoryBlock *block){
    if (block == NULL) {
        return NULL;
    }

    if (block->order >= MAX_ORDERS) {
        return NULL;
    }

    if(block->prev != NULL){
        block->prev->next = block->next; 
    } else {
        memoryBlockMap[block->order] = block->next;
    }

    if(block->next != NULL){
        block->next->prev = block->prev; 
    }

    block->prev = NULL;
    block->next = NULL;

    return block;
}

static MemoryBlock *createMemoryBlock(void *memoryToAllocate, uint8_t order){
    MemoryBlock *block = (MemoryBlock *)memoryToAllocate;
    block->state = FREE;
    block->order = order;
    block->prev = NULL;
    block->next = NULL;

    return block;
}

static MemoryManagerADT getMemoryManagerInternal() {
    return (MemoryManagerADT) firstAddress;
}

static MemoryBlock * merge_blocks(MemoryManagerADT memoryManager, MemoryBlock *block) {

    if (block == NULL || block->state == ALLOCATED || block->order >= memoryManager->maxExp) {
        return NULL;
    }

    uintptr_t block_addr = (uintptr_t)block;
    uintptr_t buddy_addr = block_addr ^ (1 << block->order); // 1 << block->order, calculates block size and finds buddy address through XOR
    MemoryBlock *buddyBlock = (MemoryBlock *)buddy_addr;

    if (buddyBlock->state == FREE && buddyBlock->order == block->order) {
        removeMemoryBlock(memoryManager->memoryBlockMap, block);
        removeMemoryBlock(memoryManager->memoryBlockMap, buddyBlock);

        MemoryBlock *merged_block = (block_addr < buddy_addr) ? block : buddyBlock; // We keep the block with the lower address
        merged_block->order++; //As we merge, we increase the order (As it has bigger size)
        merged_block->state = FREE;
        merged_block->prev = NULL;
        merged_block->next = NULL;

        return merged_block;
    }

    return NULL;
}

static void split_block(MemoryManagerADT memoryManager, MemoryBlock *block_to_split) {
    if (block_to_split == NULL || block_to_split->state == ALLOCATED || block_to_split->order <= MIN_ORDER) {
        return;
    }

    uint8_t new_order = block_to_split->order - 1; // 1 << new_order is the size of the new block (We reduce the size)
    uintptr_t block_addr = (uintptr_t)block_to_split;
    uintptr_t buddy_addr = block_addr ^ (1 << new_order); // 1 << new_order, calculates block size and finds buddy address through XOR
    MemoryBlock *buddyBlock = (MemoryBlock *)buddy_addr;

    removeMemoryBlock(memoryManager->memoryBlockMap, block_to_split); 

    block_to_split->state = FREE;
    block_to_split->order = new_order;

    createMemoryBlock(buddyBlock, new_order);

    block_to_split->next = memoryManager->memoryBlockMap[new_order];
    if (memoryManager->memoryBlockMap[new_order] != NULL) {
        memoryManager->memoryBlockMap[new_order]->prev = block_to_split;
    }
    memoryManager->memoryBlockMap[new_order] = block_to_split;
    block_to_split->prev = NULL;

    buddyBlock->next = memoryManager->memoryBlockMap[new_order];
    if (memoryManager->memoryBlockMap[new_order] != NULL) {
        memoryManager->memoryBlockMap[new_order]->prev = buddyBlock;
    }
    memoryManager->memoryBlockMap[new_order] = buddyBlock;
    buddyBlock->prev = NULL;
}

MemoryManagerADT createMemoryManager(void * startMem, uint64_t totalSize){
    if (totalSize < sizeof(MemoryManagerCDT) + sizeof(MemoryInfoCDT) + (1ULL << MIN_ORDER)) {
        return NULL;
    }

    MemoryManagerADT memoryManager = (MemoryManagerADT) startMem;
    firstAddress = memoryManager;

    memoryManager->memoryInfo = (MemoryInfoADT)((uintptr_t)memoryManager + sizeof(MemoryManagerCDT));
    initMemoryInfo(memoryManager->memoryInfo);

    uintptr_t blocks_start_addr = (uintptr_t)memoryManager->memoryInfo + sizeof(MemoryInfoCDT);
    uint64_t memory_for_blocks = totalSize - (blocks_start_addr - (uintptr_t)startMem);

    memoryManager->maxExp = log2_floor(memory_for_blocks); //Calculates the maximum order (and with this size of the blocks)

    if (memoryManager->maxExp >= MAX_ORDERS) {
         return NULL;
    }

    for(int i = 0; i < MAX_ORDERS; i++){
        memoryManager->memoryBlockMap[i] = NULL;
    }

    MemoryBlock * firstMemoryBlock = (MemoryBlock *)blocks_start_addr;
    createMemoryBlock(firstMemoryBlock, memoryManager->maxExp);
    memoryManager->memoryBlockMap[memoryManager->maxExp] = firstMemoryBlock;
    
    strcpy(memoryManager->memoryInfo->memoryType, "buddy"); 
    memoryManager->memoryInfo->totalMemory = totalSize;
    memoryManager->memoryInfo->freeMemory = (1 << memoryManager->maxExp);

    return memoryManager;
}

void * malloc(const size_t memoryToAllocate){
    if(memoryToAllocate == 0) {
        return NULL;
    }

    MemoryManagerADT memoryManager = getMemoryManagerInternal();
    
    if (memoryManager == NULL) {
        return NULL;
    }


    uint8_t required_order = get_required_order(memoryToAllocate, memoryManager->maxExp);

    if (required_order == -1 || required_order > memoryManager->maxExp) {
        return NULL;
    }

    uint8_t current_order = required_order;
    MemoryBlock *block_to_allocate = NULL;

    while (current_order <= memoryManager->maxExp) {
        if (memoryManager->memoryBlockMap[current_order] != NULL) {
            block_to_allocate = removeMemoryBlock(memoryManager->memoryBlockMap, memoryManager->memoryBlockMap[current_order]);
            break;
        }
        current_order++;
    }

    if (block_to_allocate == NULL) {
        return NULL;
    }

    while (block_to_allocate->order > required_order) {
        split_block(memoryManager, block_to_allocate);
    }

    block_to_allocate->state = ALLOCATED;
    memoryManager->memoryInfo->freeMemory -= (1 << block_to_allocate->order);

    return (void *)block_to_allocate + sizeof(MemoryBlock);
}

void free(void * memoryToFree){
    if (memoryToFree == NULL){ 
        return;
    }

    MemoryManagerADT memoryManager = getMemoryManagerInternal();
    if (memoryManager == NULL) {
        return;
    }

    MemoryBlock *block_to_free = (MemoryBlock *)((uintptr_t)memoryToFree - sizeof(MemoryBlock));

    block_to_free->state = FREE;
    memoryManager->memoryInfo->freeMemory += (1 << block_to_free->order);

    MemoryBlock *current_block = block_to_free;
    while (current_block->order < memoryManager->maxExp) {
        MemoryBlock *merged_block = merge_blocks(memoryManager, current_block);

        if (merged_block != NULL) {
            current_block = merged_block;
        } else {
            break;
        }
    }

    current_block->next = memoryManager->memoryBlockMap[current_block->order];
    if (memoryManager->memoryBlockMap[current_block->order] != NULL) {
        memoryManager->memoryBlockMap[current_block->order]->prev = current_block;
    }
    memoryManager->memoryBlockMap[current_block->order] = current_block;
    current_block->prev = NULL;
}