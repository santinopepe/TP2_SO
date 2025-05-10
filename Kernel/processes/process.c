#include <process.h>
#include <stdint.h>
#include <MemoryManager.h>
#include <lib.h>
#include <scheduler.h>

void freeProcess(Process *process){
    if(process == NULL){
        return; 
    }
    free(process->rsp);
    free(process->stack);
    free(process->basePointer);
    free(process);
}

uint16_t waitForChildren(uint16_t pid){ 
    //devuelve 0 si pudo esperar a los hijos, -1 si no existe el proceso 
    //o -2 si no se puede esperar a=los hi && (priority > 3 && priority < 0)jos
    SchedulerADT scheduler = getSchedulerADT(); 
    if(scheduler == NULL){
        return -1; 
    }
    Process *process = findProcess(pid); //si eventualmente hacemos una lista de bloqueados

    blockProcess(); //bloquea el proceso que está corriendo
    //addToBlocked 
    return 0; 
}   

void initProcess(/*Params del proceso*/){

}