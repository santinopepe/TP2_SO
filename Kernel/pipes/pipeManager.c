#include <pipeManager.h>
#include <MemoryManager.h>
#include <stddef.h>
#include <stdint.h>
#include <process.h>
#include <scheduler.h>

typedef struct pipeManagerCDT
{
    Pipe pipes[MAX_PIPES];
    uint8_t pipeCount;
} pipeManagerCDT;

static pipeManagerADT pipeManager = NULL;

pipeManagerADT createPipeManager()
{

    pipeManager = (pipeManagerADT)malloc(sizeof(pipeManagerCDT));
    if (pipeManager == NULL)
    {
        return NULL;
    }
    pipeManager->pipeCount = 0;

    for (int i = 0; i < MAX_PIPES; i++)
    {
        pipeManager->pipes[i] = createPipe();
    }

    return pipeManager;
}

Pipe createPipe()
{
    Pipe pipe;
    pipe.readIndex = 0;
    pipe.writeIndex = 0;
    pipe.size = 0;
    pipe.fd = -1;
    pipe.inputPID = -1;
    pipe.outputPID = -1;
    pipe.readLock = 0;
    pipe.writeLock = 0;
    return pipe;
   
}



uint8_t openPipe(uint16_t pid, uint8_t mode)
{

    if (pipeManager->pipeCount >= MAX_PIPES){
        return -1;
    }
    uint8_t pipeIndex = 0;
    uint8_t foundPipe = 0;
    for (; pipeIndex < MAX_PIPES; pipeIndex++){
        if (pipeManager->pipes[pipeIndex].fd == -1){
            pipeManager->pipes[pipeIndex] = createPipe();
            
            if (mode == READ && pipeManager->pipes[pipeIndex].inputPID == -1){
                pipeManager->pipes[pipeIndex].inputPID = pid;
                pipeManager->pipes[pipeIndex].outputPID = -1;
            } else if (mode == WRITE && pipeManager->pipes[pipeIndex].outputPID == -1){
                pipeManager->pipes[pipeIndex].inputPID = -1;
                pipeManager->pipes[pipeIndex].outputPID = pid;
            }

            pipeManager->pipeCount++;
            return pipeIndex;
        }
    }

    return -1;
  
}

uint8_t closePipe(uint8_t fd)
{

    if (fd > MAX_PIPES || pipeManager->pipes[fd].fd == -1)
    {
        return 0;
    }

    pipeManager->pipes[fd].fd = -1;
    pipeManager->pipes[fd].inputPID = -1;
    pipeManager->pipes[fd].outputPID = -1;
    pipeManager->pipeCount--;

    return 1;
}

uint8_t writePipe(uint8_t fd, char *buffer, uint8_t size)
{

    if (fd > MAX_PIPES || size > PIPE_SIZE || pipeManager->pipes[fd].writeLock || pipeManager->pipes[fd].size == PIPE_SIZE)
    {
        return 0;
    }


    for (int i = 0; i < size; i++)
    {

        while (pipeManager->pipes[fd].writeIndex == pipeManager->pipes[fd].readIndex && pipeManager->pipes[fd].size != 0)
        { // Si el buffer está lleno, espera a que haya espacio
            pipeManager->pipes[fd].writeLock = 1;
            blockProcess(pipeManager->pipes[fd].outputPID); // ACA PODEMOS HACER UN blockByPipe
        }

        pipeManager->pipes[fd].buffer[pipeManager->pipes[fd].writeIndex] = buffer[i];
        pipeManager->pipes[fd].writeIndex++;
        pipeManager->pipes[fd].writeIndex = (pipeManager->pipes[fd].writeIndex + 1) % PIPE_SIZE; // Circular buffer

        if (pipeManager->pipes[fd].readLock)
        {
            pipeManager->pipes[fd].readLock = 0;
            unblockProcess(pipeManager->pipes[fd].inputPID); // Desbloquea el proceso que estaba esperando para leer
        }
    }

    pipeManager->pipes[fd].size += size;
    pipeManager->pipes[fd].writeLock = 0;

    return size;
}

uint8_t readPipe(uint8_t fd, char *buffer, uint8_t size)
{

    if (fd > MAX_PIPES || size > PIPE_SIZE || pipeManager->pipes[fd].readLock || pipeManager->pipes[fd].size == 0)
    {
        return 0;
    }

    uint8_t bytesToRead = size < pipeManager->pipes[fd].size ? size : pipeManager->pipes[fd].size;

    for (int i = 0; i < bytesToRead; i++)
    {

        while (pipeManager->pipes[fd].size == 0)
        { // Si el buffer está vacío, espera a que haya datos
            pipeManager->pipes[fd].readLock = 1;
            blockProcess(pipeManager->pipes[fd].inputPID); // ACA PODEMOS HACER UN blockByPipe
        }

        buffer[i] = pipeManager->pipes[fd].buffer[pipeManager->pipes[fd].readIndex];
        pipeManager->pipes[fd].readIndex++;
        pipeManager->pipes[fd].readIndex = (pipeManager->pipes[fd].readIndex + 1) % PIPE_SIZE; // Circular buffer

        if (pipeManager->pipes[fd].writeLock)
        {
            pipeManager->pipes[fd].writeLock = 0;
            unblockProcess(pipeManager->pipes[fd].outputPID); // Desbloquea el proceso que estaba esperando para escribir
        }
    }

    pipeManager->pipes[fd].size -= bytesToRead;
    pipeManager->pipes[fd].readLock = 0;

    return bytesToRead;
}