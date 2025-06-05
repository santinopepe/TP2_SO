#include <process.h>
#include <stdint.h>
#include <MemoryManager.h>
#include <lib.h>
#include <scheduler.h>
#include <semaphoresManager.h>


static int strlen(const char *str){ //esto Hay que cambiarlo
    int len = 0;
    while(str[len] != '\0'){
        len++;
    }
    return len;
}

// ESTO habria que cambiarlo
static void strcpy(char *dest, const char *source) {
    if (dest == NULL || source == NULL) {
        return;
    }
    int i = 0;
    while (source[i] != '\0') {
        dest[i] = source[i];
        i++;
    }
    dest[i] = '\0';
}




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

uint16_t waitForChildren(uint16_t pid) {
    SchedulerADT scheduler = getSchedulerADT();
    if (scheduler == NULL) return -1;

    

    Process *parent = &scheduler->process[pid];
    if (parent == NULL || parent->status == DEAD) return -1;

    if (parent->children == NULL || isEmpty(parent->children)) return 0;

    if (parent->children_sem == -1) {
        parent->children_sem = parent->PID + 2;
        create_sem(parent->children_sem, 0);
        sem_open(parent->children_sem);
    }
    sem_wait(parent->children_sem);

    return 0;
}