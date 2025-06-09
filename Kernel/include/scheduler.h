#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h> 
#include <process.h>
#include <doubleLinkedListADT.h>


#define MIN_QUANTUM 1
#define STACK_SIZE 0x1000 //4kb

typedef struct SchedulerCDT{
    Process process[MAX_PROCESOS]; //array de procesos, cada posicion es el pid del proceso
    uint8_t processCount; //cantidad de procesos en el array
    DoubleLinkedListADT readyList;
    DoubleLinkedListADT blockedList;
    uint8_t currentPID; 
    uint64_t quantum; 
    uint8_t hasStarted;
}SchedulerCDT;

typedef struct SchedulerCDT * SchedulerADT; 


/**
 * @brief Obtiene el SchedulerADT actual
 * @return SchedulerADT: Puntero al scheduler actual
 */

SchedulerADT getSchedulerADT();


/**
 * @brief Mata un proceso dado su pid
 * @param pid: PID del proceso a matar
 * @return int: 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso
 */

int killProcess(uint16_t pid); 


/**
 * @brief Cambia la prioridad de un proceso dado su pid
 * @param pid: PID del proceso a cambiar la prioridad
 * @param priority: Nueva prioridad del proceso
 * @return int: 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad
 */

int setPriority(uint16_t pid, uint8_t priority); 

/**
 * @brief Crea un nuevo proceso
 * @param rip: Punto de entrada del proceso
 * @param argv: Argumentos del proceso
 * @param argc: Cantidad de argumentos del proceso
 * @param priority: Prioridad del proceso
 * @param fileDescriptors: Descriptores de archivos del proceso
 * @return uint16_t: PID del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos
 */
uint16_t createProcess(EntryPoint rip, char **argv, int argc, uint8_t priority, uint16_t fileDescriptors[]); 


/**
 * @brief Renuncia al CPU, cede su espacio a otro proceso
 * Se usa en el scheduler para cambiar de proceso, se usa en el dispatcher
 */
void yield();

/**
 * @brief Cambia el estado de un proceso dado su pid
 * @param pid: PID del proceso a cambiar el estado
 * @param status: Nuevo estado del proceso
 * @return int: 0 si pudo cambiar el estado, -1 si no existe el proceso
 */

int setStatus(uint16_t pid, ProcessStatus status);


/**
 * @brief Crea un nuevo scheduler
 * @return SchedulerADT: Puntero al nuevo scheduler
 */
SchedulerADT createScheduler();

/**
 * @brief Desbloquea un proceso dado su pid
 * @param pid: PID del proceso a desbloquear
 * @return int: 0 si pudo desbloquear el proceso, -1 si no existe el proceso o -2 si no se puede desbloquear el proceso
 */

int unblockProcess(uint16_t pid);

/**
 * @brief Bloquea un proceso dado su pid
 * @param pid: PID del proceso a bloquear
 * @return int: 0 si pudo bloquear el proceso, -1 si no existe el proceso o -2 si no se puede bloquear el proceso
 */

int blockProcess(uint16_t pid);

/**
 * @brief Busca un proceso dado su pid
 * @param pid: PID del proceso a buscar
 * @return Process*: Puntero al proceso encontrado o NULL si no existe el proceso
 */

Process *findProcess(uint16_t pid); 

/**
 * @brief Obtiene el PID del proceso actual
 * @return uint16_t: PID del proceso actual o -1 si no se pudo obtener el PID
 */

uint16_t getPid();

/**
 * @brief Espera a que todos los hijos del proceso actual terminen
 * @param prevStackPointer: Puntero al stack del proceso que llama a la función
 * @return uint16_t: PID del hijo que terminó o 0 si no hay hijos
 * 
 */

void *schedule(void * prevStackPointer);

/**
 * @brief Mata el proceso en primer plano
 * @return int: 0 si pudo matar el proceso, -1 si no hay procesos en primer plano o -2 si no se pudo obtener el scheduler
 */

int killForegroundProcess();

/**
 * @brief Obtiene la información de los procesos vivos
 * @param size: Puntero donde se guardará el tamaño del array de procesos
 * @return ProcessData*: Array de procesos vivos o NULL si no se pudo obtener el scheduler
 */

ProcessData * processInfo();


/**
 * @brief Cambia los descriptores de archivos de un proceso dado su pid
 * @param pid: PID del proceso a cambiar los descriptores de archivos
 * @param fileDescriptors: Array de descriptores de archivos a cambiar
 * @return int: 0 si pudo cambiar los descriptores, -1 si no existe el proceso o -2 si no se pudo obtener el scheduler
 */
int changeFDS(uint16_t pid, uint16_t fileDescriptors[]); 

/**
 * @brief Obtiene el descriptor de archivo del proceso actual
 * @param fd: Descriptor de archivo a obtener
 * @return int: Descriptor de archivo del proceso actual o -1 si no se pudo obtener el scheduler o el pid es inválido
 */

int getFileDescriptor(uint8_t fd); 

#endif 