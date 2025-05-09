#include <stdint.h> 


typedef enum { BLOCKED = 0,
    READY,
    RUNNING,
    ZOMBIE,
    DEAD } ProcessStatus;

typedef struct SchedulerCDT * SchedulerADT; 

int killProcess(uint16_t pid); //devuelve 0 si pudo matar el proceso, -1 si no existe el proceso o -2 si no se puede matar el proceso

int setPriority(uint16_t pid, uint8_t priority); //devuelve 0 si pudo cambiar la prioridad, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad

int waitForChildren(); //devuelve 0 si pudo esperar a los hijos, -1 si no existe el proceso o -2 si no se puede esperar a los hijos

uint16_t createProcess(); //devuelve el pid del proceso creado o -1 si no se pudo crear el proceso o -2 si no hay espacio en la tabla de procesos

void yield();

int setStatus(uint16_t pid, ProcessStatus status);

SchedulerADT createScheduler();

void unblockProcess(uint16_t pid);

void blockProcess();
