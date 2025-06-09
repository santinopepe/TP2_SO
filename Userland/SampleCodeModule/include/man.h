#ifndef _MAN_H
#define _MAN_H
#include <shell.h>
/* Matriz con una breve explicacion de uso de cada uno de los comandos */
char * usages [QTY_COMMANDS] = {
    "Uso: help - Muestra una lista de todos los comandos disponibles en la terminal. No recibe parametros.",
    
    "Uso: man [COMANDO] - Explica el funcionamiento y la sintaxis de un comando especifico enviado como parametro.",
    
    "Uso: inforeg - Muestra informacion detallada de los registros del procesador capturados en un momento arbitrario de la\nejecucion del sistema.\nNo recibe parametros.",
    
    "Uso: time - Despliega la hora actual del sistema en formato UTC-3. No recibe parametros.",
    
    "Uso: div [OP1] [OP2] - Realiza la division entera de dos numeros naturales recibidos por parametro y muestra el resultado y\nel resto.\nEjemplo: div 10 5",
    
    "Uso: kaboom - Lanza una excepcion de Invalid Opcode para probar el manejo de errores del sistema. No recibe parametros.",
    
    "Uso: font-size [1|2|3] - Cambia el tamanio de la fuente de la terminal. Se debe indicar el tamanio deseado como parámetro (1, 2 o 3).\nEjemplo: font-size 2",
    
    "Uso: printmem [DIR] - Imprime los primeros 32 bytes de memoria a partir de la direccion enviada como parametro, mostrando el\ncontenido en hexadecimal y ASCII.\nEjemplo: printmem 10ff8c",
    
    "Uso: clear - Limpia completamente la pantalla de la terminal.\nNo recibe parametros.",
    
    "Uso: nice <pid> <prioridad> - Cambia la prioridad de ejecucion de un proceso identificado por su PID. La prioridad debe ser un\nvalor valido.",
    
    "Uso: block <pid> - Bloquea el proceso con el PID especificado, impidiendo que sea ejecutado por el scheduler hasta ser\ndesbloqueado.",
    
    "Uso: unblock <pid> - Desbloquea el proceso con el PID especificado, permitiendo que vuelva a ser ejecutado por el scheduler.",
    
    "Uso: kill <pid> - Elimina el proceso con el PID especificado, liberando sus recursos.",
    
    "Uso: test-processes <cantidad> - Ejecuta un test de creacion y finalizacion de multiples procesos para verificar el correcto\nmanejo de procesos por el sistema.",
    
    "Uso: test-priority - Ejecuta un test para comprobar el funcionamiento de las prioridades de los procesos en el scheduler.",
    
    "Uso: test-mm <max_memory> - Ejecuta un test de administración de memoria, solicitando y liberando bloques hasta alcanzar el\nmaximo indicado.",
    
    "Uso: test-sync <sem> - Ejecuta un test de sincronizacion entre procesos utilizando semaforos. El parametro indica si se debe\nusar semáforo (1) o no (0).",
    
    "Uso: mem - Muestra información detallada sobre el estado actual de la memoria del sistema, incluyendo bloques libres y\nocupados.",
    
    "Uso: wc - Cuenta y muestra la cantidad de lineas recibidas por entrada estandar (STDIN). Finaliza al recibir dos lineas\nvacias consecutivas.",
    
    "Uso: filter - Lee texto desde la entrada estandar y lo imprime eliminando todas las vocales. Finaliza al recibir dos lineas\nvacias consecutivas.",
    
    "Uso: cat - Imprime por pantalla todo lo recibido por entrada estándar (STDIN) hasta que se ingresan dos lineas vacias\nconsecutivas.",
    
    "Uso: phylo - Ejecuta la simulaciin del problema de los filosofos comensales, mostrando el estado de cada filosofo en tiempo\n real.",
    
    "Uso: ps - Muestra informacion sobre todos los procesos vivos, incluyendo PID, estado, prioridad y recursos asignados.",
    
    "Uso: loop <cantidad de segundos> - Crea un proceso que imprime su ID y un saludo cada cierta cantidad de segundos especificada\n por parametro.",

    "Uso: color <color> - Cambia el color de la fuente de la terminal. El parametro debe ser un número del 0 al 5 que representa\nun color valido. Los colores validos son:\n 0. Verde claro,\n 1. Verde oscuro,\n 2. Rosa,\n 3. Magenta,\n 4. Plateado,\n 5. Rojo.",
};
#endif