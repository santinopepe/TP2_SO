#include <stdint.h>
#include <semaphoresManager.h>


void sem_open(Semaphore *sem){

}

void sem_close(Semaphore *sem){

}

void sem_wait(Semaphore *sem){

}

void sem_post(Semaphore *sem){

}

void create_sem(Semaphore *sem, uint8_t value){
    sem->value = value;
}