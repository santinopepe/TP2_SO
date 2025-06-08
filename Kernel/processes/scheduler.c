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
#include <semaphoresManager.h>



#define STACK_SIZE 0x1000 //4kb

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

int killForegroundProcess(){
    SchedulerADT scheduler = getSchedulerADT();
    if(scheduler == NULL){
        return -2; 
    }
    uint8_t index = 0;
    for (int i = 0; index < scheduler->processCount && i < MAX_PROCESOS; i++) {
        // El proceso 0 es la shell
        if ( i != 0 && scheduler->process[i].status != DEAD && scheduler->process[i].fileDescriptors[STDIN] == STDIN ) // o i != shellPID
        {
            index = 1; // Hay al menos un proceso en foreground
            killProcess(i);
        }
    }
    if (index == 0) {
        return -1; // No hay procesos en foreground
    }
    return 0; 
}   

int killProcess(uint16_t pid) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL) {
        return -2;
    }
    if (scheduler->process[pid].status == DEAD) {
        return -1;
    }
    closePipeEndsForPID(pid);
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
    if( (int) scheduler->process[pid].stack != NULL){
        free((void *) scheduler->process[pid].stack);
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

    uint16_t parentPID = scheduler->process[pid].parentPID;
    if (parentPID != pid ) {
        // Elimina el hijo de la lista de hijos del padre
        removeElement(scheduler->process[parentPID].children, &scheduler->process[pid].PID);

        if(scheduler->process[parentPID].children_sem != -1){
        // Si la lista está vacía, todos los hijos terminaron
            if (isEmpty(scheduler->process[parentPID].children)) {
                sem_post(scheduler->process[parentPID].children_sem);
                sem_close(scheduler->process[parentPID].children_sem);
                scheduler->process[parentPID].children_sem = -1; // Indica que no hay hijos
            }
        }
    }

    if (scheduler->process[pid].children != NULL) {
        freeList(scheduler->process[pid].children);
        scheduler->process[pid].children = NULL;
    }

    scheduler->processCount--;
    yield();
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
    scheduler->process[pid].foreground = fileDescriptors[STDIN] == STDIN; // Si el primer file descriptor es STDIN, es un proceso en foreground
    scheduler->process[pid].rip = (EntryPoint)processWrapper;

    scheduler->process[pid].argc = argc;


    uint16_t parentPID;
    if (scheduler->processCount == 0) {
        parentPID = pid; // La shell es su propio padre 
    } else {
        parentPID = scheduler->currentPID;
    }
    scheduler->process[pid].parentPID = parentPID;
    scheduler->process[pid].children = createDoubleLinkedList();
    scheduler->process[pid].children_sem = -1;

    if (parentPID != pid && scheduler->process[pid].foreground) { // Evita que el proceso 0 se agregue a sí mismo        
        insertLast(scheduler->process[parentPID].children, &scheduler->process[pid].PID);
    }

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
        removeElement(scheduler->readyList, &scheduler->process[pid].PID);
        insertLast(scheduler->blockedList, &scheduler->process[pid].PID); //agrega el proceso a la lista de bloqueados
        yield();
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


void *schedule(void *prevStackPointer) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL || scheduler->readyList->first == NULL || scheduler->processCount == 0) {
        return prevStackPointer;
    }

    if(scheduler->process[scheduler->currentPID].quantum == 0 && scheduler->readyList->first != NULL){
        scheduler->process[scheduler->currentPID].quantum = MIN_QUANTUM * (1 + scheduler->process[scheduler->currentPID].priority);
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

    


    return scheduler->process[scheduler->currentPID].rsp;
}

void processWrapper(void (*EntryPoint)(int, char**), int argc, char **argv) {
    EntryPoint(argc, argv);
    SchedulerADT scheduler = getSchedulerADT();
    killProcess(scheduler->currentPID); // marcalo como DEAD, libera memoria
}

ProcessData * processInfo(int * size) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL) {
        return NULL;
    }
    uint8_t i = 0; 
    ProcessData *process= malloc(sizeof(ProcessData) * scheduler->processCount);
    int processIndex = 0;
    while( i < MAX_PROCESOS && processIndex < scheduler->processCount ) {
        if (scheduler->process[i].status != DEAD) {
            process[processIndex].pid = scheduler->process[i].PID;
            process[processIndex].priority = scheduler->process[i].priority;
            process[processIndex].foreground = scheduler->process[i].foreground;
            process[processIndex].stack = scheduler->process[i].stack;
            process[processIndex].basePointer = scheduler->process[i].basePointer;
            process[processIndex].status = scheduler->process[i].status;
            process[processIndex].name = scheduler->process[i].name;
            processIndex++;
        }
        i++;
    }
    *size = processIndex; // Retorna el tamaño del array de procesos
    return process;
}


int changeFDS(uint16_t pid, uint16_t fileDescriptors[]){
    SchedulerADT scheduler = getSchedulerADT();

    if (scheduler == NULL || pid >= MAX_PROCESOS || scheduler->process[pid].status == DEAD) {
        return -1;
    }

    for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
        scheduler->process[pid].fileDescriptors[i] = fileDescriptors[i];
    }

    return 0;
}

int getFileDescriptor(uint8_t fd) {
    SchedulerADT scheduler = getSchedulerADT();
    return (scheduler != NULL && scheduler->currentPID < MAX_PROCESOS) ? scheduler->process[scheduler->currentPID].fileDescriptors[fd] : -1;
}