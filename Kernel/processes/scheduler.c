#include <stdint.h>
#include <scheduler.h>
#include <process.h>
#include <MemoryManager.h>
#include <lib.h>
#include <stddef.h>
#include <doubleLinkedListADT.h>
#include <video.h>
#include <stdlib.h>
#include <string.h>
#include <globals.h>



#define STACK_SIZE 0x1000 //4kb
#define MIN_QUANTUM 1

static void * SchedulerPointer = NULL;  



SchedulerADT getSchedulerADT(){
    return SchedulerPointer; 
}


SchedulerADT createScheduler(){
    SchedulerADT scheduler = malloc(sizeof(struct SchedulerCDT));
    if(scheduler == NULL){
        return NULL;
    }

    SchedulerPointer = scheduler;
    scheduler->readyList = createDoubleLinkedList();
    scheduler->blockedList = createDoubleLinkedList();

    scheduler->processCount = 0;
    scheduler->quantum = MIN_QUANTUM;
    scheduler->currentPID = 0; 
    scheduler->killFgProcess = 0; 
    scheduler->hasStarted = 0; 

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

void killForegroundProcess(){
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler == NULL){
        return; 
    }
    scheduler->killFgProcess = 1;
}   

int killProcess(uint16_t pid) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL) {
        return -2;
    }
    if (scheduler->process[pid].status == DEAD) {
        return -1;
    }

    // Eliminar de la lista correspondiente
    if (scheduler->process[pid].status == RUNNING || scheduler->process[pid].status == READY) {
        removeElement(scheduler->readyList, &scheduler->process[pid].PID);
    } else if (scheduler->process[pid].status == BLOCKED) {
        removeElement(scheduler->blockedList, &scheduler->process[pid].PID);
    }

    // Liberar memoria
    if (scheduler->process[pid].name != NULL) {
        free(scheduler->process[pid].name);
    }
    if (scheduler->process[pid].argv != NULL) {
        for (int i = 0; i < scheduler->process[pid].argc; i++) {
            if (scheduler->process[pid].argv[i] != NULL) {
                free(scheduler->process[pid].argv[i]);
            }
        }
        free(scheduler->process[pid].argv);
    }
    if (scheduler->process[pid].basePointer != 0) {
        free((void *)(scheduler->process[pid].basePointer - STACK_SIZE));
    }

    // Reiniciar campos
    scheduler->process[pid].stack = 0;
    scheduler->process[pid].basePointer = 0;
    scheduler->process[pid].status = DEAD;
    scheduler->process[pid].rsp = NULL;
    scheduler->process[pid].priority = 0;
    scheduler->process[pid].quantum = MIN_QUANTUM;
    scheduler->process[pid].foreground = 0;
    scheduler->process[pid].argv = NULL;
    scheduler->process[pid].argc = 0;
    scheduler->process[pid].rip = 0;
    scheduler->process[pid].name = NULL;
    scheduler->process[pid].fileDescriptors[0] = 0;
    scheduler->process[pid].fileDescriptors[1] = 0;
    scheduler->process[pid].fileDescriptors[2] = 0;

    scheduler->processCount--;
    if(pid == scheduler->currentPID) {
        yield(); // cede el CPU al siguiente proceso
    }
    return 0;
}

int setPriority(uint16_t pid, uint8_t priority) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL || pid >= MAX_PROCESOS || priority < 0) {
        return -1;
    }
    if (scheduler->process[pid].status == DEAD) {
        return -1;
    }
    scheduler->process[pid].priority = priority;
    scheduler->process[pid].quantum = MIN_QUANTUM * (1 + priority);
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

uint16_t createProcess(EntryPoint originalEntryPoint, char **argv, int argc, uint8_t priority, uint16_t fileDescriptors[]) {
    if (originalEntryPoint == NULL || argv == NULL || argc < 0) {
        return -1;
    }

    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL) {
        return -1;
    }

    if (scheduler->processCount >= MAX_PROCESOS) {
        return -2; // excedido de procesos
    }

    // Buscamos el primer proceso muerto para usar su pid
    int i = 0;
    while (scheduler->process[i].status != DEAD) {
        i++;
    }

    uint8_t pid = i;

    scheduler->process[pid].status = READY;
    scheduler->process[pid].PID = pid;
    scheduler->process[pid].priority = priority;
    scheduler->process[pid].quantum = MIN_QUANTUM* (1 + priority);
    scheduler->quantum = (pid == 0) ? MIN_QUANTUM: scheduler->process[pid].quantum; // Si es el primer proceso, no se cambia el quantum del scheduler
    scheduler->process[pid].foreground = fileDescriptors[0] == STDIN ? 1 : 0; // Si el primer file descriptor es STDIN, es un proceso en foreground
    scheduler->process[pid].rip = (EntryPoint)processWrapper;

    scheduler->process[pid].argc = argc;

    scheduler->process[pid].name = malloc(strlen(argv[0]) + 1);
    if (scheduler->process[pid].name == NULL) {
        return -1;
    }
    strcpy(scheduler->process[pid].name, argv[0]);

    scheduler->process[pid].argv = allocArgv(&scheduler->process[pid], argv, argc);
    if (scheduler->process[pid].argv == NULL) {
        free((void*)scheduler->process[pid].name);
        return -1;
    }

    // Reserva memoria para el stack
    scheduler->process[pid].stack = (uint64_t)malloc(STACK_SIZE); 
    if (scheduler->process[pid].stack == 0) {
        free((void*)scheduler->process[pid].argv);
        free((void*)scheduler->process[pid].name);
        return -1;
    }
    scheduler->process[pid].basePointer = (scheduler->process[pid].stack + STACK_SIZE);
    for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
        scheduler->process[pid].fileDescriptors[i] = fileDescriptors[i];
    }

    if (scheduler->readyList->first == NULL) {
        insertFirst(scheduler->readyList, &scheduler->process[pid].PID);
    } else {
        insertLast(scheduler->readyList, &scheduler->process[pid].PID);
    }

    scheduler->processCount++;
    // Usa la función asm para setear el stack frame inicial
    scheduler->process[pid].rsp = (void*)setUpStackFrame(
        scheduler->process[pid].basePointer,
        (uint64_t)scheduler->process[pid].rip,
        scheduler->process[pid].argc,
        scheduler->process[pid].argv,
        (EntryPoint)originalEntryPoint
    );

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
void processSwitch() {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL || scheduler->readyList->first == NULL) {
        return;
    }

    uint8_t pid = scheduler->currentPID;

    // Cambiar el estado del proceso actual a READY si está RUNNING
    if (scheduler->process[pid].status == RUNNING) {
        scheduler->process[pid].status = READY;
    }

    // Seleccionar el siguiente proceso
    Node *aux = scheduler->readyList->first;
    if (aux->next != NULL) {
        scheduler->currentPID = *(uint8_t *)aux->next->data;
        scheduler->readyList->first = aux->next;
        scheduler->readyList->first->prev = NULL;
        aux->next = NULL;
        scheduler->readyList->last->next = aux;
        aux->prev = scheduler->readyList->last;
        scheduler->readyList->last = aux;
    } else {
        scheduler->currentPID = *(uint8_t *)aux->data; // Volver al primer proceso
    }

    scheduler->process[scheduler->currentPID].status = RUNNING;
}

int blockProcess(uint16_t pid){ //bloquea el proceso, lo saca de la lista de listos y lo agrega a la lista de bloqueados
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler==NULL){
        return -1 ; 
    }
    
    

    if(scheduler->process[pid].status == RUNNING){ 
        scheduler->process[pid].status = BLOCKED; 
        processSwitch();
        removeElement(scheduler->readyList, &scheduler->process[pid].PID);
        insertLast(scheduler->blockedList, &scheduler->process[pid].PID); //agrega el proceso a la lista de bloqueados

    } else if (scheduler->process[pid].status == READY){
        scheduler->process[pid].status = BLOCKED; 
        removeElement(scheduler->readyList, &scheduler->process[pid].PID);
        insertLast(scheduler->blockedList, &scheduler->process[pid].PID); //agrega el proceso a la lista de bloqueados
    }

    return 0;
       
}


Process *findProcess(uint16_t pid){
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler == NULL){
        return NULL;
    }
    return &scheduler->process[pid];
}

int unblockProcess(uint16_t pid){
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler==NULL){
        return -1; 
    }
    
    if(scheduler->process[pid].status == BLOCKED){
        scheduler->process[pid].status = READY; 
        
        
        if(scheduler->readyList->first == NULL){
            insertFirst(scheduler->readyList, &scheduler->process[pid].PID);
        } else {
            insertLast(scheduler->readyList, &scheduler->process[pid].PID);
        }

        removeElement(scheduler->blockedList, &scheduler->process[pid].PID); 
    }
    return 0;
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

void *schedule(void *prevStackPointer) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL || scheduler->readyList->first == NULL || scheduler->processCount == 0) {
        return prevStackPointer;
    }

    // Decrementar quantum
    scheduler->quantum--;
    if (scheduler->quantum > 0 && scheduler->process[scheduler->currentPID].status == RUNNING) {
        return prevStackPointer;
    }

    // Cambiar al siguiente proceso si el quantum es 0 o el proceso está bloqueado/muerto
    if (scheduler->quantum == 0 || scheduler->process[scheduler->currentPID].status != RUNNING) {
        if(scheduler->hasStarted != 0){
            scheduler->process[scheduler->currentPID].rsp = prevStackPointer;
        }
        processSwitch();      
        scheduler->quantum = scheduler->process[scheduler->currentPID].quantum;
        scheduler->hasStarted = 1; // Indica que el scheduler ha comenzado a funcionar
    }

   

    // Verificar que RSP no sea nulo
    if (scheduler->process[scheduler->currentPID].rsp == NULL) {
        // Manejar error (por ejemplo, matar el proceso)
        killProcess(scheduler->currentPID);
        return schedule(prevStackPointer); // Intentar con el siguiente proceso
    }

    uint16_t currentPID = scheduler->currentPID;

    if(scheduler->killFgProcess && scheduler->process[currentPID].fileDescriptors[STDIN] == STDIN){
        scheduler->killFgProcess=0;
        killProcess(currentPID);
    }

    return scheduler->process[scheduler->currentPID].rsp;
}

void processWrapper(void (*EntryPoint)(int, char**), int argc, char **argv) {
    EntryPoint(argc, argv);
    SchedulerADT scheduler = getSchedulerADT();
    killProcess(scheduler->currentPID); // marcalo como DEAD, libera memoria
}


void processInfo(ProcessData * process) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL) {
        return;
    }
    uint8_t i = 0; 
    while(i < scheduler->processCount ) {
        if (scheduler->process[i].status != DEAD) {
            process[i].pid = scheduler->process[i].PID;
            process[i].priority = scheduler->process[i].priority;
            process[i].foreground = scheduler->process[i].foreground;
            process[i].stack = scheduler->process[i].stack;
            process[i].basePointer = scheduler->process[i].basePointer;
            process[i].status = scheduler->process[i].status;
            strcpy(process[i].name, scheduler->process[i].name);
        }
        i++;
    }

}
