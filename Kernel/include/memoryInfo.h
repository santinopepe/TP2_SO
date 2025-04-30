#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

typedef struct MemoryInfoCDT *MemoryInfoADT;

MemoryInfo *createMemoryInfoCopy(MemoryInfo *memoryInfo);
void initMemoryInfo(MemoryInfo *memoryInfo);

#endif