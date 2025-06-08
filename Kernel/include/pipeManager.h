#ifndef PIPEMANAGER_H
#define PIPEMANAGER_H

#include <stdint.h>

#define READ 0
#define WRITE 1
#define PIPE_SIZE 512 // ESTO ES UNA LIMITACIÓN
#define MAX_PIPES 50  // ESTO ES UNA LIMITACIÓN

typedef struct Pipe
{
    char buffer[PIPE_SIZE];
    uint8_t readIndex;
    uint8_t writeIndex;
    uint8_t size;
    int8_t fd;
    int8_t inputPID, outputPID;
    uint8_t readLock, writeLock;
} Pipe;

typedef struct pipeManagerCDT *pipeManagerADT;

/**
 * @brief Crea un nuevo pipeManager
 *  
 * @return pipeManagerADT
 */
pipeManagerADT createPipeManager(void);

/**
 * @brief Crea un nuevo pipe
 * 
 * @return Pipe
 */
Pipe createPipe(void);

/**
 * @brief Abre un pipe
 * @param pid: PID del proceso que abre el pipe
 * @param mode: Modo de apertura del pipe   
 * @return uint8_t: Descriptor del pipe
 */
uint8_t openPipe(uint16_t pid, uint8_t mode);

/**
 * @brief Cierra un pipe
 * @param fd: Descriptor del pipe
 */
uint8_t closePipe(uint8_t fd);

/**
 * @brief Escribe en un pipe
 * @param fd: Descriptor del pipe
 * @param buffer: Buffer de datos a escribir
 * @param size: Tamaño del buffer de los datos a escribir
 * @return uint8_t: Cantidad de los datos escritos
 */
uint8_t writePipe(uint8_t fd, char *buffer, uint8_t size);

/**
 * @brief Lee en un pipe
 * @param fd: Descriptor del pipe
 * @param buffer: Buffer de datos a leer
 * @param size: Tamaño del buffer de los datos a leer
 * @return uint8_t: Cantidad de los datos leídos
 */
uint8_t readPipe(uint8_t fd, char *buffer, uint8_t size);

int killPipedProcesses(); 

#endif