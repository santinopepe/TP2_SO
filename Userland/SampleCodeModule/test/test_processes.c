
#include <stdio.h>
#include <syscalls.h>
#include "test_util.h"
#include <tests.h>
#include <stddef.h>


typedef struct P_rq {
    int32_t pid;
    ProcessStatus state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {"endless_loop", NULL};

  printf("========== INICIO DEL TEST DE PROCESOS ==========\n");

  if (argc != 2) {
    printf("test_processes: ERROR - Se esperaba 1 argumento (max_processes), recibidos %d\n", (int)argc);
    return -1;
  }

  if ((max_processes = satoi(argv[1])) <= 0) {
    printf("test_processes: ERROR - max_processes debe ser un numero positivo. Recibido: %s\n", argv[1]);
    return -1;
  }
  printf("Configuracion del test: max_processes = %d\n", (int)max_processes);

  p_rq p_rqs[max_processes];

  uint64_t iteration = 0;
  while (1) {
    iteration++;
    printf("\n----- Iteracion %d del test de procesos -----\n", (int)iteration);
    alive = 0; // Reiniciar contador de procesos vivos para esta iteración

    printf("Fase de creacion: Creando %d procesos...\n", (int)max_processes);
    uint16_t fileDescriptors[] = {0, 1, 2};
    for (rq = 0; rq < max_processes; rq++) {
      printf("  Creando proceso %d/%d...\n", rq + 1, (int)max_processes);
      p_rqs[rq].pid = createProcess((EntryPoint) endless_loop, argvAux, 1, 0, fileDescriptors); // Asumo que endless_loop es el nombre y NULL el final de argv

      if (p_rqs[rq].pid == -1) {
        printf("test_processes: ERROR creando proceso en el intento %d.\n", rq + 1);
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
        printf("    Proceso creado con PID: %d. Estado: RUNNING. Procesos vivos: %d\n", (int)p_rqs[rq].pid, alive);
      }
    }
    printf("Fase de creacion completada. Total procesos vivos: %d\n", alive);

    printf("Fase de gestion aleatoria (kill/block/unblock)...\n");
    while (alive > 0) {
      printf("  Procesos vivos actualmente: %d. Realizando acciones aleatorias...\n", alive);
      for (rq = 0; rq < max_processes; rq++) {
        if (p_rqs[rq].state == DEAD) continue; // Saltar procesos ya muertos

        action = GetUniform(100) % 2; // 0 para kill, 1 para block/unblock
        
        printf("    Proceso PID %d (Estado actual: %s): ", (int)p_rqs[rq].pid, p_rqs[rq].state == RUNNING ? "RUNNING" : (p_rqs[rq].state == BLOCKED ? "BLOCKED" : "DEAD"));

        switch (action) {
          case 0: // Intentar matar
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              printf("Accion: Intentando matar.\n");
              if (killProcess(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR matando proceso PID %d\n", (int)p_rqs[rq].pid);
                return -1;
              }
              p_rqs[rq].state = DEAD;
              alive--;
              printf("      Proceso PID %d matado. Estado: DEAD. Procesos vivos: %d\n", (int)p_rqs[rq].pid, alive);
            } else {
              printf("Accion: Matar (ya estaba muerto o en estado inesperado).\n");
            }
            break;

          case 1: // Intentar bloquear
            if (p_rqs[rq].state == RUNNING) {
              printf("Accion: Intentando bloquear.\n");
              if (blockProcess(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR bloqueando proceso PID %d\n", (int)p_rqs[rq].pid);
                return -1;
              }
              p_rqs[rq].state = BLOCKED;
              printf("      Proceso PID %d bloqueado. Estado: BLOCKED.\n", (int)p_rqs[rq].pid);
            } else {
               printf("Accion: Bloquear (no estaba RUNNING).\n");
            }
            break;
        }
      }

      // Randomly unblocks processes
      printf("    Realizando intentos de desbloqueo aleatorios...\n");
      for (rq = 0; rq < max_processes; rq++) {
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          printf("      Proceso PID %d (Estado BLOCKED): Intentando desbloquear.\n", (int)p_rqs[rq].pid);
          if (unblockProcess(p_rqs[rq].pid) == -1) {
            printf("test_processes: ERROR desbloqueando proceso PID %d\n", (int)p_rqs[rq].pid);
            return -1;
          }
          p_rqs[rq].state = RUNNING;
          printf("        Proceso PID %d desbloqueado. Estado: RUNNING.\n", (int)p_rqs[rq].pid);
        }
      }
      if (alive == 0) {
        printf("  Todos los procesos de esta iteracion han sido matados.\n");
      }
    }
    printf("----- Fin de la iteracion %d -----\n", (int)iteration);
  }
  printf("========== FIN DEL TEST DE PROCESOS ==========\n");
  return 0; 
}
