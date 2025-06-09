// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process.h>
#include <stdint.h>
#include <MemoryManager.h>
#include <lib.h>
#include <scheduler.h>
#include "../../Shared/include/string.h"
#include "../../Shared/include/stdlib.h"
#include <semaphoresManager.h>


char **allocArgv(Process *p, char **argv, int argc){
    char **newArgv = malloc((argc+1) * sizeof(char*));
        if(newArgv == NULL){
            return NULL;
        }

        for(int i=0; i<argc; i++){
            newArgv[i] = malloc(strlen(argv[i])+1);
            
            if(newArgv[i] == NULL){
                for(int j=0; j<1; j++){
                    free(newArgv[j]);
                }
                free(newArgv);
                return NULL;
            }
            strcpy(newArgv[i], argv[i]);
        }

        newArgv[argc] = NULL;
        return newArgv;
    }

void freeProcess(Process *process){
    if(process == NULL){
        return; 
    }
    free(&process->rsp);
    free(&process->stack);
    free(&process->basePointer);
    free(process);
}

uint16_t waitForChildren() {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL) return -1;

    uint16_t myPID = getPid();
    Process *me = &scheduler->process[myPID];
    if (me == NULL || me->status == DEAD) return -1;

    if (me->children == NULL || isEmpty(me->children)) return 0;

    if (me->children_sem == -1) {
        me->children_sem = me->PID + 2;
        sem_open(me->children_sem);
        create_sem(me->children_sem, 0);
    }
    sem_wait(me->children_sem);
    yield();
    return 0;
}