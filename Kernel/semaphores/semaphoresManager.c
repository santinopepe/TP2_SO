#include <stdint.h>
#include <semaphoresManager.h>
#include <MemoryManager.h>
#include <doubleLinkedListADT.h>
#include <scheduler.h>
#include <process.h>


#define IN_USE 1
#define NOT_IN_USE 0



extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);

typedef struct Semaphore{
    uint8_t value; //Valor del semaforo
    uint8_t used;  //Indica si el semaforo esta en uso o no
    uint8_t lock; //Esto lo usamos para entradas criticas

    DoubleLinkedListADT waitingProcesses; //Lista de procesos que esperan por el semaforo
}Semaphore; 

typedef struct  semaphoresManagerCDT {
    Semaphore semaphores[QUANTITY_OF_SEMAPHORES];
}semaphoresManagerCDT;


SemaphoreADT semaphoresManager = NULL;

SemaphoreADT create_semaphoresManager(){

    semaphoresManager = (SemaphoreADT) malloc(sizeof(semaphoresManagerCDT));
    if (semaphoresManager == NULL){
        return NULL;
    }

    for (int i = 0; i < QUANTITY_OF_SEMAPHORES; i++){
        semaphoresManager->semaphores[i].value = 0;
        semaphoresManager->semaphores[i].used = NOT_IN_USE;
        semaphoresManager->semaphores[i].waitingProcesses = createDoubleLinkedList();
        
        if (semaphoresManager->semaphores[i].waitingProcesses == NULL){
            return NULL;
        }

    }

    return semaphoresManager;

}


int8_t sem_open(uint8_t sem){
    if(semaphoresManager == NULL || sem >= QUANTITY_OF_SEMAPHORES){
        return -1;
    }

    return -1 * (semaphoresManager->semaphores[sem].used == NOT_IN_USE);
}

int8_t sem_close(uint8_t sem){
    if(semaphoresManager == NULL || sem >= QUANTITY_OF_SEMAPHORES || semaphoresManager->semaphores[sem].used == NOT_IN_USE){
        return -1;
    }

    semaphoresManager->semaphores[sem].used = NOT_IN_USE;
    semaphoresManager->semaphores[sem].value = 0;

    freeList(semaphoresManager->semaphores[sem].waitingProcesses);
    semaphoresManager->semaphores[sem].waitingProcesses = createDoubleLinkedList();
    
    return 0;
}

int8_t sem_wait(uint8_t sem){
    if(semaphoresManager == NULL || sem >= QUANTITY_OF_SEMAPHORES || semaphoresManager->semaphores[sem].used == NOT_IN_USE){
        return -1;
    }

    acquire(&semaphoresManager->semaphores[sem].lock);

    if (semaphoresManager->semaphores[sem].value > 0){
        semaphoresManager->semaphores[sem].value--;
        release(&semaphoresManager->semaphores[sem].lock);
        return 0;
    }
    
    int16_t * pid = (int16_t *) malloc(sizeof(int16_t));
    if (pid == NULL){
        release(&semaphoresManager->semaphores[sem].lock);
        return -1;
    }

    *pid = getPid(); 
    insertLast(semaphoresManager->semaphores[sem].waitingProcesses, pid);
    release(&semaphoresManager->semaphores[sem].lock);
    blockProcess(*pid); //PODRIAMOS HACER blockBySemaphore(*pid); que setea una variable en el struct del PCB con un valor que inidica que lo bloqueo un semaforo
    return 0; //El proceso se bloquea y espera a que el semaforo se libere
}



int8_t sem_post(uint8_t sem){
    if(semaphoresManager == NULL || sem >= QUANTITY_OF_SEMAPHORES || semaphoresManager->semaphores[sem].used == NOT_IN_USE){
        return -1;
    }

    acquire(&semaphoresManager->semaphores[sem].lock);

    while(!isEmpty(semaphoresManager->semaphores[sem].waitingProcesses)){
        int16_t * pid = (int16_t *) getFirst(semaphoresManager->semaphores[sem].waitingProcesses); // get and remove
        removeElement(semaphoresManager->semaphores[sem].waitingProcesses, pid);
        Process * process = findProcess(*pid);
        if (process == NULL || process->status == DEAD){
            free(pid);
            continue;
        }
        unblockProcess(*pid);
        free(pid);
        break;
    }

    if(isEmpty(semaphoresManager->semaphores[sem].waitingProcesses)){
        semaphoresManager->semaphores[sem].value++;
    }

    release(&semaphoresManager->semaphores[sem].lock);
    return 0;
}

int8_t create_sem(uint8_t sem, uint8_t value){

    if(semaphoresManager == NULL || sem >= QUANTITY_OF_SEMAPHORES || semaphoresManager->semaphores[sem].used == IN_USE){
        return -1; 
    }
    semaphoresManager->semaphores[sem].value = value;
    semaphoresManager->semaphores[sem].used = IN_USE;
    
    return 0;
}

int8_t sem_checkUse(uint8_t sem){
    if(semaphoresManager == NULL || sem >= QUANTITY_OF_SEMAPHORES){
        return -1; 
    }
    return semaphoresManager->semaphores[sem].used;
}