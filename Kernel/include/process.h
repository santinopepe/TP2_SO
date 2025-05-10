#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

typedef enum {
    BLOCKED = 0,
    READY,
    RUNNING,
    ZOMBIE,
    DEAD
} ProcessStatus;

typedef struct process{
    ProcessStatus status;
    void *rsp;
    uint8_t priority;
    uint8_t quantum;
    uint16_t foreground;
    void *stack;
    void *basePointer;
} Process;


typedef struct ProcessData{ //datos del proceso para hacer el ps
    char name[20]; //nombre del proceso, 20 porque pintó
    uint16_t pid; //pid del proceso
    uint8_t priority;
    void *stack;
    void *basePointer;
    uint16_t foreground; //1 si es foreground, 0 si es background
} ProcessData;

#endif 