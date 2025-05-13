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
    uint64_t stack;
    uint64_t basePointer;
    uint16_t PID;
} Process;


typedef struct ProcessData{ //datos del proceso para hacer el ps
    char name[20]; //nombre del proceso, 20 porque pintó
    uint16_t pid; //pid del proceso
    uint8_t priority;
    uint64_t stack;
    uint64_t basePointer;
    uint16_t foreground; //1 si es foreground, 0 si es background
} ProcessData;

void freeProcess(Process *process);

uint16_t waitForChildren(uint16_t pid); 

uint64_t setUpStackFrame(uint64_t stackBase, uint64_t code, int argc, char *args[]);

#endif 