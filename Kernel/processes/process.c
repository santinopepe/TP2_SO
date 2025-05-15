#include <process.h>
#include <stdint.h>
#include <MemoryManager.h>
#include <lib.h>
#include <scheduler.h>

static int strlen(const char *str){ //esto Hay que cambiarlo
    int len = 0;
    while(str[len] != '\0'){
        len++;
    }
    return len;
}

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


static char **allocArgv(Process *p, char **argv, int argc);



uint8_t initProcess(Process *process, uint16_t pid, uint64_t rip, char **args, int argc, uint16_t fileDescriptors[]){
    if(pid > 1){ 
        process->status = BLOCKED; //Si es la shell arranca bloqueado
    }
    else{
        process->status = READY;
    }
    process->PID = pid;
    process->priority = 0;
    process->quantum = MIN_QUANTUM;
    process->foreground = 0; 
    process->rip=rip;
    process->argc=argc;
    process->name = malloc(strlen(args[0])+1);
    if(process->name==NULL){
        return -1;
    }
    process->argv = allocArgv(process, process->argv, argc);
    strcpy(process->name, args[0]);

    if(process->argv==NULL){
        return -1;
    }

    process->basePointer = (uint64_t) malloc(STACK_SIZE) + STACK_SIZE; //Esto es la base del stack

    if(process->basePointer == 0){ //si no se pudo crear el stack o el basePointer
        free((void*) (process->argv));
        return -1; //no se pudo crear el proceso
    }

    process->rsp = (void *) process->basePointer; //esto es el stack, el rsp apunta al final del stack
    process->stack = setUpStackFrame(process->basePointer, process->rip, argc, process->argv);

    for(int i=0; i<CANT_FILE_DESCRIPTORS; i++){
        process->fileDescriptors[i] = fileDescriptors[i];
    }

    return 0;
}

static char **allocArgv(Process *p, char **argv, int argc){
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