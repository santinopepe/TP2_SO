#include <stdint.h>
#include <scheduler.h>

#define MAX_PROCESOS 1000

typedef struct SchedulerCDT{

} SchedulerCDT;

SchedulerCDT *createScheduler(){

}

int killProcess(uint16_t pid){ //devuelve 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso

}

void *setPriority(uint16_t pid, uint8_t priority){ //devuelve 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad

}

void waitForChildren(uint16_t pid){ //devuelve 0 si pudo esperar a los hijos, -1 si no existe el proceso o -2 si no se puede esperar a los hijos

}

uint16_t createProcess(){ //devuelve el pid del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos

}

void setStatus(uint16_t pid, int status){ 

}

void yield(){ //funcion para renuciar al cpu, para ceder su espacio a otro proceso, se usa en el scheduler para cambiar de proceso, se usa en el dispatcher

}
