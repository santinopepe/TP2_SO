#include <cmdParserADT.h>
#include <stddef.h>
#include <syscalls.h> 
#include <string.h>   
#include <stdio.h>

#define MAX_COMMANDS 10 //Esto es una limitacion
#define PIPE '|'
#define BUFFER 256
#define MAX_ARGS 10 



typedef struct Function {
    char *name;         
    int argc;
    char **argv;        
    int background;
    uint8_t stdin_fd; 
    uint8_t stdout_fd; 
} Function;

typedef struct CommandCDT {
    Function commands[MAX_COMMANDS];
    int qtyPrograms;
} CommandCDT;


static void _freeFunction(Function *func); // Función para liberar la memoria de una Function
static int _skipSpaces(const char *input);

typedef struct CommandCDT * CommandADT;

CommandADT parseInput(char *input) {
    CommandADT command = (CommandADT) malloc(sizeof(CommandCDT));
    if (command == NULL) {
        printf("Error: no se pudo reservar memoria para el comando.\n");
        return NULL; 
    }
    command->qtyPrograms = 0;
    int inputIdx = 0;

    while (input[inputIdx] != '\n' && input[inputIdx] != '\0' && command->qtyPrograms < MAX_COMMANDS) {
        inputIdx += _skipSpaces(input + inputIdx);

        if (input[inputIdx] == '\n' || input[inputIdx] == '\0') {
            break;
        }

        Function *currentFunction = &(command->commands[command->qtyPrograms]);
        currentFunction->name = NULL;  
        currentFunction->argc = 0;
        currentFunction->argv = NULL; 
        currentFunction->background = 0;

    
        int copiedChars = strcpychar((currentFunction->name), input + inputIdx, ' ');

        //Aca deberia haber || currentFunction->name == NULL
        if (copiedChars == 0 ) {
            freeCommandADT(command);
            printf("Error: no se pudo copiar el nombre del comando.\n");
            return NULL;
        }
        inputIdx += copiedChars;

        currentFunction->argv = (char **) malloc(MAX_ARGS * sizeof(char *));
        if (currentFunction->argv == NULL) {
            freeCommandADT(command);
            return NULL;
        }

        while (input[inputIdx] != '\n' && input[inputIdx] != '\0' && input[inputIdx] != PIPE && currentFunction->argc < MAX_ARGS) {
            inputIdx += _skipSpaces(input + inputIdx);

            if (input[inputIdx] == '&') {
                currentFunction->background = 1;
                inputIdx++;
                inputIdx += _skipSpaces(input + inputIdx);
                if (input[inputIdx] == '\n' || input[inputIdx] == '\0' || input[inputIdx] == PIPE) {
                    break;
                }
            } else if (input[inputIdx] == PIPE) {
                break;
            } else if (input[inputIdx] == '\n' || input[inputIdx] == '\0') {
                break;
            }

            char *arg = NULL;
            copiedChars = strcpychar(arg, input + inputIdx, ' ');
            if (copiedChars == 0 || arg == NULL) {
                break;
            }
            currentFunction->argv[currentFunction->argc] = arg;
            inputIdx += copiedChars;
            currentFunction->argc++;
        }

        command->qtyPrograms++;

        if (input[inputIdx] == PIPE) {
            inputIdx++;
        }

    }

    return command;
}


static int _skipSpaces(const char *input) {
    int i = 0;
    while (input[i] == ' ' || input[i] == '\t') {
        i++;
    }
    return i;
}

static void _freeFunction(Function *func) {
    if (func->name != NULL) {
        free(func->name);
        func->name = NULL;
    }
    if (func->argv != NULL) {
        for (int i = 0; i < func->argc; i++) {
            if (func->argv[i] != NULL) {
                free(func->argv[i]);
                func->argv[i] = NULL;
            }
        }
        free(func->argv);
        func->argv = NULL;
    }
}

void freeCommandADT(CommandADT cmd) {
    if (cmd == NULL) {
        return;
    }
    for (int i = 0; i < cmd->qtyPrograms; i++) {
        _freeFunction(&(cmd->commands[i]));
    }
    free(cmd);
}

char * getCommandName(CommandADT cmd, int index) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return NULL;
    }
    return cmd->commands[index].name;
}

int getCommandArgc(CommandADT cmd, int index) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return -1;
    }
    return cmd->commands[index].argc;
}
char ** getCommandArgv(CommandADT cmd, int index) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return NULL;
    }
    return cmd->commands[index].argv;
}
int isCommandBackground(CommandADT cmd, int index) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return -1;
    }
    return cmd->commands[index].background;
}
int getCommandQty(CommandADT cmd) {
    if (cmd == NULL) {
        return -1;
    }
    return cmd->qtyPrograms;
}

uint8_t getCommandStdin(CommandADT cmd, int index) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return -1;
    }
    return cmd->commands[index].stdin_fd;
}

uint8_t getCommandStdout(CommandADT cmd, int index) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return -1;
    }
    return cmd->commands[index].stdout_fd;
}

void setCommandStdin(CommandADT cmd, int index, uint8_t fd) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return;
    }
    cmd->commands[index].stdin_fd = fd;
}

void setCommandStdout(CommandADT cmd, int index, uint8_t fd) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return;
    }
    cmd->commands[index].stdout_fd = fd;
}

int getIsBackground(CommandADT cmd, int index) {
    if (cmd == NULL || index < 0 || index >= cmd->qtyPrograms) {
        return -1;
    }
    return cmd->commands[index].background;
}
