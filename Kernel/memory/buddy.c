#include <MemoryManager.h>
#include <memoryInfo.h>
#include <stdint.h>
#include <stddef.h>
#include <lib.h>

#define FREE 0
#define ALLOCATED 1
#define MIN_ORDER 4
#define MAX_ORDERS 64

static void *firstAddress;

typedef struct MemoryBlock
{
    uint8_t state;
    struct MemoryBlock *next;
    struct MemoryBlock *prev;
    uint8_t order;
} MemoryBlock;

typedef struct MemoryManagerCDT
{
    MemoryInfoADT memoryInfo;
    MemoryBlock *memoryBlockMap[MAX_ORDERS];
    uint8_t maxExp;
} MemoryManagerCDT;

static uint8_t log2_floor(uint64_t n);

static uint8_t get_required_order(size_t size, uint8_t max_order);

static MemoryBlock *removeMemoryBlock(MemoryBlock **memoryBlockMap, MemoryBlock *block);

static MemoryBlock *createMemoryBlock(void *memoryToAllocate, uint8_t order);

static MemoryManagerADT getMemoryManagerInternal();

static int is_block_in_heap(uintptr_t block_addr, uintptr_t heap_start, uint64_t heap_size);

static MemoryBlock *merge_blocks(MemoryManagerADT memoryManager, MemoryBlock *block);

static MemoryBlock *split_block(MemoryManagerADT memoryManager, MemoryBlock *block_to_split);

// ESTO habria que cambiarlo
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

static uint8_t log2_floor(uint64_t n)
{
    if (n == 0)
    {
        return 0;
    }

    uint8_t log_val = 0;
    while ((1ULL << (log_val + 1)) <= n)
    {
        log_val++;
    }
    return log_val;
}

static uint8_t get_required_order(size_t size, uint8_t max_order)
{
    size_t required_size = size + sizeof(MemoryBlock);

    uint8_t order = MIN_ORDER;

    size_t block_size = 1ULL << MIN_ORDER; // 2^MIN_ORDER

    while (block_size < required_size && order < max_order)
    {
        order++;
        block_size <<= 1; 
    }
    if (block_size < required_size)
    {
        return -1;
    }

    return order;
}

static MemoryBlock *removeMemoryBlock(MemoryBlock **memoryBlockMap, MemoryBlock *block)
{
    if (block == NULL)
    {
        return NULL;
    }

    if (block->order > MAX_ORDERS)
    {
        return NULL;
    }

    if (block->prev != NULL)
    {
        block->prev->next = block->next;
    }
    else
    {
        memoryBlockMap[block->order] = block->next;
    }

    if (block->next != NULL)
    {
        block->next->prev = block->prev;
    }

    block->prev = NULL;
    block->next = NULL;

    return block;
}

static MemoryBlock *createMemoryBlock(void *memoryToAllocate, uint8_t order)
{
    MemoryBlock *block = (MemoryBlock *)memoryToAllocate;
    block->state = FREE;
    block->order = order;
    block->prev = NULL;
    block->next = NULL;

    return block;
}

static MemoryManagerADT getMemoryManagerInternal()
{
    return (MemoryManagerADT)firstAddress;
}

static int is_block_in_heap(uintptr_t block_addr, uintptr_t heap_start, uint64_t heap_size)
{
    return block_addr >= heap_start && block_addr < (heap_start + heap_size);
}

static MemoryBlock *merge_blocks(MemoryManagerADT memoryManager, MemoryBlock *block)
{
    if (block == NULL || block->state == ALLOCATED || block->order >= memoryManager->maxExp)
    {
        return NULL;
    }

    uintptr_t block_addr = (uintptr_t)block;
    uintptr_t buddy_addr = block_addr ^ (1ULL << block->order);
    uintptr_t heap_start = (uintptr_t)memoryManager->memoryBlockMap[memoryManager->maxExp];
    uint64_t heap_size = 1ULL << memoryManager->maxExp;
    if (!is_block_in_heap(buddy_addr, heap_start, heap_size))
    {
        return NULL;
    }
    MemoryBlock *buddyBlock = (MemoryBlock *)buddy_addr;

    
    if (buddyBlock->state == FREE && buddyBlock->order == block->order)
    {
        // Determina cuál bloque va primero para el merge
        MemoryBlock *first_block = (block_addr < buddy_addr) ? block : buddyBlock;
        MemoryBlock *second_block = (block_addr < buddy_addr) ? buddyBlock : block;

        removeMemoryBlock(memoryManager->memoryBlockMap, first_block);
        removeMemoryBlock(memoryManager->memoryBlockMap, second_block);

        MemoryBlock *merged_block = first_block;
        merged_block->order++;
        merged_block->state = FREE;
        merged_block->prev = NULL;
        merged_block->next = NULL;

        return merged_block;
    }

    return NULL;
}

static MemoryBlock *split_block(MemoryManagerADT memoryManager, MemoryBlock *block_to_split)
{
    if (block_to_split == NULL || block_to_split->state == ALLOCATED || block_to_split->order <= MIN_ORDER)
    {
        return block_to_split;
    }

    uint8_t new_order = block_to_split->order - 1;
    uintptr_t block_addr = (uintptr_t)block_to_split;
    uintptr_t buddy_addr = block_addr ^ (1ULL >> new_order);
    uint64_t heap_size = 1ULL << memoryManager->maxExp;
    if (!is_block_in_heap(buddy_addr, (uintptr_t)firstAddress, heap_size))
    {   
        return block_to_split;
    }
    MemoryBlock *buddyBlock = (MemoryBlock *)buddy_addr;

    block_to_split = removeMemoryBlock(memoryManager->memoryBlockMap, block_to_split);

    block_to_split->state = FREE;
    block_to_split->order = new_order; 
    block_to_split->prev = NULL;
    block_to_split->next = memoryManager->memoryBlockMap[new_order];
    if (memoryManager->memoryBlockMap[new_order] != NULL)
    {
        memoryManager->memoryBlockMap[new_order]->prev = block_to_split;
    }
    memoryManager->memoryBlockMap[new_order] = block_to_split;

    createMemoryBlock(buddyBlock, new_order);
    buddyBlock->prev = NULL;
    buddyBlock->next = memoryManager->memoryBlockMap[new_order];
    if (memoryManager->memoryBlockMap[new_order] != NULL)
    {
        memoryManager->memoryBlockMap[new_order]->prev = buddyBlock;
    }
    memoryManager->memoryBlockMap[new_order] = buddyBlock;

    return block_to_split;
}

MemoryManagerADT createMemoryManager(void *startMem, uint64_t totalSize)
{
    if (totalSize < sizeof(MemoryManagerCDT) + sizeof(MemoryInfoCDT) + (1ULL << MIN_ORDER))
    {
        return NULL;
    }

    MemoryManagerADT memoryManager = (MemoryManagerADT)startMem;
    firstAddress = memoryManager;

    memoryManager->memoryInfo = (MemoryInfoADT)((uintptr_t)memoryManager + sizeof(MemoryManagerCDT));
    initMemoryInfo(memoryManager->memoryInfo);

    uintptr_t blocks_start_addr = (uintptr_t)memoryManager->memoryInfo + sizeof(MemoryInfoCDT);
    uintptr_t heap_end = (uintptr_t)startMem + totalSize;
    uint64_t available_size = heap_end - blocks_start_addr;
    if (available_size < (1ULL << MIN_ORDER)) {
        return NULL; 
    }
    
    uint8_t maxExp = log2_floor(available_size);
    if (maxExp < MIN_ORDER) {
        maxExp = MIN_ORDER;
    }
    

    uintptr_t aligned_heap_size = 1ULL << maxExp;
    uintptr_t aligned_start_addr = blocks_start_addr;

    if (aligned_start_addr % aligned_heap_size != 0) {
        aligned_start_addr += aligned_heap_size - (aligned_start_addr % aligned_heap_size);
        
        if (aligned_start_addr + aligned_heap_size > heap_end) {
            maxExp--;
            if (maxExp < MIN_ORDER) return NULL;
            aligned_heap_size = 1ULL << maxExp;
            aligned_start_addr = blocks_start_addr;
            if (aligned_start_addr % aligned_heap_size != 0)
                aligned_start_addr += aligned_heap_size - (aligned_start_addr % aligned_heap_size);
            if (aligned_start_addr + aligned_heap_size > heap_end) return NULL;
        }
    }
    blocks_start_addr = aligned_start_addr;


    for (int i = 0; i < MAX_ORDERS; i++)
    {
        memoryManager->memoryBlockMap[i] = NULL;
    }

    MemoryBlock *firstMemoryBlock = (MemoryBlock *)blocks_start_addr;
    createMemoryBlock(firstMemoryBlock, maxExp);
    memoryManager->memoryBlockMap[maxExp] = firstMemoryBlock;
    memoryManager->maxExp = maxExp;

    strcpy(memoryManager->memoryInfo->memoryType, "buddy");
    memoryManager->memoryInfo->totalMemory = aligned_heap_size; 
    memoryManager->memoryInfo->freeMemory = aligned_heap_size;

    return memoryManager;
}

void *malloc(const size_t memoryToAllocate)
{
    if (memoryToAllocate == 0)
    {
        return NULL;
    }

    MemoryManagerADT memoryManager = getMemoryManagerInternal();

    if (memoryManager == NULL)
    {
        return NULL;
    }

    uint8_t required_order = get_required_order(memoryToAllocate, memoryManager->maxExp);

    if (required_order == -1 || required_order > memoryManager->maxExp)
    {
        return NULL;
    }


    uint8_t current_order = required_order;
    MemoryBlock *block_to_allocate = NULL;

    // Busca un bloque libre del orden requerido o superior
    while (current_order <= memoryManager->maxExp)
    {        

        if (memoryManager->memoryBlockMap[current_order] != NULL)
        {
            block_to_allocate = removeMemoryBlock(memoryManager->memoryBlockMap, memoryManager->memoryBlockMap[current_order]);

            break;
        }
        current_order++;
    }

    if (block_to_allocate == NULL)
    {
        return NULL;
    }

    // Divide el bloque si es necesario
    while (block_to_allocate->order > required_order)
    {
        block_to_allocate = split_block(memoryManager, block_to_allocate);
        
        uintptr_t block_addr = (uintptr_t)block_to_allocate;
        uintptr_t buddy_addr = block_addr ^ (1ULL << (block_to_allocate->order -1));
        block_to_allocate = (MemoryBlock*) buddy_addr;
    }

    block_to_allocate->state = ALLOCATED;
    memoryManager->memoryInfo->freeMemory -= (1ULL << block_to_allocate->order);

    return (void *)((uintptr_t)block_to_allocate + sizeof(MemoryBlock));
}

void free(void *memoryToFree)
{
    if (memoryToFree == NULL)
    {
        return;
    }

    MemoryManagerADT memoryManager = getMemoryManagerInternal();
    if (memoryManager == NULL)
    {
        return;
    }

    MemoryBlock *block_to_free = (MemoryBlock *)((uintptr_t)memoryToFree - sizeof(MemoryBlock));

    if (block_to_free->state == FREE)
    {
        return; 
    }

    block_to_free->state = FREE;
    memoryManager->memoryInfo->freeMemory += (1ULL << block_to_free->order);

    MemoryBlock *current_block = block_to_free;
    while (current_block->order < memoryManager->maxExp)
    {
        MemoryBlock *merged_block = merge_blocks(memoryManager, current_block);

        if (merged_block != NULL)
        {
            current_block = merged_block;
        }
        else
        {
            break;
        }
    }

    
    current_block->next = memoryManager->memoryBlockMap[current_block->order];
    if (memoryManager->memoryBlockMap[current_block->order] != NULL)
    {
        memoryManager->memoryBlockMap[current_block->order]->prev = current_block;
    }
    memoryManager->memoryBlockMap[current_block->order] = current_block;
    current_block->prev = NULL;
}