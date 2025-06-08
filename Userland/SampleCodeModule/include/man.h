#ifndef _MAN_H
#define _MAN_H
#include <shell.h>
/* Matriz con una breve explicacion de uso de cada uno de los comandos */
char * usages [QTY_COMMANDS] = {
    "Uso: help - Muestra una lista de todos los comandos disponibles en la terminal. No recibe parámetros.",
    
    "Uso: man [COMANDO] - Explica el funcionamiento y la sintaxis de un comando específico enviado como parámetro.",
    
    "Uso: inforeg - Muestra información detallada de los registros del procesador capturados en un momento arbitrario de la\nejecución del sistema.\nNo recibe parámetros.",
    
    "Uso: time - Despliega la hora actual del sistema en formato UTC-3. No recibe parámetros.",
    
    "Uso: div [OP1] [OP2] - Realiza la división entera de dos números naturales recibidos por parámetro y muestra el resultado y\nel resto.\nEjemplo: div 10 5",
    
    "Uso: kaboom - Lanza una excepción de Invalid Opcode para probar el manejo de errores del sistema. No recibe parámetros.",
    
    "Uso: font-size [1|2|3] - Cambia el tamaño de la fuente de la terminal. Se debe indicar el tamaño deseado como parámetro (1, 2 o 3).\nEjemplo: font-size 2",
    
    "Uso: printmem [DIR] - Imprime los primeros 32 bytes de memoria a partir de la dirección enviada como parámetro, mostrando el\ncontenido en hexadecimal y ASCII.\nEjemplo: printmem 10ff8c",
    
    "Uso: clear - Limpia completamente la pantalla de la terminal.\nNo recibe parámetros.",
    
    "Uso: nice <pid> <prioridad> - Cambia la prioridad de ejecución de un proceso identificado por su PID. La prioridad debe ser un\nvalor válido.",
    
    "Uso: block <pid> - Bloquea el proceso con el PID especificado, impidiendo que sea ejecutado por el scheduler hasta ser\ndesbloqueado.",
    
    "Uso: unblock <pid> - Desbloquea el proceso con el PID especificado, permitiendo que vuelva a ser ejecutado por el scheduler.",
    
    "Uso: kill <pid> - Elimina el proceso con el PID especificado, liberando sus recursos.",
    
    "Uso: test-processes <cantidad> - Ejecuta un test de creación y finalización de múltiples procesos para verificar el correcto\nmanejo de procesos por el sistema.",
    
    "Uso: test-priority - Ejecuta un test para comprobar el funcionamiento de las prioridades de los procesos en el scheduler.",
    
    "Uso: test-mm <max_memory> - Ejecuta un test de administración de memoria, solicitando y liberando bloques hasta alcanzar el\nmáximo indicado.",
    
    "Uso: test-sync <sem> - Ejecuta un test de sincronización entre procesos utilizando semáforos. El parámetro indica si se debe\nusar semáforo (1) o no (0).",
    
    "Uso: mem - Muestra información detallada sobre el estado actual de la memoria del sistema, incluyendo bloques libres y\nocupados.",
    
    "Uso: wc - Cuenta y muestra la cantidad de líneas recibidas por entrada estándar (STDIN). Finaliza al recibir dos líneas\nvacías consecutivas.",
    
    "Uso: filter - Lee texto desde la entrada estándar y lo imprime eliminando todas las vocales. Finaliza al recibir dos líneas\nvacías consecutivas.",
    
    "Uso: cat - Imprime por pantalla todo lo recibido por entrada estándar (STDIN) hasta que se ingresan dos líneas vacías\nconsecutivas.",
    
    "Uso: phylo - Ejecuta la simulación del problema de los filósofos comensales, mostrando el estado de cada filósofo en tiempo\n real.",
    
    "Uso: ps - Muestra información sobre todos los procesos vivos, incluyendo PID, estado, prioridad y recursos asignados.",
    
    "Uso: loop <cantidad de segundos> - Crea un proceso que imprime su ID y un saludo cada cierta cantidad de segundos especificada\n por parámetro.",

    "Uso: color <color> - Cambia el color de la fuente de la terminal. El parámetro debe ser un número del 0 al 5 que representa\nun color valido. Los colores validos son:\n 0. Verde claro,\n 1. Verde oscuro,\n 2. Rosa,\n 3. Magenta,\n 4. Plateado,\n 5. Rojo.",
};
#endif