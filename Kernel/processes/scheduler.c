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

// ESTO habria que cambiarlo
static void strcpy(char dest[], const char source[])
{
    int i = 0;
    while (1)
    {
        dest[i] = source[i];

        if (dest[i] == '\0')
        {
            break;
        }

        i++;
    }
    return;
}

typedef struct SchedulerCDT{
    Process process[MAX_PROCESOS]; //array de procesos, cada posicion es el pid del proceso
    uint8_t processCount; //cantidad de procesos en el array
    DoubleLinkedListADT readyList;
    DoubleLinkedListADT blockedList;
    uint8_t currentPID; 
    uint64_t quantum; 
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
    scheduler->quantum = 0;
    scheduler->currentPID = 0; 

    for(int i = 0; i < MAX_PROCESOS; i++){
        scheduler->process[i].status = DEAD;
        scheduler->process[i].rsp = NULL;
        scheduler->process[i].priority = 0; //0 es la menor prioridad
        scheduler->process[i].quantum = MIN_QUANTUM;
        scheduler->process[i].foreground = 0; //0 si es background, 1 si es foreground
        scheduler->process[i].stack = 0; 
        scheduler->process[i].basePointer = 0; 
        scheduler->process[i].argv=NULL;
        scheduler->process[i].argc=0;
        scheduler->process[i].rip=0;
        scheduler->process[i].fileDescriptors[0]=0;
        scheduler->process[i].fileDescriptors[1]=0;
        scheduler->process[i].fileDescriptors[2]=0;
        scheduler->process[i].name=NULL;
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
        removeElement(scheduler->readyList, &pid); 
    } else if(scheduler->process[pid].status == BLOCKED){
        removeElement(scheduler->blockedList, &pid); 
    }
    
    scheduler->process[pid].stack=0;
    scheduler->process[pid].basePointer=0;
    scheduler->process[pid].status = DEAD;
    scheduler->process[pid].rsp = NULL;
    scheduler->process[pid].priority = 0;
    scheduler->process[pid].quantum = 0;
    scheduler->process[pid].foreground = 0;
    scheduler->process[pid].argv=NULL;
    scheduler->process[pid].argc=0;
    scheduler->process[pid].rip=0;
    scheduler->process[pid].name=NULL;

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

uint16_t createProcess(uint64_t rip, char **args, int argc, uint8_t priority, uint16_t fileDescriptors[]){ //devuelve el pid del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos
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
    Process* p = &scheduler->process[pid];

    if(initProcess(p, pid, rip, args, argc, fileDescriptors)==-1){
        free(p);
        return -1;
    }
    
    if(scheduler->readyList->first==NULL){
        insertFirst(scheduler->readyList, &pid);
    }
    else{
        insertLast(scheduler->readyList, &pid);
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
        scheduler->currentPID = *(uint8_t *)scheduler->readyList->first->next->data;
        
        aux = scheduler->readyList->first;
        
        scheduler->readyList->first = scheduler->readyList->first->next;
        scheduler->readyList->first->prev = NULL;
        
        aux->next = NULL;
        
        scheduler->readyList->last->next = aux;
        aux->prev = scheduler->readyList->last;
        scheduler->readyList->last = aux;
    }

    //Creo que esto no es roundRobin pero por ahora lo dejo asi
    scheduler->currentPID = *(uint8_t *)scheduler->readyList->first->data; //si no hay siguiente, vuelve al primero
    scheduler->process[scheduler->currentPID].status = RUNNING; 

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
        removeElement(scheduler->readyList, &pid);

        insertLast(scheduler->blockedList, &pid); //agrega el proceso a la lista de bloqueados
    } else if (scheduler->process[scheduler->currentPID].status == READY){
        scheduler->process[scheduler->currentPID].status = BLOCKED; 
        removeElement(scheduler->readyList, &pid);

        insertLast(scheduler->blockedList, &pid); //agrega el proceso a la lista de bloqueados
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
        
        
        if(scheduler->readyList->first == NULL){
            insertFirst(scheduler->readyList, &pid);
        } else {
            insertLast(scheduler->readyList, &pid);
        }

        removeElement(scheduler->blockedList, &pid); 
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
            strcpy(processData[j].name, processData[i].name);
            j++;
        }
    }

    return processData;
}

void *schedule(void * prevStackPointer){
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler == NULL){
        return prevStackPointer; 
    } 
    if (scheduler->readyList->first == NULL){
        return prevStackPointer; 
    }
    scheduler->quantum--;
    if(scheduler->quantum >0 && scheduler->process[scheduler->currentPID].status == RUNNING){
        return scheduler->process[scheduler->currentPID].rsp; //si es mayor que 0 que siga ejecurando 
    }

    if(scheduler->quantum == 0 || scheduler->process[scheduler->currentPID].status == BLOCKED || scheduler->process[scheduler->currentPID].status == DEAD){
        scheduler->process[scheduler->currentPID].rsp = prevStackPointer; 
        processSwitch(); 
        
    }
    
    scheduler->quantum= scheduler->process[scheduler->currentPID].quantum; 

    return scheduler->process[scheduler->currentPID].rsp; 
}