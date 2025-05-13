#include <stdint.h>
#include <scheduler.h>
#include <process.h>
#include <MemoryManager.h>
#include <lib.h>
#include <stddef.h>
#include <doubleLinkedListADT.h>

#define MAX_PROCESOS 1000
#define STACK_SIZE 0x1000 //4kb
#define MIN_QUANTUM 10 

static void * SchedulerPointer = NULL;  

typedef struct SchedulerCDT{
    Process process[MAX_PROCESOS]; //array de procesos, cada posicion es el pid del proceso
    uint8_t processCount; //cantidad de procesos en el array
    DoubleLinkedListADT readyList;
    DoubleLinkedListADT blockedList;
    uint8_t currentPID; 
}SchedulerCDT;

SchedulerADT getSchedulerADT(){
    return SchedulerPointer; 
}


SchedulerADT createScheduler(){
    SchedulerADT scheduler = malloc(sizeof(SchedulerCDT));
    if(scheduler == NULL){
        return NULL;
    }

    SchedulerPointer = scheduler;
    scheduler->readyList = createDoubleLinkedList();
    scheduler->blockedList = createDoubleLinkedList();

    scheduler->processCount = 0;

    for(int i = 0; i < MAX_PROCESOS; i++){
        scheduler->process[i].status = DEAD;
        scheduler->process[i].rsp = NULL;
        scheduler->process[i].priority = 0; //0 es la menor prioridad
        scheduler->process[i].quantum = MIN_QUANTUM;
        scheduler->process[i].foreground = 0; //0 si es background, 1 si es foreground
        scheduler->process[i].stack = NULL; 
        scheduler->process[i].basePointer = NULL; 
    }
    return scheduler;
}

int killProcess(uint16_t pid){ //devuelve 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL ){
        return -2; 
    }
    if(scheduler->process[pid].status == DEAD){
        return -1; 
    } else if(scheduler->process[pid].status == RUNNING || scheduler->process[pid].status == READY){
        removeElement(scheduler->readyList, &scheduler->process[pid]); 
    } else if(scheduler->process[pid].status == BLOCKED){
        removeElement(scheduler->blockedList, &scheduler->process[pid]); 
    }
    

    scheduler->process[pid].status = DEAD;
    scheduler->process[pid].rsp = NULL;
    scheduler->process[pid].priority = 0;
    scheduler->process[pid].quantum = 0;
    scheduler->process[pid].foreground = 0;
    scheduler->process[pid].stack = NULL; //libera la memoria del stack
    scheduler->process[pid].basePointer = NULL; //libera la memoria del basePointer

    scheduler->processCount--; 

    return 0; 

}

int setPriority(uint16_t pid, uint8_t priority){ //devuelve 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler == NULL || priority < 0){
        return -1; 
    }
    scheduler->process[pid].priority = priority; //cambia la prioridad del proceso al que se le pasa el pid
    scheduler->process[pid].quantum = MIN_QUANTUM*(priority); 
    return 0; 
}



int setStatus(uint16_t pid, ProcessStatus status){ 
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL){
        return -1; 
    }
    scheduler->process[pid].status = status; //cambia el estado del proceso al que   
    return 0;
}

void yield(){ //funcion para renuciar al cpu, para ceder su espacio a otro proceso, se usa en el scheduler para cambiar de proceso, se usa en el dispatcher
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL){
        return; 
    }
    scheduler->process[scheduler->currentPID].quantum = 0; 
    callTimerTick();
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

    //Buscamos el primer proceso muerto para usar su pid
    int i = 0; 
    while(scheduler->process[i].status != DEAD){ 
        i++;
    }

    uint8_t pid = i; 
    scheduler->process[pid].status = READY;
    scheduler->process[pid].rsp = NULL;
    scheduler->process[pid].priority = 0;
    scheduler->process[pid].quantum = MIN_QUANTUM;
    scheduler->process[pid].foreground = 0; //Pq aca usas scheduler->processCount en vez de pid?

    //PROBABLEMETE CAMBIEMOS VER LO DE SETSTACKFRAME
    scheduler->process[pid].stack = malloc(STACK_SIZE); //Esto es la base del stack? Pq tenemos esto y RSP 
    scheduler->process[pid].basePointer = scheduler->process[pid].stack ; //O esto es la base del stack

    if(scheduler->process[pid].stack == NULL || scheduler->process[pid].basePointer == NULL){ //si no se pudo crear el stack o el basePointer
        return -1; //no se pudo crear el proceso
    }

    scheduler->process[pid].rsp = scheduler->process[pid].stack + STACK_SIZE; //esto es el stack, el rsp apunta al final del stack
    

    //Creacion de Stack falso (El burro de arranque)
    uint64_t *stack = (uint64_t *)scheduler->process[pid].rsp;
    if(stack == NULL){
        return -1; //no se pudo crear el stack
    }

    *--stack = 0x0;              // SS 

    uint64_t* temp = --stack;
    *temp = (uint64_t)stack;     // RSP
    
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

    if(scheduler->readyList->first==NULL){
        insertFirst(scheduler->readyList, &scheduler->process[pid]);
    }
    else{
        insertLast(scheduler->readyList, &scheduler->process[pid]);
    }

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


// ESTO puede anuint6dar peenemos que tener cuidado con el tema de procesos bloqueados, pq si 
// bloqueamos el proceso y lo sacamos de la lista de listos, no se puede usar esta funcion
// pero si no lo sacamos seteamos el currentpid al siguiente y despues lo sacamos de la lista de listos funca.
//Pasa a ser el setter de running a ready
void processSwitch(){

    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL || scheduler->readyList->first == NULL){
        return; 
    }
    
    uint8_t pid = scheduler->currentPID;

    if(scheduler->process[pid].status == RUNNING){ //si el proceso actual es el que se esta ejecutando
        scheduler->process[pid].status = READY; //cambia el estado del proceso a listo
    }

    Node *aux;
    if (scheduler->readyList->first->next != NULL) {
        /*scheduler->currentPID = scheduler->readyList->first->next->PID;*/
        
        aux = scheduler->readyList->first;
        
        scheduler->readyList->first = scheduler->readyList->first->next;
        scheduler->readyList->first->prev = NULL;
        
        aux->next = NULL;
        
        insertLast(scheduler->readyList, aux); //agrega el proceso al final de la lista de listos
    }

    //Creo que esto no es roundRobin pero por ahora lo dejo asi
   /* scheduler->currentPID = scheduler->readyList->first->PID;*/ //si no hay siguiente, vuelve al primero

}  

void blockProcess(){ //bloquea el proceso, lo saca de la lista de listos y lo agrega a la lista de bloqueados
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler==NULL){
        return; 
    }
    uint16_t pid = scheduler->currentPID;
    

    if(scheduler->process[scheduler->currentPID].status == RUNNING){ 
        scheduler->process[scheduler->currentPID].status = BLOCKED; 
        processSwitch();
        removeElement(scheduler->readyList, &scheduler->process[pid]);

        insertLast(scheduler->blockedList, &scheduler->process[pid]); //agrega el proceso a la lista de bloqueados
    } else if (scheduler->process[scheduler->currentPID].status == READY){
        scheduler->process[scheduler->currentPID].status = BLOCKED; 
        removeElement(scheduler->readyList, &scheduler->process[pid]);

        insertLast(scheduler->blockedList, &scheduler->process[pid]); //agrega el proceso a la lista de bloqueados
    }
       
}


Process *findProcess(uint16_t pid){
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler == NULL){
        return NULL;
    }
    return &scheduler->process[pid];
}

void unblockProcess(uint16_t pid){
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler==NULL){
        return; 
    }
    
    if(scheduler->process[pid].status == BLOCKED){
        scheduler->process[pid].status = READY; 
        Process * unblockedProcess = findProcess(pid);
        
        if(scheduler->readyList->first == NULL){
            insertFirst(scheduler->readyList, unblockedProcess);
        } else {
            insertLast(scheduler->readyList, unblockedProcess);
        }

        removeElement(scheduler->blockedList, unblockedProcess); 
    }
}

ProcessData *ps(){ //lo ideal aca seria devolver un array con la info de cada p{}roceso para luego imprimirlo haciendo el llamado en userland
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

