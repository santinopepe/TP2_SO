#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

typedef struct MemoryManagerCDT * MemoryManagerADT;

typedef struct MemoryInfoCDT * MemoryInfoADT;

MemoryInfoADT *createMemoryInfoCopy(MemoryInfoADT memoryInfo);

void initMemoryInfo(MemoryInfoADT memoryInfo);

#endif
