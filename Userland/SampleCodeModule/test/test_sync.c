#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <globals.h>
#include <syscalls.h>
#include <test_util.h>

#define SEM_ID 50
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
    if (!sem_open(SEM_ID)) {
      printf("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      sem_wait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      sem_post(SEM_ID);
  }

  if (use_sem)
    sem_close(SEM_ID);

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{name,n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];


  if (argc != 3){
    printf("test_sync: ERROR: Se esperaban 2 argumentos (n, use_sem), recibidos %d\n", argc - 1);
    return -1;
  }
  char *argvDec[] = {"my_processes_dec", argv[1], "-1", argv[2], NULL};
  char *argvInc[] = {"my_processes_inc", argv[1], "1", argv[2], NULL};
  global = 0;

  uint64_t i;
  
  printf("Iniciando test_sync...\n");
  printf("Argumentos del test: n=%s, use_sem=%s\n", argv[1], argv[2]);

  printf("Creando procesos...\n");
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    printf("  Creando par de procesos %d (Dec e Inc)...\n", i + 1);
    pids[i] = createProcess((EntryPoint)my_process_inc, argvDec, 3, 1, fileDescriptors);
    printf("    Proceso Dec (PID: %d) creado.\n", pids[i]);
    pids[i + TOTAL_PAIR_PROCESSES] = createProcess((EntryPoint)my_process_inc, argvInc, 3, 1, fileDescriptors);
    printf("    Proceso Inc (PID: %d) creado.\n", pids[i + TOTAL_PAIR_PROCESSES]);
  }

  printf("Esperando a que los procesos terminen...\n");
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    printf("  Esperando al par de procesos %d (PID Dec: %d, PID Inc: %d)...\n", i + 1, pids[i], pids[i + TOTAL_PAIR_PROCESSES]);
    printf("    Proceso Dec (PID: %d) finalizado.\n", pids[i]);
    printf("    Proceso Inc (PID: %d) finalizado.\n", pids[i + TOTAL_PAIR_PROCESSES]);
  }
  waitForChildren(); // Espera a que todos los procesos terminen

  printf("Todos los procesos han terminado.\n");
  printf("Final value: %d\n", global);

  return 0;
}
