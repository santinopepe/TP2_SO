#include <stdint.h>
#include <scheduler.h>
#include <process.h>
#include <MemoryManager.h>
#include <lib.h>

#define MAX_PROCESOS 1000

static void * Scheduler;  


typedef enum { BLOCKED = 0,
    READY,
    RUNNING,
    ZOMBIE,
    DEAD } ProcessStatus;

typedef struct process{
    ProcessStatus status;
    void *rsp;
    uint8_t priority;
    uint8_t quantum;
}Process;

typedef struct ReadyList{
    uint8_t PID; 
    struct ReadyList * next;  
    struct ReadyList * prev;     
}ReadyList; 

typedef struct SchedulerCDT{
    Process process[MAX_PROCESOS]; //array de procesos, cada posicion es el pid del proceso
    uint8_t processCount; //cantidad de procesos en el array
    ReadyList * first; 
    uint8_t currentPID; 
}SchedulerCDT;


static SchedulerADT getSchedulerADT(){
    return Scheduler; 
}


SchedulerADT createScheduler(){
    SchedulerADT scheduler = malloc(sizeof(SchedulerCDT));
    if(scheduler == NULL){
        return NULL;
    }

    Scheduler = scheduler;

    scheduler->processCount = 0;
    scheduler->first = NULL;
    for(int i = 0; i < MAX_PROCESOS; i++){
        scheduler->process[i].status = DEAD;
        scheduler->process[i].rsp = NULL;
        scheduler->process[i].priority = 0;
        scheduler->process[i].quantum = 0;
    }
    return scheduler;
}

int killProcess(uint8_t pid){ //devuelve 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL ){
        return -2; 
    }
    if(scheduler->process[pid].status != DEAD){
        return -1; 
    }

    scheduler->process[pid].status = DEAD;
    scheduler->process[pid].rsp = NULL;
    scheduler->process[pid].priority = 0;
    scheduler->process[pid].quantum = 0;

    return 0; 

}

void setPriority(uint8_t pid, uint8_t priority){ //devuelve 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad
    
}

void waitForChildren(uint16_t pid){ //devuelve 0 si pudo esperar a los hijos, -1 si no existe el proceso o -2 si no se puede esperar a=los hi && (priority > 3 && priority < 0)jos

}

void setStatus(SchedulerADT scheduler,uint16_t pid, ProcessStatus status){ 
    scheduler->process[pid].status = status; //cambia el estado del proceso al que
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL){
        return -1; 
    }
    if(scheduler->processCount >= MAX_PROCESOS){
        return -2; //excedido de procesos
    }

    uint8_t pid = scheduler->processCount;
    scheduler->process[pid].status = READY;
    scheduler->process[pid].rsp = NULL;
    scheduler->process[pid].priority = 0;
    scheduler->process[pid].quantum = 0;

    scheduler->processCount++;
      return pid;
}

void yield(){ //funcion para renuciar al cpu, para ceder su espacio a otro proceso, se usa en el scheduler para cambiar de proceso, se usa en el dispatcher

}

uint16_t createProcess(){ //devuelve el pid del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL){
        return -1; 
    }
    if(scheduler->processCount >= MAX_PROCESOS){
        return -2; //excedido de procesos
    }

    uint8_t pid = scheduler->processCount;
    scheduler->process[pid].status = READY;
    scheduler->process[pid].rsp = NULL;
    scheduler->process[pid].priority = 0;
    scheduler->process[pid].quantum = 0;

    scheduler->processCount++;
    return pid;
}  


uint16_t getPid(){
   SchedulerADT scheduler = getSchedulerADT();
 
   if(scheduler == NULL){
        return -1; 
   }  
   return scheduler->currentPID;
}



void processSwitch(){
    
}  