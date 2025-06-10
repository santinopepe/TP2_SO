// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MemoryManager.h"
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
      mm_rqs[rq].address = mm_malloc(mm_rqs[rq].size);

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
}


