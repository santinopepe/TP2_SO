// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*#include <syscalls.h>
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 2)
    return -1;
  printf("test_mm: Iniciando...\n");

  if ((max_memory = satoi(argv[1])) <= 0)
    return -1;

  printf("test_mm: max_memory = %d bytes\n", (int)max_memory);

  while (1) {
    rq = 0;
    total = 0;

    printf("Fase 1: Solicitando bloques de memoria...\n");
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    printf(" -> Se solicitaron %d bloques, sumando %d bytes.\n", rq, total);
    printf("Fase 2: Chequeando Memoria\n");
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          return -1;
        }
    

    printf("Fase 3: Liberando memoria...\n");
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        free(mm_rqs[i].address);

  }
  printf("test_mm completado con EXITO!\n\n");
}*/


#include <tests.h>     
#include <test_util.h>
#include <stdlib.h>       
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>  
#include <MemoryManager.h>  

#define MAX_BLOCKS 128

typedef struct MM_rq {
    void *address;
    uint32_t size;
} mm_rq;

int test_mm(int argc, char *argv[]) {
    printf("test_mm: Iniciando...\n");
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;

    if (argc != 2) {
         printf("test_mm ERROR: Se esperaba 1 argumento (max_memory), recibidos %d\n", argc); 
         return -1; 
    }

    max_memory = satoi(argv[1]);

    if (max_memory <= 0) {
        printf("test_mm ERROR: max_memory invalido (%s -> %d)\n", argv[1], max_memory); 
        return -1; 
    }

    rq = 0;
    total = 0;

    printf("Iniciando test_mm con max_memory = %d bytes\n", max_memory);

    printf("Fase 1: Solicitando bloques de memoria...\n");
    while (rq < MAX_BLOCKS && total < max_memory) {
        uint32_t size_to_alloc = GetUniform(max_memory - total - 1) + 1;
        if (size_to_alloc == 0) size_to_alloc = 1; 

        mm_rqs[rq].address = malloc(size_to_alloc);

        if (mm_rqs[rq].address != NULL) {
            mm_rqs[rq].size = size_to_alloc;
            total += mm_rqs[rq].size; 
            rq++;
        } else {
            printf(" Malloc devolvio NULL (bloque %d, intento size=%d, total acumulado=%d)\n", rq, size_to_alloc, total);
            break; 
        }
    }
     printf(" -> Se solicitaron %d bloques, sumando %d bytes.\n", rq, total);

    uint32_t i;
    printf("Fase 2: Escribiendo en la memoria asignada...\n");
    for (i = 0; i < rq; i++) {
        if (mm_rqs[i].address) {
            memset(mm_rqs[i].address, i % 256, mm_rqs[i].size); 

            if (i == 0) {
                printf("DEBUG: Verificando bloque 0 inmediatamente despues de su memset...\n");
                if (!memcheck(mm_rqs[0].address, 0, mm_rqs[0].size)) {
                    printf("DEBUG: ERROR! Bloque 0 corrupto INMEDIATAMENTE despues de memset!\n");
                    // Podrías imprimir los primeros bytes para ver qué hay
                    uint8_t* temp_ptr = (uint8_t*)mm_rqs[0].address;
                    printf("DEBUG: Primeros bytes: %x %x %x %x\n", temp_ptr[0], temp_ptr[1], temp_ptr[2], temp_ptr[3]);
                } else {
                    printf("DEBUG: Bloque 0 OK inmediatamente despues de su memset.\n");
            }
        }
        }
    }
     printf(" -> Escritura completada.\n");

    printf("Fase 3: Verificando contenido de la memoria...\n");
    for (i = 0; i < rq; i++) {
        if (mm_rqs[i].address) {
            if (!memcheck(mm_rqs[i].address, i % 256, mm_rqs[i].size)) { 
                printf("test_mm ERROR: Fallo de verificacion en bloque %d! Addr=%d, Size=%d, Expected=(uint8_t)%d (0x%d)\n",
                i, mm_rqs[i].address, mm_rqs[i].size, (uint8_t)(i % 256), (uint8_t)(i % 256));
                for (uint32_t j = 0; j < rq; j++) {
                    if(mm_rqs[j].address) free(mm_rqs[j].address);
                }
                return -1; 
            }
        }
    }
     printf(" -> Verificacion completada.\n");

    printf("Fase 4: Liberando memoria...\n");
    for (i = 0; i < rq; i++) {
        if (mm_rqs[i].address) {
            free(mm_rqs[i].address); 
        }
    }
     printf(" -> Memoria liberada.\n");


    printf("test_mm completado con EXITO!\n\n");
    return 0; 
}