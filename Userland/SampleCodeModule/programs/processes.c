// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <processes.h>
#include <string.h>
#include <stdlib.h>
#include <tests.h>
#include <phylo.h>
#include <man.h>


#define QTY_BYTES 32 
#define TICKS 0.055
#define MIN_FONT_SIZE 1
#define MAX_FONT_SIZE 3

#define ES_VOCAL(n) ((n) == 'a' || (n) == 'e' || (n) == 'i' || (n) == 'o' || (n) == 'u' || \
                            (n) == 'A' || (n) == 'E' || (n) == 'I' || (n) == 'O' || (n) == 'U')



static void printQuantityBars(uint64_t total, uint64_t consumed); 
static int numDigits(int n);
static void printSpaces(int n);
static int getCommandIndex(char *command); 


static Command commands[] = { 
    {"help", "Listado de comandos", (CommandFunction)help},
    {"man", "Manual de uso de los comandos", (CommandFunction)man},
    {"inforeg", "Informacion de los registos que fueron capturados en un momento arbitrario de ejecucion del sistema", (CommandFunction)printInfoReg},
    {"time", "Despliega la hora actual UTC - 3", (CommandFunction)time},
    {"div", "Hace la division entera de dos numeros naturales enviados por parametro Uso: div <numerador> <denominador>", (CommandFunction)div},
    {"kaboom", "Ejecuta una excepcion de Invalid Opcode", (CommandFunction)kaboom},
    {"font-size", "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero", (CommandFunction)fontSize},
    {"printmem", "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en formato hexadecimal enviada por parametro", (CommandFunction)printMem},
    {"clear", "Limpia toda la pantalla", (CommandFunction)myClear},
    {"nice", "Cambia la prioridad de un proceso. Uso: nice <pid> <prioridad>", (CommandFunction)niceWrapper},
    {"block", "Bloquea un proceso. Uso: block <pid>", (CommandFunction)blockProcessWrapper},
    {"unblock", "Desbloquea un proceso. Uso: unblock <pid>", (CommandFunction)unblockProcessWrapper},
    {"kill", "Elimina un proceso. Uso: kill <pid>", (CommandFunction)kill},
    {"test-processes", "Ejecuta un test de procesos. Uso: test-processes <cantidad>", (CommandFunction)test_processes},
    {"test-priority", "Ejecuta un test de prioridades. Uso: test-priority", (CommandFunction)test_prio},
    {"test-mm", "Ejecuta un test de memoria. Uso: test-mm <max_memory>", (CommandFunction)test_mm},
    {"test-sync", "Ejectua un test de sincronizacion. Uso; test-sync <sem>", (CommandFunction)test_sync},
    {"mem", "Muestra informacion de la memoria del sistema", (CommandFunction)mem},
    {"wc", "Cuenta la cantidad de lineas del input", (CommandFunction)wc},
    {"filter", "Filtra las vocales del input", (CommandFunction)filter},
    {"cat", "Imprime el STDIN tal como lo recibe", (CommandFunction)cat},
    {"phylo", "Ejecuta la simulacion del problema de los filosofos comensales", (CommandFunction)phylo},
    {"ps", "Muestra la informacion de los procesos vivos", (CommandFunction)ps},
    {"loop", "Imprime su ID con un saludo cada una determinada cantidad de segundos. Uso: loop <cantidad de segundos>", (CommandFunction)loop},
    {"color", "Cambia el color de la fuente. Uso: color <color>", (CommandFunction)changeColor},
    
};



void mem(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Uso: mem\n");
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
        printf("Uso: ps\n");
        return;
    }
    char *status[] = {"BLOCKED", "READY", "RUNNING", "ZOMBIEE", "DEAD"};
    char *foreground[2] = { "BACKGROUND", "FOREGROUND"};

    int size = 0;
    ProcessData *processes = processInfo(&size);

    // Encabezado alineado

    printf("PID  Nombre    Prioridad     Estado      Plano        Stack\n");
    for (int i = 0; i < size; i++){
        printf("%d", processes[i].pid);
        printSpaces(5 - numDigits(processes[i].pid)); 

        printf("%s", processes[i].name);
        printSpaces(10 - strlen(processes[i].name)); 

        int prioSpaces = 9 - numDigits(processes[i].priority);
        int left = prioSpaces / 2;
        int right = prioSpaces - left;
        printSpaces(left);
        printf("%d", processes[i].priority);
        printSpaces(right+4);

        printf("%s", status[processes[i].status]);
        printSpaces(11 - strlen(status[processes[i].status])); 

        printf("%s", foreground[processes[i].foreground]);
        printSpaces(13 - strlen(foreground[processes[i].foreground])); 

        printf("%d\n", processes[i].stack);
    }

    free(processes);
}

// Helper para contar dígitos de un número
static int numDigits(int n) {
    int digits = 1;
    while (n /= 10) digits++;
    return digits;
}

static void printSpaces(int n) {
    for (int i = 0; i < n; i++)
        putchar(' ');
}

void kill(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: kill <pid>\n");
        return;
    }
    int pid = atoi(argv[1]);
    if (pid == 0){
        printf("Error: No se puede matar la shell.\n");
        return;
    }
    int result = killProcess(pid);
    switch (result)
    {
    case 0:
        printf("Proceso %d eliminado exitosamente.\n", atoi(argv[1]));
        break;
    case -1:
        printf("Error: El proceso no existe.\n");
        break;
    case -2:
        printf("Error: No se puede matar el proceso.\n");
        break;
    default:
        printf("Error desconocido al matar el proceso.\n");
    }
}

void filter(int argc, char *argv[]) {
    int c;
    while ((c = getchar()) != -1) { // Leer hasta EOF
        if (c == 0)
            continue; 
        if (!ES_VOCAL(c))
            putchar(c);
    }

    
}

int cat(int argc, char *argv[]) {
    int c;
    while ((c = getchar()) != EOF) { // Leer hasta EOF
        if (c == 0)
            continue;
        putchar(c);
    }
    if (c == EOF) {
        putchar('\n'); 
    }
    return 0;
}


int wc(int argc, char *argv[]) {
    int c;
    int lineCounter = 0;
    while ((c = getchar()) != -1) { // Leer hasta EOF
        if (c == 0)
            continue; 
        if (c == '\n') {
            lineCounter++;
        }
    }
    printf("Cantidad de lineas: %d\n", lineCounter);
    return 0;
}


void loop(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Debes insertar UN parámetro indicando el número de segundos que deseas probar\n");
		return;
	}
	int secs = atoi(argv[1]);

	if (secs < 0) {
		printf("El numero de segundos debe ser mayor a cero\n");
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
        printf(WRONG_PARAMS);
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
        printf("Error: El proceso no existe.\n");
        break;
    case -2:
        printf("Error: No se puede cambiar la prioridad del proceso.\n");
        break;
    default:
        printf("Error desconocido al cambiar la prioridad del proceso.\n");
    }
    return result;
}


void blockProcessWrapper(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: block <pid>\n");
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
        printf("Error: El proceso no existe.\n");
        break;
    case -2:
        printf("Error: No se puede bloquear el proceso.\n");
        break;
    default:
        printf("Error desconocido al bloquear el proceso.\n");
    }
}

void unblockProcessWrapper(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: unblock <pid>\n");
        return;
    }
    int result = unblockProcess(atoi(argv[1]));
    switch (result)
    {
    case 0:
        printf("Proceso %d desbloqueado exitosamente.\n", atoi(argv[1]));
        break;
    case -1:
        printf("Error: El proceso no existe.\n");
        break;
    case -2:
        printf("Error: No se puede desbloquear el proceso.\n");
        break;
    default:
        printf("Error desconocido al desbloquear el proceso.\n");
    }
}

void changeColor(int argc, char * argv[]) {
    if(argc != 2){
        printf("Uso: changeColor <codigo de color>\n");
        return;
    }
    int color = atoi(argv[1]);
    if(color < 0 || color > 5){
        printf("Color no valido. Debe ser un numero entre 0 y 5.\n");
        return;
    }
    Color colorCode[6] = { LIGHT_GREEN, DARK_GREEN, PINK, MAGENTA, SILVER, RED };
    Color chosenColor = colorCode[color];
    int c;
    while ((c = getchar()) != EOF) { 
        if (c == 0)
            continue; 
        printfc(chosenColor, "%c", c);
    }
}

static char *_regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
void printInfoReg()
{
    int len = sizeof(_regNames) / sizeof(char *);
    uint64_t regSnapshot[len];
    getInfoReg(regSnapshot);
    for (int i = 0; i < len; i++)
        printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
}

void man(int argc, char *argv[])
{
    int idx = getCommandIndex(argv[1]);
    if (idx != -1){
        printf("index %d\n", idx);
        printf("%s\n", usages[idx]);
    }else{
        printf("Comando no encontrado.\n");
    }
        
}
void printMem(char *pos)
{
    uint8_t resp[QTY_BYTES];
    char *end;
    getMemory(strtoh(pos, &end), resp);
    for (int i = 0; i < QTY_BYTES; i++)
    {
        printf("0x%2x ", resp[i]);
        if (i % 4 == 3)
            putchar('\n');
    }
}

int div(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf(WRONG_PARAMS);
        return 1;
    }
    printf("%s/%s=%d\r\n", argv[0], argv[1], atoi(argv[0]) / atoi(argv[1]));
    return 1;
}

void time(int argc, char *argv[])
{
    uint32_t secs = getSeconds();
    uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
    printf("%2d:%2d:%2d\r\n", h, m, s);
}

 void fontSize(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf(INVALID_FONT_SIZE);
        return;
    }
    int s = atoi(argv[0]);
    if (s >= MIN_FONT_SIZE && s <= MAX_FONT_SIZE)
        setFontSize((uint8_t)atoi(argv[0]));
    else
    {
        printf(INVALID_FONT_SIZE);
        puts(CHECK_MAN_FONT);
    }
}

/**
 * @brief  Devuelve el indice del vector de comandos dado su nombre
 * @param  command: Nombre del comando a buscar
 * @return  Indice del comando
 */

static int getCommandIndex(char *command)
{
    for (int idx = 0; idx < QTY_COMMANDS; idx++)
    {
        if (!strcmp(commands[idx].name, command))
            return idx;
    }
    return -1;
}

void help(int argc, char *argv[])
{
    for (int i = 0; i < QTY_COMMANDS; i++){
        printf("%s: %s\n", commands[i].name, commands[i].description);
    }

}

void myClear()
{
    clear();
}
