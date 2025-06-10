#include "tests.h"
#include "MemoryManager.h"

#define STRUCT_SIZE 256 * 1024 * 1024 // 256MB


int main() {

    static uint8_t memory[HEAP_SIZE + STRUCT_SIZE ];
    createMemoryManager(memory,HEAP_SIZE + STRUCT_SIZE); 
    char * argv[] = {"test_mm", "1000000"}; 
    test_mm(2, argv);
    return 0;
}