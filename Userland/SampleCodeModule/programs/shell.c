#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <syscalls.h>
#include <man.h>
#include <libasm.h>
#include <cmdParserADT.h>
#include <tests.h>
#include <phylo.h>
#include <color.h>
#include <processes.h>





/* Enum para la cantidad de argumentos recibidos */
typedef enum
{
    NO_PARAMS = 0,
    SINGLE_PARAM,
    DUAL_PARAM
} functionType;
#define QTY_BYTES 32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE 1
#define MAX_FONT_SIZE 3
#define BUFFER 256
#define CURSOR_FREQ 100 // Frecuencia del cursor en ticks (ejemplo)
#define MAX_CHARS 256

// FD de la shell
static int current_stdin_fd = STDIN;
static int current_stdout_fd = STDOUT;



typedef void (*CommandFunction)(int argc, char *argv[]); // Definicion de puntero a funcion

typedef struct
{
    char *name;        // Nombre del comando
    char *description; // Descripcion del comando (para help)
    CommandFunction f; // Funcion a ejecutar
} Command;

static void help(int argc, char *argv[]);
static void man(int argc, char *argv[]);
static void printInfoReg();
static void time(int argc, char *argv[]);
static int div(int argc, char *argv[]);
static void fontSize(int argc, char *argv[]);
static void printMem(char *pos);
static int getCommandIndex(char *command);
static void myClear();


static int readLineWithCursor(char *buffer, int max_len);
static void executePipedCommands(CommandADT command);

static void changeColor(int argc, char * argv[]);

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


void run_shell()
{
    puts(WELCOME);

    char inputBuffer[MAX_CHARS];
    while (1)
    {
        putchar('\n');
        putchar('>');

        int charsRead = readLineWithCursor(inputBuffer, MAX_CHARS);

        if (charsRead == 0)
        {
            continue;
        }

        CommandADT command = parseInput(inputBuffer);

        if (command == NULL || getCommandQty(command) == 0)
        {
            printErr(INVALID_COMMAND);
            if (command != NULL)
            {
                freeCommandADT(command);
            }
            continue;
        }
        executePipedCommands(command);

        freeCommandADT(command);
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


static void help(int argc, char *argv[])
{
    for (int i = 0; i < QTY_COMMANDS; i++){
        printf("%s: %s\n", commands[i].name, commands[i].description);
    }

}


static int div(int argc, char *argv[])
{
    if (argc != 3)
    {
        printErr(WRONG_PARAMS);
        return 1;
    }
    printf("%s/%s=%d\r\n", argv[0], argv[1], atoi(argv[0]) / atoi(argv[1]));
    return 1;
}

static void time(int argc, char *argv[])
{
    uint32_t secs = getSeconds();
    uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
    printf("%2d:%2d:%2d\r\n", h, m, s);
}

static void fontSize(int argc, char *argv[])
{
    if (argc != 2)
    {
        printErr(INVALID_FONT_SIZE);
        return;
    }
    int s = atoi(argv[0]);
    if (s >= MIN_FONT_SIZE && s <= MAX_FONT_SIZE)
        setFontSize((uint8_t)atoi(argv[0]));
    else
    {
        printErr(INVALID_FONT_SIZE);
        puts(CHECK_MAN_FONT);
    }
}


static void printMem(char *pos)
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

static char *_regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
static void printInfoReg()
{
    int len = sizeof(_regNames) / sizeof(char *);
    uint64_t regSnapshot[len];
    getInfoReg(regSnapshot);
    for (int i = 0; i < len; i++)
        printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
}

static void man(int argc, char *argv[])
{
    int idx = getCommandIndex(argv[1]);
    if (idx != -1){
        printf("index %d\n", idx);
        printf("%s\n", usages[idx]);
    }else{
        printErr("Comando no encontrado.\n");
    }
        
}

static void myClear()
{
    clear();
}


static int readLineWithCursor(char *buffer, int max_len)
{
    int ticks = getTicks();
    int cursorTicks = 0;
    char cursorDrawn = 0;
    uint64_t bIdx = 0;
    char c;

    while (1)
    {
        // Lógica del cursor parpadeante
        cursorTicks = getTicks() - ticks;
        if (cursorTicks > CURSOR_FREQ)
        {
            ticks = getTicks();
            cursorTicks = 0;
            if (cursorDrawn)
            {
                putchar('\b'); // Borra el cursor
            }
            else
            {
                putchar('_'); // Dibuja el cursor
            }
            cursorDrawn = !cursorDrawn;
        }

        c = getchar(); // Lee un caracter (asumo que es no bloqueante o retorna 0 si no hay entrada)

        if (c != 0)
        { // Si se recibió un caracter real
            if (cursorDrawn)
            { // Si el cursor estaba dibujado, bórralo antes de imprimir el caracter
                putchar('\b');
                cursorDrawn = !cursorDrawn;
            }

            if (c == '\n')
            { // Si es Enter, terminar de leer la línea
                break;
            }
            else if (c == '\b' || c == 0x7F)
            { // Retroceso (0x7F es DEL a veces)
                if (bIdx > 0)
                {
                    bIdx--;
                    putchar('\b'); // Mueve el cursor atrás
                    putchar(' ');  // Borra el caracter visualmente
                    putchar('\b'); // Mueve el cursor atrás otra vez
                }
                else
                {
                }
            }
            else if (bIdx < max_len - 1)
            { // Caracter normal y hay espacio en el buffer
                buffer[bIdx++] = c;
                putchar(c); // Imprime el caracter
            }
            else
            { // Buffer lleno
            }
        }
    }

    if (cursorDrawn)
    { // Si el cursor estaba dibujado al final, bórralo
        putchar('\b');
    }
    putchar('\n');

    buffer[bIdx] = '\0';
    return bIdx;
}


static void executePipedCommands(CommandADT command)
{
    uint8_t shell_original_stdin = current_stdin_fd;
    uint8_t shell_original_stdout = current_stdout_fd;
    int qtyPrograms = getCommandQty(command);

    if (qtyPrograms > 2) {
        printErr("Solo se permite un pipe (dos comandos).\n");
        return;
    }

    if (qtyPrograms == 1) {
        // Sin pipe, ejecuta normalmente
        char *cmdName = getCommandName(command, 0);
        int argc = getCommandArgc(command, 0);
        char **argv = getCommandArgv(command, 0);
        int cmd_index_in_shell = getCommandIndex(cmdName);

        if (cmd_index_in_shell == -1) {
            printErr(INVALID_COMMAND);
            return;
        }


   
            uint8_t is_background = getIsBackground(command, 0);
            uint16_t fileDescriptors[3] = {STDIN, STDOUT, STDERR};
            if(is_background){
                fileDescriptors[0] = 1;
            }
            uint16_t pid = createProcess((EntryPoint)commands[cmd_index_in_shell].f, argv, argc, 0, fileDescriptors);
            if (pid == -1) {
                printErr("Error: No se pudo crear el proceso.\n");
                return;
            }
            if(!is_background) {
                waitForChildren();
            } 
    
    } else if (qtyPrograms == 2) {
         // Un solo pipe permitido
        char *cmdName1 = getCommandName(command, 0);
        int argc1 = getCommandArgc(command, 0);
        char **argv1 = getCommandArgv(command, 0);
        int cmd_index1 = getCommandIndex(cmdName1);

        char *cmdName2 = getCommandName(command, 1);
        int argc2 = getCommandArgc(command, 1);
        char **argv2 = getCommandArgv(command, 1);
        int cmd_index2 = getCommandIndex(cmdName2);

        if (cmd_index1 == -1 || cmd_index2 == -1) {
            printErr(INVALID_COMMAND);
            return;
        }

        uint16_t fileDescriptors1[3];
        uint16_t fileDescriptors2[3];
        uint16_t pid1, pid2;

        fileDescriptors1[0] = shell_original_stdin; // cat reads from shell's original stdin
        fileDescriptors1[1] = 0;          // cat's stdout IS the pipe
        fileDescriptors1[2] = STDERR;


        // 1. Creamos el primer proceso (escritor)
        pid1 = createProcess((EntryPoint)commands[cmd_index1].f, argv1, argc1, 0, fileDescriptors1);
        // 2. Abrimos el pipe para el escritor
        if ( ( fileDescriptors1[1] = openPipe(pid1, 1 )) == -1) {
            printErr("Error: Pipe setup failed for writer\n");
            killProcess(pid1);
            return;
        }
        changeFDS(pid1, fileDescriptors1); // Cambiamos el stdin de la shell al pipe

        fileDescriptors2[0] = 0;          // filter's stdin IS the pipe
        fileDescriptors2[1] = shell_original_stdout;// filter writes to shell's original stdout
        fileDescriptors2[2] = STDERR;

        // 3. Creamos el segundo proceso (lector)
        pid2 = createProcess((EntryPoint)commands[cmd_index2].f, argv2, argc2, 0, fileDescriptors2);

        if (( fileDescriptors2[0] = openPipe(pid2, 0 )) == -1) {
            printErr("Error: Pipe setup failed for reader\n");
            killProcess(pid1);
            killProcess(pid2);
            closePipe(fileDescriptors1[1]); // Cerramos el pipe del escritor
            return;
        }

        changeFDS(pid2, fileDescriptors2); // Cambiamos el stdout de la shell al pipe
        waitForChildren();
        closePipe(fileDescriptors1[1]); // Cerramos el pipe del escritor
      
       
    }

    current_stdin_fd = shell_original_stdin;
    current_stdout_fd = shell_original_stdout;
}



static void changeColor(int argc, char * argv[]) {
    if(argc != 2){
        printErr("Uso: changeColor <codigo de color>\n");
        return;
    }
    int color = atoi(argv[1]);
    if(color < 0 || color > 5){
        printErr("Color no valido. Debe ser un numero entre 0 y 5.\n");
        return;
    }
    Color colorCode[6] = { LIGHT_GREEN, DARK_GREEN, PINK, MAGENTA, SILVER, RED };
    Color chosenColor = colorCode[color];
    int c;
    while ((c = getchar()) != EOF) { // Termina al recibir EOF
        if (c == 0)
            continue; 
        printfc(chosenColor, "%c", c);
    }
}
