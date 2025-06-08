#include <processes.h>


#define TICKS 0.055

#define ES_VOCAL(n) ((n) == 'a' || (n) == 'e' || (n) == 'i' || (n) == 'o' || (n) == 'u' || \
                            (n) == 'A' || (n) == 'E' || (n) == 'I' || (n) == 'O' || (n) == 'U')



static void printQuantityBars(uint64_t total, uint64_t consumed); 

void mem(int argc, char *argv[])
{
    if (argc != 1)
    {
        printErr("Uso: mem\n");
        return;
    }

    uint64_t usedMemory = getUsedMemory();
    uint64_t freeMemory = getFreeMemory();
    uint64_t totalMemory = usedMemory + freeMemory;

    char *memoryType = getMemoryType();
    
    printf("Tipo de memoria: %s\n", memoryType);

    if(strcmp(memoryType, "Bitmap") == 0)
    {
        printf("Bloques totales: %d \n", totalMemory);
        printQuantityBars(totalMemory, totalMemory);
        printf("Bloques libres: %d \n", freeMemory);
        printQuantityBars(freeMemory, totalMemory);
        printf("Bloques usados: %d \n", usedMemory);
        printQuantityBars(totalMemory, usedMemory);
    } else{
        printf("Memoria total: %d bytes\n", totalMemory);
        printQuantityBars(totalMemory, totalMemory);
        printf("Memoria libre: %d bytes\n", freeMemory);
        printQuantityBars(totalMemory, freeMemory);
        printf("Memoria usada: %d bytes\n", usedMemory);
        printQuantityBars(totalMemory, usedMemory);
    }
   
  
}

static void printQuantityBars(uint64_t total, uint64_t consumed){
    int percentage = 0;
    if (total > 0) {
        percentage = (int)(((uint64_t)consumed * 100) / total);
    }

    
    if (percentage < 0) {
        percentage = 0;
    }
    if (percentage > 100) {
        percentage = 100;
    }

    int bars = percentage ; 

    putchar('[');
    for (int i = 0; i < bars; i++)
    {
        putchar('=');
    }
    for (int i = bars; i < 100; i++) 
    {
        putchar(' ');
    }
    putchar(']');
    printf(" %d", percentage); 
    putchar('%');
    putchar('\n');
}

void ps(int argc, char *argv[])
{
    if (argc != 1)
    {
        printErr("Uso: ps\n");
        return;
    }
    char *status[] = {"BLOCKED", "READY", "RUNNING", "ZOMBIEE", "DEAD"};
    char * foreground[2] = { "BACKGROUND", "FOREGROUND"};

    int size=0;
    ProcessData * processes = processInfo(&size); // Llamamos a la funcion que obtiene la informacion de los procesos


    printf("PID Nombre Prioridad Estado         Plano          Stack\n");
    for (int i = 0; i < size; i++)
    {
        printf("%d   %s  %d         %s        %s     %d\n", processes[i].pid, processes[i].name, processes[i].priority, status[processes[i].status], foreground[processes[i].foreground],processes[i].stack);
    }

    free(processes); // Liberamos la memoria asignada para los procesos
}

void kill(int argc, char *argv[])
{
    if (argc != 2)
    {
        printErr("Uso: kill <pid>\n");
        return;
    }
    int pid = atoi(argv[1]);
    if (pid == 0){
        printErr("Error: No se puede matar la shell.\n");
        return;
    }
    int result = killProcess(pid);
    switch (result)
    {
    case 0:
        printf("Proceso %d eliminado exitosamente.\n", atoi(argv[1]));
        break;
    case -1:
        printErr("Error: El proceso no existe.\n");
        break;
    case -2:
        printErr("Error: No se puede matar el proceso.\n");
        break;
    default:
        printErr("Error desconocido al matar el proceso.\n");
    }
}

void filter(int argc, char *argv[]) {
    int c;
    int prevWasNewline = 0;
    while (1) {
        c = getchar();
        if (c == 0)
            break; 
        if (!ES_VOCAL(c))
            putchar(c);
        if (c == '\n') {
            if (prevWasNewline)
                break;
            prevWasNewline = 1;
        } else {
            prevWasNewline = 0;
        }
    }
    putchar('\n');
}

int cat(int argc, char *argv[]) {
    int c;
    int prevWasNewline = 0;
    while (1) {
        c = getchar();
        if( c == 0)
            continue;; 
        putchar(c);
        if (c == '\n') {
            if (prevWasNewline)
                break;
            prevWasNewline = 1;
        } else {
            prevWasNewline = 0;
        }
    }
    return 0;
}


int wc(int argc, char *argv[]) {
    int c;
    int lineCounter = 0;
    int prevWasNewline = 0;
    while (1) {
        c = getchar();
        if (c == 0)
            continue; 
        if (c == '\n') {
            if (prevWasNewline)
                break;
            lineCounter++;
            prevWasNewline = 1;
        } else {
            prevWasNewline = 0;
        }
    }
    printf("La cantidad de lineas: %d\n", lineCounter);
    return 0;
}


void loop(int argc, char *argv[]) {

	if (argc != 2) {
		printf("You must insert ONE parameter indicating the number of seconds you desire to test\n");
		return;
	}
	int secs = atoi(argv[1]);

	if (secs < 0) {
		printf("Number of seconds must be greater than 0\n");
		return;
	}
	int realTime = secs / TICKS;

	while (1) {
		printf("Hello World! PID: %d\n", getPid());
		wait_time(realTime);
	}

    
	return;
}


int niceWrapper(int argc, char *argv[])
{
    if (argc != 3)
    {
        printErr(WRONG_PARAMS);
        puts("Uso: nice <pid> <prioridad>");
        return -1;
    }

    int result = setPriority(atoi(argv[1]), atoi(argv[2]));
    switch (result)
    {
    case 0:
        printf("Prioridad del proceso %d cambiada a %d exitosamente.\n", atoi(argv[1]), atoi(argv[2]));
        break;
    case -1:
        printErr("Error: El proceso no existe.\n");
        break;
    case -2:
        printErr("Error: No se puede cambiar la prioridad del proceso.\n");
        break;
    default:
        printErr("Error desconocido al cambiar la prioridad del proceso.\n");
    }
    return result;
}


void blockProcessWrapper(int argc, char *argv[])
{
    if (argc != 2)
    {
        printErr("Uso: block <pid>\n");
        return;
    }
    printf("Bloqueando proceso %s...\n", argv[1]);
    int result = blockProcess(atoi(argv[1]));
    switch (result)
    {
    case 0:
        printf("Proceso %d bloqueado exitosamente.\n", atoi(argv[1]));
        break;
    case -1:
        printErr("Error: El proceso no existe.\n");
        break;
    case -2:
        printErr("Error: No se puede bloquear el proceso.\n");
        break;
    default:
        printErr("Error desconocido al bloquear el proceso.\n");
    }
}

void unblockProcessWrapper(int argc, char *argv[])
{
    if (argc != 2)
    {
        printErr("Uso: unblock <pid>\n");
        return;
    }
    int result = unblockProcess(atoi(argv[1]));
    switch (result)
    {
    case 0:
        printf("Proceso %d desbloqueado exitosamente.\n", atoi(argv[1]));
        break;
    case -1:
        printErr("Error: El proceso no existe.\n");
        break;
    case -2:
        printErr("Error: No se puede desbloquear el proceso.\n");
        break;
    default:
        printErr("Error desconocido al desbloquear el proceso.\n");
    }
}
