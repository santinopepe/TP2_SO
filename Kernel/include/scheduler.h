#include <stdint.h> 

typedef struct SchedulerCDT * SchedulerADT; 

int killProcess(int pid); //devuelve 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso

void *setPriority(int pid, int priority); //devuelve 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad

void waitForChildren(int pid); //devuelve 0 si pudo esperar a los hijos, -1 si no existe el proceso o -2 si no se puede esperar a los hijos

uint16_t createProcess(); //devuelve el pid del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos

void yield();

void setStatus(uint16_t pid, int status);

SchedulerADT createScheduler();