#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE 256 * 1024 * 1024 // 256MB
#define PAGE_SIZE 64 // 4KB
#define FREE 0



typedef struct MemoryManagerCDT * MemoryManagerADT;


/**
 * @brief  Crea el administrador de memoria
 * @param  startMem: Direccion de inicio de la memoria
 * @param  totalSize: Tamaño total de la memoria
 */
void createMemoryManager(void * startMem, uint64_t totalSize); 

/**
 * @brief  Reserva memoria del heap
 * @param  memoryToAllocate: Cantidad de memoria a reservar
 * @return Direccion de la memoria reservada
 */

void * malloc(const size_t memoryToAllocate);

/**
 * @brief  Libera memoria del heap
 * @param  memoryToFree: Direccion de la memoria a liberar
 */

void free(void * memoryToFree); 

/**
 * @brief  Devuelve el administrador de memoria
 * @return Puntero al administrador de memoria
 */

char * getMemoryType();

/**
 * @brief  Devuelve la cantidad de memoria usada
 * @return Cantidad de memoria usada en bytes
 */

int getUsedMemory();

/**
 * @brief  Devuelve la cantidad de memoria libre
 * @return Cantidad de memoria libre en bytes
 */

int getFreeMemory();

#endif // MEMORY_MANAGER_H