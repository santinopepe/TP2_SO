#include <process.h>
#include <stdint.h>
#include <MemoryManager.h>
#include <lib.h>
#include <scheduler.h>

static void * SchedulerPointer = NULL;  

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