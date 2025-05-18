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

char **allocArgv(Process *p, char **argv, int argc);

/*
uint8_t initProcess(Process *process, uint16_t pid, uint64_t rip, char **args, int argc, uint16_t fileDescriptors[]){
    if(pid < 1){ 
        process->status = BLOCKED; //Si es la shell arranca bloqueado
    }
    else{
        process->status = READY;
    }
    process->PID = pid;
    process->priority = 0;
    process->quantum = MIN_QUANTUM;
    process->foreground = 1;
    process->rip = rip;
    process->argc = argc;

    // Asignar memoria para el nombre
    process->name = malloc(strlen(args[0]) + 1);
    if (process->name == NULL) {
        return -1;
    }
    process->argv = allocArgv(process, args, argc);
    strcpy(process->name, args[0]);

    // Asignar memoria para argv
    process->argv = allocArgv(process, args, argc);
    if (process->argv == NULL) {
        free(process->name);
        return -1;
    }

    // Asignar memoria para el stack (asegurar espacio para alineación)
    process->basePointer = (uint64_t)malloc(STACK_SIZE + 16);
    // Alinear basePointer a 16 bytes
    process->basePointer = (process->basePointer + STACK_SIZE + 15) & ~0xF;
    if (process->basePointer == 0) {
        free(process->name);
        for (int i = 0; i < argc; i++) {
            free(process->argv[i]);
        }
        free(process->argv);
        return -1;
    }

    

    // Configurar el stack frame
    process->stack = setUpStackFrame(process->basePointer, rip, argc, process->argv);
    process->rsp = (void *)process->stack;

    for (int i = 0; i < CANT_FILE_DESCRIPTORS; i++) {
        process->fileDescriptors[i] = fileDescriptors[i];
    }

    process->status = (pid == 0) ? RUNNING : READY;

    return 0;
} */

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