#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <globals.h>

typedef int (*EntryPoint)();

#define CANT_FILE_DESCRIPTORS 3


typedef struct process{
    char *name;
    char **argv;
    int argc;
    EntryPoint rip;
    ProcessStatus status;
    void *rsp;
    uint8_t priority;
    uint8_t quantum;
    uint16_t foreground;
    uint64_t stack;
    uint64_t basePointer;
    uint16_t PID;
    int16_t fileDescriptors[CANT_FILE_DESCRIPTORS];

} Process;



void freeProcess(Process *process);

uint16_t waitForChildren(uint16_t pid); 

uint64_t setUpStackFrame(uint64_t stackBase, uint64_t code, int argc, char *args[], uint64_t originalEntryPoint);

uint8_t initProcess(Process *process, uint16_t pid, uint64_t rip, char **args, int argc, uint16_t fileDescriptors[]);

char **allocArgv(Process *p, char **argv, int argc);

void processWrapper(void (*entryPoint)(int, char**), int argc, char **argv);



#endif 