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


SchedulerADT getSchedulerADT();

int killProcess(uint16_t pid); //devuelve 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso

int setPriority(uint16_t pid, uint8_t priority); //devuelve 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad

//int waitForChildren(); //devuelve 0 si pudo esperar a los hijos, -1 si no existe el proceso o -2 si no se puede esperar a los hijos

uint16_t createProcess(EntryPoint rip, char **argv, int argc, uint8_t priority, uint16_t fileDescriptors[]); //devuelve el pid del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos

void yield();

int setStatus(uint16_t pid, ProcessStatus status);

SchedulerADT createScheduler();

int unblockProcess(uint16_t pid);

int blockProcess(uint16_t pid);

Process *findProcess(uint16_t pid); //devuelve el proceso con el pid pasado por parametro o NULL si no existe el proceso

uint16_t getPid();

void *schedule(void * prevStackPointer);

int killForegroundProcess();

void processInfo(ProcessData * process); 

int changeFDS(uint16_t pid, uint16_t fileDescriptors[]); 

#endif 