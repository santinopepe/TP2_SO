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
        return -1; 
    }

    Pipe newPipe = createPipe();
    newPipe.fd = pipeIndex + 3; // Asignamos un fd a partir de 3 (0, 1, 2 son stdin, stdout, stderr)
    if (mode == READ){
        newPipe.inputPID = pid;
    } else if (mode == WRITE){
        newPipe.outputPID = pid;
    } else {
        return -1; 
    }

    pipeManager->pipes[pipeIndex] = newPipe;
    pipeManager->pipeCount++;
    return pipeManager->pipes[pipeIndex].fd;
}

uint8_t closePipe(uint8_t fd)
{

    if (fd > MAX_PIPES || pipeManager->pipes[fd - 3].fd == -1)
    {
        return 0;
    }

    pipeManager->pipes[fd - 3].fd = -1;
    pipeManager->pipes[fd - 3].inputPID = -1;
    pipeManager->pipes[fd - 3].outputPID = -1;
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
        while(pipeManager->pipes[fd - 3].size == PIPE_SIZE){
            pipeManager->pipes[fd -3].writeLock = 1;
            blockProcess(pipeManager->pipes[fd -3].outputPID);
            yield();     
        }
        pipeManager->pipes[fd-3].buffer[pipeManager->pipes[fd-3].writeIndex] = buffer[i];
        pipeManager->pipes[fd-3].writeIndex = (pipeManager->pipes[fd-3].writeIndex + 1) % PIPE_SIZE;
        pipeManager->pipes[fd-3].size++;
        written++;

        if (pipeManager->pipes[fd-3].readLock)
        {
            pipeManager->pipes[fd-3].readLock = 0;
            unblockProcess(pipeManager->pipes[fd-3].inputPID);
        }
    }
    pipeManager->pipes[fd-3].writeLock = 0;
    return written;
}


uint8_t readPipe(uint8_t fd, char *buffer, uint8_t size)
{
    if (fd-3 >= MAX_PIPES || size > PIPE_SIZE || pipeManager->pipes[fd-3].readLock || pipeManager->pipes[fd-3].size == 0)
    {
        return 0;
    }

    uint8_t bytesToRead = size < pipeManager->pipes[fd-3].size ? size : pipeManager->pipes[fd-3].size;

    if( bytesToRead == 0)
    {
        blockProcess(pipeManager->pipes[fd-3].inputPID);
        pipeManager->pipes[fd-3].readLock = 1;
        yield();
        return 0; // No hay datos para leer, bloquea el proceso de lectura
    }

    for (int i = 0; i < bytesToRead; i++)
    {
        while( pipeManager->pipes[fd-3].size == 0){
            pipeManager->pipes[fd-3].readLock = 1;
            blockProcess(pipeManager->pipes[fd-3].inputPID);
            yield(); 
        }

        buffer[i] = pipeManager->pipes[fd-3].buffer[pipeManager->pipes[fd-3].readIndex];
        pipeManager->pipes[fd-3].readIndex = (pipeManager->pipes[fd-3].readIndex + 1) % PIPE_SIZE;
        pipeManager->pipes[fd-3].size--;

        if (pipeManager->pipes[fd-3].writeLock)
        {
            pipeManager->pipes[fd-3].writeLock = 0;
            unblockProcess(pipeManager->pipes[fd-3].outputPID);
        }
    }
    pipeManager->pipes[fd-3].readLock = 0;
    return bytesToRead;
}

int killPipedProcesses() {
    uint8_t pid = getPid();
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipeManager->pipes[i].inputPID == pid || pipeManager->pipes[i].outputPID == pid) {
            
            killProcess(pipeManager->pipes[i].inputPID);
            killProcess(pipeManager->pipes[i].outputPID);
            closePipe(pipeManager->pipes[i].fd);
            pipeManager->pipes[i] = createPipe(); // Reinicializa el pipe
            pipeManager->pipeCount--;
            return 0; // Mata el proceso actual y sale de la función
        }
    }
    return killForegroundProcess();
}
