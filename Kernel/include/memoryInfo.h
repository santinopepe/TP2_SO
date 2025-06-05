#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <globals.h>

typedef struct MemoryInfoCDT * MemoryInfoADT;


void initMemoryInfo(MemoryInfoADT info); 

void setTotalMemory(uint64_t value);

void setTotalPages(uint64_t value);

void addPageSize(uint64_t value);

void addTotalMemory(uint64_t value);

void addFreePages(uint64_t value);

void addUsedPages(uint64_t value);

void setFreeMemory(uint64_t value); 

void addFreeMemory(uint64_t value);

void addUsedMemory(uint64_t value);

void setMemoryType(const char *type);

void setPageSize(uint64_t value);

void setFreePages(uint64_t value); 

MemoryInfoADT getMemoryInfo(); 

#endif
