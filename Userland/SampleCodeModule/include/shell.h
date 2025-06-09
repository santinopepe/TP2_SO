#ifndef _SHELL_H
#define _SHELL_H
#define QTY_COMMANDS 25

#define WELCOME "Bienvenido a SIM SIM OS!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT "Escriba \"man font-size\" para ver las dimensiones validas\n"



/**
 * @brief Corre la terminal
 * 
 */
void run_shell();
#endif