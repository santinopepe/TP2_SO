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
    int8_t pipeIndex = 0;
    for (; pipeIndex < MAX_PIPES; pipeIndex++){
        if (pipeManager->pipes[pipeIndex].fd != -1){
            if (mode == READ && pipeManager->pipes[pipeIndex].inputPID == -1){
                pipeManager->pipes[pipeIndex].inputPID = pid;
                return pipeManager->pipes[pipeIndex].fd;
            } else if (mode == WRITE && pipeManager->pipes[pipeIndex].outputPID == -1){
                pipeManager->pipes[pipeIndex].outputPID = pid;
                return pipeManager->pipes[pipeIndex].fd;

            }
        }
    }
    pipeIndex = -1; 
    for(int i = 0 ; pipeIndex < MAX_PIPES; pipeIndex++, i++){

       if(pipeManager->pipes[i].fd == -1)
        {
            pipeIndex = i; 
            break;
        }
    }

    if (pipeIndex == -1)
    {
        printf("Error: No se pudo abrir el pipe, todos los pipes estan ocupados.\n");
        return -1; 
    }

    Pipe newPipe = createPipe();
    newPipe.fd = pipeIndex;
    if (mode == READ){
        newPipe.inputPID = pid;
    } else if (mode == WRITE){
        newPipe.outputPID = pid;
    } else {
        return -1; 
    }

    pipeManager->pipes[pipeIndex] = newPipe;
    pipeManager->pipeCount++;
    return pipeIndex;
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
    if (fd >= MAX_PIPES || size > PIPE_SIZE || pipeManager->pipes[fd].writeLock || pipeManager->pipes[fd].size == PIPE_SIZE)
    {
        return 0;
    }

    uint8_t written = 0;
    for (int i = 0; i < size; i++)
    {
        if (pipeManager->pipes[fd].size == PIPE_SIZE){
            pipeManager->pipes[fd].writeLock = 1;
            blockProcess(pipeManager->pipes[fd].outputPID);
            break; 
        }
        pipeManager->pipes[fd].buffer[pipeManager->pipes[fd].writeIndex] = buffer[i];
        pipeManager->pipes[fd].writeIndex = (pipeManager->pipes[fd].writeIndex + 1) % PIPE_SIZE;
        pipeManager->pipes[fd].size++;
        written++;

        if (pipeManager->pipes[fd].readLock)
        {
            pipeManager->pipes[fd].readLock = 0;
            unblockProcess(pipeManager->pipes[fd].inputPID);
        }
    }
    pipeManager->pipes[fd].writeLock = 0;
    return written;
}


uint8_t readPipe(uint8_t fd, char *buffer, uint8_t size)
{
    if (fd >= MAX_PIPES || size > PIPE_SIZE || pipeManager->pipes[fd].readLock || pipeManager->pipes[fd].size == 0)
    {
        return 0;
    }

    uint8_t bytesToRead = size < pipeManager->pipes[fd].size ? size : pipeManager->pipes[fd].size;

    for (int i = 0; i < bytesToRead; i++)
    {
        if( pipeManager->pipes[fd].size == 0){
            pipeManager->pipes[fd].readLock = 1;
            blockProcess(pipeManager->pipes[fd].inputPID);
            break; 
        }

        buffer[i] = pipeManager->pipes[fd].buffer[pipeManager->pipes[fd].readIndex];
        pipeManager->pipes[fd].readIndex = (pipeManager->pipes[fd].readIndex + 1) % PIPE_SIZE;
        pipeManager->pipes[fd].size--;

        if (pipeManager->pipes[fd].writeLock)
        {
            pipeManager->pipes[fd].writeLock = 0;
            unblockProcess(pipeManager->pipes[fd].outputPID);
        }
    }
    pipeManager->pipes[fd].readLock = 0;
    return bytesToRead;
}