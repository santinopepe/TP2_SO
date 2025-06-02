#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdint.h>

#define STDIN 0 
#define STDOUT 1 
#define STDERR 2 

#define MAX_PROCESOS 1000

extern uint16_t fileDescriptors[3];

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

typedef struct MemoryInfoCDT * MemoryInfoADT;

typedef enum {
    BLOCKED = 0,
    READY,
    RUNNING,
    ZOMBIE,
    DEAD
} ProcessStatus;

typedef struct ProcessData{ 
    char name[20]; 
    uint16_t pid; 
    uint8_t priority;
    uint64_t stack;
    uint64_t basePointer;
    uint16_t foreground; 
    ProcessStatus status; 
} ProcessData;

typedef int (*EntryPoint)();

#endif