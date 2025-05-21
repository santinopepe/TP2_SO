#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <globals.h>

typedef struct MemoryInfoCDT * MemoryInfoADT;

MemoryInfoADT createMemoryInfoCopy(MemoryInfoADT memoryInfo);

void initMemoryInfo(MemoryInfoADT info); 

#endif
