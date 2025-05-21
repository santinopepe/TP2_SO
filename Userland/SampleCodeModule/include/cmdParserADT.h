#ifndef  CMD_PARSER_ADT_H
#define CMD_PARSER_ADT_H


#include <stdint.h>

typedef void (*CommandFunction)(int argc, char *argv[]); // Definicion de puntero a funcion

typedef struct CommandCDT * CommandADT;


/**
 * @brief Crea una estructura con los comandos 
 * @param input: Cadena de caracteres a parsear
 * @return CommandADT: Estructura con los comandos
*/
CommandADT parseInput(char *input); 


/**
 * @brief Libera la memoria de la estructura de comandos
 * @param cmd: Estructura de comandos a liberar
*/
void freeCommandADT(CommandADT cmd); 

/**
 * @brief Devuelve el nombre del comando
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
 * @return char*: Nombre del comando
*/
char * getCommandName(CommandADT cmd, int index);
/**
 * @brief Devuelve la cantidad de argumentos del comando
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
 * @return int: Cantidad de argumentos del comando
*/
int getCommandArgc(CommandADT cmd, int index);
/**
 * @brief Devuelve los argumentos del comando
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
 * @return char**: Argumentos del comando
*/
char ** getCommandArgv(CommandADT cmd, int index);

/**
 * @brief Devuelve la cantidad de comandos
 * @param cmd: Estructura de comandos
 * @return int: Cantidad de comandos
*/
int getCommandQty(CommandADT cmd); // Devuelve la cantidad de comandos en el vector de comandos


/**
 * @brief Devuelve el stdin del comando
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
*/
uint8_t getCommandStdin(CommandADT cmd, int index);

/**
 * @brief Devuelve el stdout del comando
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
*/
uint8_t getCommandStdout(CommandADT cmd, int index);

/**
 * @brief Cambia el stdin del comando
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
 * @param fd: File descriptor a cambiar
*/
void setCommandStdin(CommandADT cmd, int index, uint8_t fd);

/**
 * @brief Cambia el stdout del comando
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
 * @param fd: File descriptor a cambiar
*/
void setCommandStdout(CommandADT cmd, int index, uint8_t fd);

/**
 * @brief Devuelve si el comando se ejecuta en segundo plano
 * @param cmd: Estructura de comandos
 * @param index: Indice del comando
 * @return int: 1 si se ejecuta en segundo plano, 0 si no
*/
int isCommandBackground(CommandADT cmd, int index);

#endif // CMD_PARSER_ADT_H