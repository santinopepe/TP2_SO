#include <stdint.h>
#include <stddef.h>
#include <syscalls.h>
#include <globals.h>
#include <shell.h>
#include <stdio.h>
#include <cmdParserADT.h>


typedef struct
{
    char *name;        // Nombre del comando
    char *description; // Descripcion del comando (para help)
    CommandFunction f; // Funcion a ejecutar
} Command;



/**
 * @brief Cuenta la cantidad de lineas.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 * @return 0 si se ejecuta correctamente, -1 si hay un error.
 */
 int wc(int argc, char *argv[]);

 /**
  * @brief Imprime el contenido de un archivo.
  * @param argc: Cantidad de argumentos
  * @param argv: Argumentos
  * @return 0 si se ejecuta correctamente, -1 si hay un error.
  */
 int cat(int argc, char *argv[]);

 /**
  * @brief Filtra las vocales de la entrada estándar.
  * @param argc: Cantidad de argumentos
  * @param argv: Argumentos
  * @return 0 si se ejecuta correctamente, -1 si hay un error.
  */
 void filter(int argc, char *argv[]);

/** 
 * @brief Imprime los procesos activos en un instante dad.
 *  @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
 void ps(int argc, char *argv[]);

 /**
  * @brief Ejecuta un proceso que imprime su ID con un saludo cada una determinada cantidad de segundos.
  * @param argc: Cantidad de argumentos
  * @param argv: Argumentos
  */
void loop(int argc, char *argv[]);

/**
 * @brief Imprime el estado de la memoria del sistema.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void mem(int argc, char *argv[]);

/**
 * @brief Mata un proceso dado su PID.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void kill(int argc, char *argv[]);


/**
 * @brief Cambia la prioridad de un proceso dado su PID.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
int niceWrapper(int argc, char *argv[]);

/**
 * @brief Bloquea un proceso dado su PID.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void blockProcessWrapper(int argc, char *argv[]);


/**
 * @brief Desbloquea un proceso dado su PID.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void unblockProcessWrapper(int argc, char *argv[]);

/**
 * @brief Imprime la informacion de los registros.
 */
void printInfoReg();

/**
 * @brief Cambia el color de la fuente.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void changeColor(int argc, char *argv[]);


/**
 * @brief Es el manual de los comandos.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void man(int argc, char *argv[]);

/**
 * @brief Imprime un vuelco de memoria, a partir de una posicion dada.
 * @param pos: Posicion de memoria a imprimir
 */
void printMem(char *pos);

/**
 * @brief Hace una division entre dos numeros.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
int div(int argc, char *argv[]);

/**
 * @brief Imprime la hora actual del sistema.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void time(int argc, char *argv[]);


/**
 * @brief Cambia el tamaño de la fuente.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void fontSize(int argc, char *argv[]);  

/**
 * @brief Imprime una lista con los comandos disponibles y sus descripciones.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void help(int argc, char *argv[]);

/**
 * @brief Limpia la pantalla.
 * @param argc: Cantidad de argumentos
 * @param argv: Argumentos
 */
void myClear(); 

