// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    if (fd-3 >= MAX_PIPES || size > PIPE_SIZE || pipeManager->pipes[fd-3].writeLock || pipeManager->pipes[fd-3].size == PIPE_SIZE)
    {
        return 0;
    }

    Pipe *pipe = &pipeManager->pipes[fd-3];
    if (pipe->fd == -1 || pipe->outputPID == -1)
        return 0; // Pipe cerrado o sin escritor

    uint8_t written = 0;
    for (int i = 0; i < size; i++) {
        // Si el buffer está lleno, bloquear escritor
        while (pipe->size == PIPE_SIZE) {
            pipe->writeLock = 1;
            blockProcess(pipe->outputPID);
            yield();
            // Si el pipe fue cerrado mientras estaba bloqueado, abortar
            if (pipe->fd == -1 || pipe->outputPID == -1)
                return written;
        }
        pipe->buffer[pipe->writeIndex] = buffer[i];
        pipe->writeIndex = (pipe->writeIndex + 1) % PIPE_SIZE;
        pipe->size++;
        written++;

        // Si hay un lector bloqueado, desbloquearlo
        if (pipe->readLock) {
            pipe->readLock = 0;
            unblockProcess(pipe->inputPID);
        }
    }
    pipe->writeLock = 0;
    return written;
}

uint8_t readPipe(uint8_t fd, char *buffer, uint8_t size)
{
    if (fd < 3 || fd-3 >= MAX_PIPES || size > PIPE_SIZE)
        return 0;

    Pipe *pipe = &pipeManager->pipes[fd-3];
    if (pipe->fd == -1 || pipe->inputPID == -1)
        return 0; // Pipe cerrado o sin lector

    // Si no hay datos y el escritor cerró el pipe, devolver EOF
    if (pipe->size == 0 && pipe->outputPID == -1) {
        return (uint8_t)-1; // EOF
    }

    // Si no hay datos pero el pipe sigue abierto, bloquear
    while (pipe->size == 0 && pipe->outputPID != -1) {
        pipe->readLock = 1;
        blockProcess(pipe->inputPID);
        yield();
        // Si el pipe fue cerrado mientras estaba bloqueado, devolver EOF
        if (pipe->size == 0 && pipe->outputPID == -1)
            return (uint8_t)-1;
    }

    uint8_t bytesToRead = size < pipe->size ? size : pipe->size;
    for (int i = 0; i < bytesToRead; i++) {
        buffer[i] = pipe->buffer[pipe->readIndex];
        pipe->readIndex = (pipe->readIndex + 1) % PIPE_SIZE;
        pipe->size--;

        // Si hay un escritor bloqueado, desbloquearlo
        if (pipe->writeLock) {
            pipe->writeLock = 0;
            unblockProcess(pipe->outputPID);
        }
    }
    pipe->readLock = 0;
    return bytesToRead;
}

int killPipedProcesses() {
    uint8_t pid = getPid();
    SchedulerADT scheduler = getSchedulerADT();
    if(strcmp(scheduler->process[pid].name, "phylo")){
                return 0;
    }
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

void closePipeEndsForPID(uint16_t pid) {
    for (int i = 0; i < MAX_PIPES; i++) {
        Pipe *pipe = &pipeManager->pipes[i];
        if (pipe->outputPID == pid) {
            pipe->outputPID = -1;
        }
        if (pipe->inputPID == pid) {
            pipe->inputPID = -1;
        }
        // Solo si ambos extremos están cerrados, liberá el pipe:
        if (pipe->outputPID == -1 && pipe->inputPID == -1 && pipe->fd != -1) {
            closePipe(pipe->fd);
        }
    }
}