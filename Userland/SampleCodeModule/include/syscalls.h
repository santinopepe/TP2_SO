#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stdint.h>
#include <color.h>
#include <globals.h>


/**
 * @brief Escribe a partir del descriptor recibido un caracter
 * @param fd: FileDescriptor (STDOUT | STDERR)
 * @param c: Caracter a escribir
 */
void write(int fd, char c);

/**
 * @brief Lee un byte a partir del descriptor recibido
 * @param fd: FileDescriptor (STDIN | KBDIN)
 * @return Byte leido
 */
uint8_t read(int fd);

/**
 * @brief Devuelve la hora expresada en segundos
 * @return Hora expresada en segundos
 */
uint32_t getSeconds();

/**
 * @brief Pone todos los pixeles de la pantalla en negro y limpia el buffer de video
 */
void clear(void);

/**
 * @brief 
 * @param regarr: Vector donde se llena la informacion de los registros
 * @return Puntero a la informacion de los registros
 */
uint64_t * getInfoReg(uint64_t * regarr);

/**
 * @brief Cambia el tamaño de la fuente
 * @param size: (1|2|3)
 */
void setFontSize(uint8_t size);

/**
 * @brief Devuelve las dimensiones de la pantalla
 * @return 32 bits menos significativos el ancho, 32 el alto 
 */
uint32_t getScreenResolution();

/**
 * @brief  Dibuja un rectangulo
 * @param  x: Origen en x 
 * @param  y: Origen en y 
 * @param  width: Ancho
 * @param  height: Alto
 * @param  color: Color de relleno
 */
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

/**
 * @brief Devuelve la cantidad de ticks actuales
 * @return Cantidad de ticks
 */
uint64_t getTicks();

/**
 * @brief Llena un vector con 32 bytes de informacion a partir de una direccion de memoria en hexa
 * @param pos: Direccion de memoria a partir de la cual se llena el vector 
 * @param vec: Vector en el cual se llena la informacion
 */
void getMemory(uint64_t pos, uint8_t * vec);

/**
 * @brief Reproduce un sonido
 * @param frequency: Frecuencia del sonido
 * @param ticks: Cantidad de ticks que se desea que dure el sonido
 */
void playSound(uint64_t frequency, uint64_t ticks);

/**
 * @brief Ejecuta una excepcion de Invalid Opcode Exception
 */
void kaboom();

/**
 * @brief Establece un color de fuente
 * @param r: Color rojo
 * @param g: Color verde
 * @param b: Color azul
 */
void setFontColor(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Retorna el color de fuente que se esta usando actualmente
 * @return Color 
 */
Color getFontColor();

/**
 * @brief Reserva memoria en el heap
 * @param size: Cantidad de memoria a reservar
 * @return Puntero a la memoria reservada
*/
void * malloc(uint64_t size);


/**
 * @brief Libera memoria en el heap
 * @param ptr: Puntero a la memoria a liberar
*/
void free(void * ptr);

/**
 * @brief Devuelve la cantidad de memoria total y libre del heap
 * @param mem: Estructura donde se guardan los datos
*/
MemoryInfoCDT * getMemoryInfo(MemoryInfoADT  mem);

/**
 * @brief Abre un semáforo
 * @param sem: Semáforo a abrir
 * @return 0: Si el semáforo se abrió correctamente, -1 si no
 */
int8_t sem_open(uint8_t sem);

/**
 * @brief Cierra un semáforo
 * @param sem: Semáforo a abrir
 * @return 0: Si el semáforo se cerró correctamente, -1 si no
 */
int8_t sem_close(uint8_t sem);

/**
 * @brief Espera un semáforo o decrementa su valor
 * @param sem: Semáforo a esperar
 * @return 0: Si el semáforo se decrementó correctamente, -1 si no
 */
int8_t sem_wait(uint8_t sem);

/**
 * @brief Aumenta el valor de un semáforo o libera un proceso bloqueado
 * @param sem: Semáforo a liberar
 * @return 0: Si el semáforo se liberó correctamente, -1 si no
 */
int8_t sem_post(uint8_t sem);

/**
 * @brief Crea un semáforo
 * @param sem: Semáforo a crear
 * @param value: Valor inicial del semáforo
 * @return 0: Si el semáforo se creó correctamente, -1 si no
 */
int8_t create_sem(uint8_t sem, uint8_t value);


/**
 * @brief Abre un pipe
 * @param pid: PID del proceso que abre el pipe
 * @param mode: Modo de apertura del pipe
 * @return uint8_t: Descriptor del pipe
*/
uint8_t openPipe(uint16_t pid, uint8_t mode);

/**
 * @brief Cierra un pipe
 * @param fd: Descriptor del pipe
*/
uint8_t closePipe(uint8_t fd);

/**
 * @brief Escribe en un pipe
 * @param fd: Descriptor del pipe
 * @param buffer: Buffer de datos a escribir
 * @param size: Tamaño del buffer de los datos a escribir
 * @return uint8_t: Cantidad de los datos escritos
*/
uint8_t writePipe(uint8_t fd, char *buffer, uint8_t size);

/**
 * @brief Lee en un pipe
 * @param fd: Descriptor del pipe
 * @param buffer: Buffer de datos a leer
 * @param size: Tamaño del buffer de los datos a leer
 * @return uint8_t: Cantidad de los datos leídos
*/
uint8_t readPipe(uint8_t fd, char *buffer, uint8_t size);

/**
 * @brief Crea un nuevo proceso
 * @param rip: Direccion de inicio del proceso
 * @param argv: Argumentos del proceso
 * @param argc: Cantidad de argumentos
 * @param priority: Prioridad del proceso
 * @param fileDescriptors: File descriptors del proceso
 * @return uint16_t: PID del nuevo proceso
*/
uint16_t createProcess(EntryPoint rip, char **argv, int argc, uint8_t priority, uint16_t fileDescriptors[]);

/**
 * @brief Mata un proceso
 * @param pid: PID del proceso a matar
 * @return 0: Si el proceso se mató correctamente, -1 si no existe el proceso o -2 si no se puede matar el proceso
*/
int killProcess(uint16_t pid);

/**
 * @brief Cambia la prioridad de un proceso
 * @param pid: PID del proceso a cambiar la prioridad
 * @param priority: Nueva prioridad del proceso
 * @return 0: Si la prioridad se cambió correctamente, -1 si no existe el proceso o -2 si no se puede cambiar la prioridad
*/
int setPriority(uint16_t pid, uint8_t priority);

/**
 * @brief Cambia el estado de un proceso
 * @param pid: PID del proceso a cambiar el estado
 * @param status: Nuevo estado del proceso
 * @return 0: Si el estado se cambió correctamente, -1 si no existe el proceso o -2 si no se puede cambiar el estado
*/
int setStatus(uint16_t pid, ProcessStatus status);

/**
 * @brief Desbloquea un proceso
 * @param pid: PID del proceso a desbloquear
*/
int unblockProcess(uint32_t pid);

/**
 * @brief Bloquea un proceso
 * @param pid: PID del proceso a bloquear
*/
int blockProcess(uint32_t pid);

/**
 * @brief Devuelve el PID del proceso actual
 * @return PID del proceso actual
*/
uint16_t getPid();


/**
 * @brief Obtiene la informacion de un proceso
 * @param process: Estructura donde se guardan los datos del proceso
*/
void processInfo(ProcessData * process);

/**
 * @brief Espera a que un proceso termine sus hijos
 * @param pid: PID del proceso a esperar
 * @return 0: Si pudo esperar a los hijos, -1 si no existe el proceso o -2 si no se puede esperar a los hijos
*/
 

uint16_t waitForChildren(uint16_t pid);


/**
 * @brief Cede el CPU al siguiente proceso
 * Esta funcion se usa en el scheduler para cambiar de proceso, se usa en el dispatcher
*/
void yield();

uint16_t waitForChildren(uint16_t pid);

void wait_time(uint32_t delta);

char * getMemoryType();

int getUsedMemory();

int getFreeMemory();

void changeFDS(uint16_t pid, uint16_t fileDescriptors[]);

#endif