#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <stdint.h>
#include <syscalls.h>
#include <tron.h>
#include <man.h>
#include <stddef.h>
#include <libasm.h>
#include <cmdParserADT.h>

/* Enum para la cantidad de argumentos recibidos */
typedef enum {NO_PARAMS = 0, SINGLE_PARAM, DUAL_PARAM} functionType;    
#define QTY_BYTES 32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE 1
#define MAX_FONT_SIZE 3
#define BUFFER 256
#define CURSOR_FREQ 100 // Frecuencia del cursor en ticks (ejemplo)
#define MAX_CHARS 256

//FD de la shell 
static int current_stdin_fd = 0;
static int current_stdout_fd = 1;



#define WELCOME "Bienvenido a Cactiland OS!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT "Escriba \"man font-size\" para ver las dimensiones validas\n"

typedef void (*CommandFunction)(int argc, char *argv[]); // Definicion de puntero a funcion

typedef struct {
    char * name;                    // Nombre del comando
    char * description;             // Descripcion del comando (para help)
    CommandFunction f;            // Funcion a ejecutar
} Command;

static void help();
static void man(char * command);
static void printInfoReg();
static void time();
static int div(char * num, char * div);
static void tron();
static void tronZen();
static void fontSize(int argc, char *argv[]);
static void printMem(char * pos);
static int getCommandIndex(char * command);
static void myClear();
static void kill(uint16_t pid); 
static int setPriorityWrapper(uint16_t pid, uint8_t priority);
static int setStatusWrapper(uint16_t pid, ProcessStatus status);

static int readLineWithCursor(char *buffer, int max_len); 
static void executePipedCommands(CommandADT command); 

static Command commands[] = {
{"help", "Listado de comandos", (CommandFunction) help},
{"man", "Manual de uso de los comandos", (CommandFunction) man},
{"inforeg", "Informacion de los registos que fueron capturados en un momento arbitrario de ejecucion del sistema", (CommandFunction) printInfoReg},
{"time", "Despliega la hora actual UTC - 3", (CommandFunction) time},
{"div", "Hace la division entera de dos numeros naturales enviados por parametro", (CommandFunction) div},
{"kaboom", "Ejecuta una excepcion de Invalid Opcode", (CommandFunction) kaboom},
{"tron", "Juego Tron Light Cycles", (CommandFunction) tron},
{"tron-zen", "Juego Tron Light Cycles con un unico jugador", (CommandFunction) tronZen},
{"font-size", "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero", (CommandFunction) fontSize},
{"printmem", "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en formato hexadecimal enviada por parametro", (CommandFunction) printMem},
{"clear", "Limpia toda la pantalla", (CommandFunction) myClear},
{"set-priority", "Cambia la prioridad de un proceso. Uso: set-priority <pid> <prioridad>", (CommandFunction) setPriorityWrapper},
{"set-status", "Cambia el estado de un proceso. Uso: set-status <pid> <estado>", (CommandFunction) setStatusWrapper},
{"block", "Bloquea un proceso. Uso: block <pid>", (CommandFunction) blockProcess},
{"unblock", "Desbloquea un proceso. Uso: unblock <pid>", (CommandFunction) unblockProcess},
{"kill", "Elimina un proceso. Uso: kill <pid>", (CommandFunction) kill}


};



void run_shell() {
    int index;
    puts(WELCOME);

    char inputBuffer[MAX_CHARS]; 
    while(1){
        putchar('>');
        
        int charsRead = readLineWithCursor(inputBuffer, MAX_CHARS);

        if (charsRead == 0) { 
            continue;
        }

        CommandADT command = parseInput(inputBuffer);

        if (command == NULL || getCommandQty(command) == 0){
            printErr(INVALID_COMMAND);
            if (command != NULL) {
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

static int getCommandIndex(char * command) {
    int idx = 0;
    for(; idx < QTY_COMMANDS; idx++){
        if(!strcmp(commands[idx].name, command))
            return idx;
    }    
    return -1;
}

static void pipeCommands(int argc, char *argv[]){
    if (argc != 2){
        printErr(WRONG_PARAMS);
        return;
    }

}

static void help(int argc, char *argv[]) {
    for (int i = 0; i < QTY_COMMANDS; i++)
        printf("%s: %s\r\n", commands[i].name, commands[i].description);
}

static int div(char * num, char * div) {
    printf("%s/%s=%d\r\n", num, div, atoi(num)/atoi(div));
    return 1;
}

static void time(int argc, char *argv[]){
    uint32_t secs = getSeconds();
    uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
    printf("%2d:%2d:%2d\r\n", h, m, s);
}

static void fontSize(int argc, char *argv[]) {
    if (argc != 2){
        printErr(INVALID_FONT_SIZE);
        return;
    }
    int s = atoi(argv[1]);
    if(s >= MIN_FONT_SIZE && s <= MAX_FONT_SIZE)
        setFontSize((uint8_t)atoi(argv[1]));
    else{
        printErr(INVALID_FONT_SIZE);
        puts(CHECK_MAN_FONT);
    }
}

static void tron(){
    setFontSize(2);
    startTron(2);
    setFontSize(1);
}

static void tronZen(){
    setFontSize(2);
    startTron(1);
    setFontSize(1);
}


static void printMem(char * pos){
    uint8_t resp[QTY_BYTES];
    char * end;
    getMemory(strtoh(pos, &end), resp);
    for(int i = 0; i < QTY_BYTES; i++) {
        printf("0x%2x ", resp[i]);
        if (i % 4 == 3)
            putchar('\n');
    }
}

static char * _regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
static void printInfoReg() {
    int len = sizeof(_regNames)/sizeof(char *);
    uint64_t regSnapshot[len];
    getInfoReg(regSnapshot);
    for (int i = 0; i < len; i++)
        printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
    
}

static void man(char * command){
    int idx = getCommandIndex(command);
    if (idx != -1)
        printf("%s\n", usages[idx]);
    else
        printErr(INVALID_COMMAND);
}

static void myClear(){
    clear();
}

static void kill(uint16_t pid) {
    int ans = killProcess(pid);
    switch (ans) {
        case 0:
            printf("El proceso ha sido eliminado exitosamente\n");
            break;
        case -1:
            printErr("No existe el proceso\n");
            break;
        case -2:
            printErr("No se puede matar el proceso\n");
            break;
        default:
            printErr("Error desconocido al tratar de matar el proceso.\n");
            break;
    }
}


static int setPriorityWrapper(uint16_t pid, uint8_t priority) {
    int result = setPriority(pid, priority);
    switch (result) {
        case 0:
            printf("Prioridad del proceso %u cambiada a %u exitosamente.\n", pid, priority);
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

static int setStatusWrapper(uint16_t pid, ProcessStatus status) {
    int result = setStatus(pid, status);
    switch (result) {
        case 0:
            printf("Estado del proceso %u cambiado exitosamente.\n", pid);
            break;
        case -1:
            printErr("Error: El proceso no existe.\n");
            break;
        case -2:
            printErr("Error: No se puede cambiar el estado del proceso.\n");
            break;
        default:
            printErr("Error desconocido al cambiar el estado del proceso.\n");
    }
    return result;
}



static int readLineWithCursor(char *buffer, int max_len) {
    int ticks = getTicks();
    int cursorTicks = 0;
    char cursorDrawn = 0;
    uint64_t bIdx = 0;
    char c;

    while (1) {
        // Lógica del cursor parpadeante
        cursorTicks = getTicks() - ticks;
        if (cursorTicks > CURSOR_FREQ) {
            ticks = getTicks();
            cursorTicks = 0;
            if (cursorDrawn) {
                putchar('\b'); // Borra el cursor
            } else {
                putchar('_');  // Dibuja el cursor
            }
            cursorDrawn = !cursorDrawn;
        }

        c = getchar(); // Lee un caracter (asumo que es no bloqueante o retorna 0 si no hay entrada)

        if (c != 0) { // Si se recibió un caracter real
            if (cursorDrawn) { // Si el cursor estaba dibujado, bórralo antes de imprimir el caracter
                putchar('\b');
                cursorDrawn = !cursorDrawn;
            }

            if (c == '\n') { // Si es Enter, terminar de leer la línea
                break;
            } else if (c == '\b' || c == 0x7F) { // Retroceso (0x7F es DEL a veces)
                if (bIdx > 0) {
                    bIdx--;
                    putchar('\b'); // Mueve el cursor atrás
                    putchar(' ');  // Borra el caracter visualmente
                    putchar('\b'); // Mueve el cursor atrás otra vez
                } else {
                }
            } else if (bIdx < max_len - 1) { // Caracter normal y hay espacio en el buffer
                buffer[bIdx++] = c;
                putchar(c); // Imprime el caracter
            } else { // Buffer lleno
            }
        }
    }

    if (cursorDrawn) { // Si el cursor estaba dibujado al final, bórralo
        putchar('\b');
    }
    putchar('\n');

    buffer[bIdx] = '\0'; 
    return bIdx; 
    
}






static void executePipedCommands(CommandADT command) {
    uint8_t shell_original_stdin = current_stdin_fd;
    uint8_t shell_original_stdout = current_stdout_fd;

    int qtyPrograms = getCommandQty(command);
    int error_occurred = 0; 

    for (int i = 0; i < qtyPrograms; i++) {
        if (error_occurred) {

            for (int j = 0; j < i; j++) {
                uint8_t fd_to_close = getCommandStdout(command, j);
                if (fd_to_close != shell_original_stdout && fd_to_close != 0xFF) {
                    closePipe(fd_to_close);
                }
            }
            break; 
        }

        char *cmdName = getCommandName(command, i);
        int argc = getCommandArgc(command, i);
        char **argv = getCommandArgv(command, i);

        if (i == 0) { 
            setCommandStdin(command, i, shell_original_stdin);
        } else { 
            setCommandStdin(command, i, getCommandStdout(command, i - 1));
        }

        if (i < qtyPrograms - 1) { 
            uint8_t pipe_fd = openPipe(0, 0); 
            if (pipe_fd == 0xFF) { 
                printErr("Error: no se pudo abrir un pipe para el comando.\n");
                error_occurred = 1; 
                continue; 
            }
            setCommandStdout(command, i, pipe_fd);
        } else { 
            setCommandStdout(command, i, shell_original_stdout);
        }

        current_stdin_fd = getCommandStdin(command, i);
        current_stdout_fd = getCommandStdout(command, i);

        int cmd_index_in_shell = getCommandIndex(cmdName);
        if (cmd_index_in_shell == -1) {
            printErr(INVALID_COMMAND);
            error_occurred = 1; 
            if (i < qtyPrograms - 1) { 
                uint8_t fd_to_close = getCommandStdout(command, i);
                if (fd_to_close != shell_original_stdout && fd_to_close != 0xFF) {
                    closePipe(fd_to_close);
                }
            }
            continue; 
        }
        
        commands[cmd_index_in_shell].f(argc, argv);

        
        if (i < qtyPrograms - 1) {
            uint8_t fd_to_close = getCommandStdout(command, i);
            if (fd_to_close != shell_original_stdout && fd_to_close != 0xFF) {
                closePipe(fd_to_close); // Cierra el extremo de escritura
            }
        }

        if (i == qtyPrograms - 1 && isCommandBackground(command,i)) { 
            printf("Comando '%s' (o pipeline) en segundo plano (requiere funcionalidad de SO para procesos).\n", cmdName);
        }
    }

    
    if (qtyPrograms > 0 && getCommandStdin(command, qtyPrograms - 1) != shell_original_stdin &&
        getCommandStdin(command, qtyPrograms - 1) != 0xFF) {
        closePipe(getCommandStdin(command, qtyPrograms - 1));
    }


    // Restaurar los descriptores de E/S originales de la shell
    current_stdin_fd = shell_original_stdin;
    current_stdout_fd = shell_original_stdout;
}