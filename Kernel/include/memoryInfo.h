#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

typedef struct MemoryInfoCDT * MemoryInfoADT;

MemoryInfoADT createMemoryInfoCopy(MemoryInfoADT memoryInfo);

void initMemoryInfo(MemoryInfoADT info); 

#endif
