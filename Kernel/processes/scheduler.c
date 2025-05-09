#include <stdint.h>
#include <scheduler.h>
#include <process.h>
#include <MemoryManager.h>
#include <lib.h>

#define MAX_PROCESOS 1000
#define STACK_SIZE 0x1000 //4kb

static void * SchedulerPointer = NULL;  

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
    uint16_t foreground;
    void *stack;
    void *basePointer;
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

typedef struct ProcessData{ //datos del proceso para hacer el ps
    char name[20]; //nombre del proceso, 20 porque pintó
    uint16_t pid; //pid del proceso
    uint8_t priority;
    void *stack;
    void *basePointer;
    uint16_t foreground; //1 si es foreground, 0 si es background
} ProcessData;


static SchedulerADT getSchedulerADT(){
    return SchedulerPointer; 
}


SchedulerADT createScheduler(){
    SchedulerADT scheduler = malloc(sizeof(SchedulerCDT));
    if(scheduler == NULL){
        return NULL;
    }

    SchedulerPointer = scheduler;

    scheduler->processCount = 0;
    scheduler->first = NULL;
    for(int i = 0; i < MAX_PROCESOS; i++){
        scheduler->process[i].status = DEAD;
        scheduler->process[i].rsp = NULL;
        scheduler->process[i].priority = 0;
        scheduler->process[i].quantum = 0;
        scheduler->process[i].foreground = 0; //0 si es background, 1 si es foreground
    }
    return scheduler;
}

int killProcess(uint16_t pid){ //devuelve 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso
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
    scheduler->process[pid].foreground = 0;
    scheduler->process[pid].stack = NULL; //libera la memoria del stack
    scheduler->process[pid].basePointer = NULL; //libera la memoria del basePointer

    return 0; 

}

void setPriority(uint16_t pid, uint8_t priority){ //devuelve 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler == NULL){
        return -1; 
    }
    scheduler->process[pid].priority = priority; //cambia la prioridad del proceso al que se le pasa el pid
}

void waitForChildren(){ //devuelve 0 si pudo esperar a los hijos, -1 si no existe el proceso o -2 si no se puede esperar a=los hi && (priority > 3 && priority < 0)jos
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL){
        return -1; 
    }
    scheduler->process[scheduler->currentPID].status = BLOCKED; //bloqueo el proceso actual para esperar a los hijos
}

void setStatus(uint16_t pid, ProcessStatus status){ 
    SchedulerADT scheduler = getSchedulerADT(); 
    scheduler->process[pid].status = status; //cambia el estado del proceso al que
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


//Aca tmb habria que hacer lo de crear un stack falso, (el burro de arranque)
// SS con 0x0
// RSP con ???
// RFLAGS con 0x202
// CS con 0x8
// RIP ??? ACA seria la primera instruccion a ejecutar
// Los parametros de la funcion a ejecutar

uint16_t createProcess(void * entry_point, void * argv){ //devuelve el pid del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos
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
    scheduler->process[pid].foreground = 0; //Pq aca usas scheduler->processCount en vez de pid?
    scheduler->process[pid].stack = malloc(STACK_SIZE); //Esto es la base del stack? Pq tenemos esto y RSP 
    scheduler->process[pid].basePointer = malloc(STACK_SIZE); //O esto es la base del stack
    scheduler->processCount++;

    if(scheduler->process[pid].stack == NULL || scheduler->process[pid].basePointer == NULL){ //si no se pudo crear el stack o el basePointer
        return -1; //no se pudo crear el proceso
    }

    scheduler->process[pid].rsp = scheduler->process[pid].stack + STACK_SIZE; //esto es el stack, el rsp apunta al final del stack
    

    //Creacion de Stack falso (El burro de arranque)
    uint64_t *stack = (uint64_t *)scheduler->process[pid].rsp;

    *--stack = 0x0;              // SS
    *--stack = (uint64_t)stack;  // RSP
    *--stack = 0x202;            // RFLAGS
    *--stack = 0x8;              // CS
    *--stack = (uint64_t)entry_point; // RIP

    *--stack = (uint64_t)argv;    // RDI
    *--stack = 0;                // RSI
    *--stack = 0;                // RDX
    *--stack = 0;                // RCX
    *--stack = 0;                // R8
    *--stack = 0;                // R9
    *--stack = 0;                // RAX
    *--stack = 0;                // RBX
    *--stack = 0;                // RBP
    *--stack = 0;                // R12
    *--stack = 0;                // R13
    *--stack = 0;                // R14
    *--stack = 0;                // R15

    scheduler->process[pid].rsp = (void *)stack;

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


//ESTO puede andar pero tenemos que tener cuidado con el tema de procesos bloqueados, pq si 
// bloqueamos el proceso y lo sacamos de la lista de listos, no se puede usar esta funcion
// pero si no lo sacamos seteamos el currentpid al siguiente y despues lo sacamos de la lista de listos funca.
void processSwitch(){

    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL || scheduler->first == NULL){
        return; 
    }
    
    uint8_t pid = scheduler->currentPID;
    scheduler->process[pid].status = READY; //ESTO puede que este mal, solo podemos usar esta funcion si el proceso esta en running, si se bloquea no se puede usar

    ReadyList * findNextProcess = scheduler->first; 
   
    while(findNextProcess != NULL){ 
        if(pid == findNextProcess->PID && findNextProcess->next != NULL){ 
            scheduler->currentPID = findNextProcess->next->PID; 
            scheduler->process[findNextProcess->PID].status = RUNNING; //cambia el estado del proceso a running
            return; 
        }
    }

    //Creo que esto no es roundRobing pero por ahora lo dejo asi
    scheduler->currentPID = scheduler->first->PID; //si no hay siguiente, vuelve al primero

}  

ProcessData *ps(){ //lo ideal aca seria devolver un array con la info de cada proceso para luego imprimirlo haciendo el llamado en userland
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL){
        return NULL; 
    }
    ProcessData *processData = malloc(sizeof(ProcessData) * scheduler->processCount); //array de procesos, cada posicion es el pid del proceso
    if(processData == NULL){
        return NULL; 
    }
    for(int i = 0, j=0; i < scheduler->processCount; i++){ //el i recorre el array de procesos y el j recorre el array de procesosData, si el proceso no es muerto se copia en el array de procesosData
        if(scheduler->process[i].status != DEAD){
            processData[j].pid = i;
            processData[j].priority = scheduler->process[i].priority;
            processData[j].stack = scheduler->process[i].stack;
            processData[j].basePointer = scheduler->process[i].basePointer;
            processData[j].foreground = scheduler->process[i].foreground;
            j++;
        }
    }

    return processData;
}