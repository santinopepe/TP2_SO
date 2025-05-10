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
    while ((1 << (log_val + 1)) <= n)
    {
        log_val++;
    }
    return log_val;
}

static uint8_t get_required_order(size_t size, uint8_t max_order)
{
    size_t required_size = size + sizeof(MemoryBlock);

    uint8_t order = MIN_ORDER;

    size_t block_size = 1 << MIN_ORDER; // 2^MIN_ORDER

    while (block_size < required_size && order < max_order)
    {
        order++;
        block_size <<= 1; // double the size
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

    if (block->order >= MAX_ORDERS)
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
    uintptr_t buddy_addr = block_addr ^ (1 << block->order);
    uintptr_t heap_start = (uintptr_t)memoryManager->memoryBlockMap[memoryManager->maxExp];
    uint64_t heap_size = 1ULL << memoryManager->maxExp;
    if (!is_block_in_heap(buddy_addr, heap_start, heap_size))
    {
        return NULL;
    }
    MemoryBlock *buddyBlock = (MemoryBlock *)buddy_addr;

    if (buddyBlock->state == FREE && buddyBlock->order == block->order)
    {
        removeMemoryBlock(memoryManager->memoryBlockMap, block);
        removeMemoryBlock(memoryManager->memoryBlockMap, buddyBlock);

        MemoryBlock *merged_block = (block_addr < buddy_addr) ? block : buddyBlock;
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
    uintptr_t buddy_addr = block_addr ^ (1 << new_order);
    uintptr_t heap_start = (uintptr_t)memoryManager->memoryBlockMap[memoryManager->maxExp];
    uint64_t heap_size = 1ULL << memoryManager->maxExp;
    if (!is_block_in_heap(buddy_addr, heap_start, heap_size))
    {
        return block_to_split;
    }
    MemoryBlock *buddyBlock = (MemoryBlock *)buddy_addr;

    removeMemoryBlock(memoryManager->memoryBlockMap, block_to_split);

    block_to_split->state = FREE;
    block_to_split->order = new_order;

    createMemoryBlock(buddyBlock, new_order);

    block_to_split->next = memoryManager->memoryBlockMap[new_order];
    if (memoryManager->memoryBlockMap[new_order] != NULL)
    {
        memoryManager->memoryBlockMap[new_order]->prev = block_to_split;
    }
    memoryManager->memoryBlockMap[new_order] = block_to_split;
    block_to_split->prev = NULL;

    buddyBlock->next = memoryManager->memoryBlockMap[new_order];
    if (memoryManager->memoryBlockMap[new_order] != NULL)
    {
        memoryManager->memoryBlockMap[new_order]->prev = buddyBlock;
    }
    memoryManager->memoryBlockMap[new_order] = buddyBlock;
    buddyBlock->prev = NULL;

    // Devuelve el bloque de menor dirección
    return (block_addr < buddy_addr) ? block_to_split : buddyBlock;
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

    uintptr_t heap_start = (uintptr_t)startMem;
    uintptr_t heap_end = heap_start + totalSize;
    uintptr_t blocks_start_addr = (uintptr_t)memoryManager->memoryInfo + sizeof(MemoryInfoCDT);
    uint8_t maxExp = log2_floor(heap_end - blocks_start_addr);

    // Encuentra la mayor potencia de 2 que quepa alineada dentro del heap
    while (1)
    {
        uintptr_t alignment = 1ULL << maxExp;
        uintptr_t aligned_addr = blocks_start_addr;
        if (aligned_addr % alignment != 0)
            aligned_addr += alignment - (aligned_addr % alignment);
        if (aligned_addr + (1ULL << maxExp) <= heap_end)
        {
            blocks_start_addr = aligned_addr;
            break;
        }
        if (maxExp == MIN_ORDER)
            return NULL; // No hay espacio suficiente
        maxExp--;
    }

    // Prints de debug
    char buffer[32];
    print("[buddy] blocks_start_addr: 0x");
    itoa(blocks_start_addr, buffer, 16);
    print(buffer);
    print("\n[buddy] maxExp: ");
    itoa(maxExp, buffer, 10);
    print(buffer);
    print("\n[buddy] heap_start: 0x");
    itoa(heap_start, buffer, 16);
    print(buffer);
    print("\n[buddy] heap_end: 0x");
    itoa(heap_end, buffer, 16);
    print(buffer);
    print("\n");

    for (int i = 0; i < MAX_ORDERS; i++)
    {
        memoryManager->memoryBlockMap[i] = NULL;
    }

    MemoryBlock *firstMemoryBlock = (MemoryBlock *)blocks_start_addr;
    createMemoryBlock(firstMemoryBlock, maxExp);
    memoryManager->memoryBlockMap[maxExp] = firstMemoryBlock;
    memoryManager->maxExp = maxExp;

    print("[buddy] firstMemoryBlock addr: 0x");
    itoa((uintptr_t)firstMemoryBlock, buffer, 16);
    print(buffer);
    print(" order: ");
    itoa(firstMemoryBlock->order, buffer, 10);
    print(buffer);
    print("\n");

    print("[buddy] memoryBlockMap[maxExp] = 0x");
    itoa((uintptr_t)memoryManager->memoryBlockMap[maxExp], buffer, 16);
    print(buffer);
    print("\n");

    print("[buddy] Estado de memoryBlockMap tras inicialización:\n");
    for (int i = 0; i <= maxExp; i++)
    {
        print("[buddy] memoryBlockMap[");
        itoa(i, buffer, 10);
        print(buffer);
        print("] = 0x");
        itoa((uintptr_t)memoryManager->memoryBlockMap[i], buffer, 16);
        print(buffer);
        print("\n");
    }

    strcpy(memoryManager->memoryInfo->memoryType, "buddy");
    memoryManager->memoryInfo->totalMemory = totalSize;
    memoryManager->memoryInfo->freeMemory = (1 << memoryManager->maxExp);

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

    char buffer[32];
    print("[buddy] malloc: required_order = ");
    itoa(required_order, buffer, 10);
    print(buffer);
    print(", maxExp = ");
    itoa(memoryManager->maxExp, buffer, 10);
    print(buffer);
    print("\n");

    print("[buddy] Estado de memoryBlockMap antes de malloc:\n");
    for (int i = 0; i <= memoryManager->maxExp; i++)
    {
        print("[buddy] memoryBlockMap[");
        itoa(i, buffer, 10);
        print(buffer);
        print("] = 0x");
        itoa((uintptr_t)memoryManager->memoryBlockMap[i], buffer, 16);
        print(buffer);
        print("\n");
    }

    int iter = 0;
    while (current_order <= memoryManager->maxExp)
    {
        if (memoryManager->memoryBlockMap[current_order] != NULL)
        {
            block_to_allocate = removeMemoryBlock(memoryManager->memoryBlockMap, memoryManager->memoryBlockMap[current_order]);
            break;
        }
        current_order++;
        iter++;
        if (iter > 100)
        {
            print("[buddy] ERROR: ciclo infinito en malloc\n");
            break;
        }
    }

    if (block_to_allocate == NULL)
    {
        return NULL;
    }

    int i = 0;

    while (block_to_allocate->order >= required_order )
    {
        block_to_allocate = split_block(memoryManager, block_to_allocate);
    }

    block_to_allocate->state = ALLOCATED;
    memoryManager->memoryInfo->freeMemory -= (1 << block_to_allocate->order);

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

    block_to_free->state = FREE;
    memoryManager->memoryInfo->freeMemory += (1 << block_to_free->order);

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