#include <stdint.h>
#include <globals.h>

typedef struct semaphoresManagerCDT * SemaphoreADT;


/**
 * @brief Crea semaphoresManager
 * @return SemaphoreADT
*/
SemaphoreADT create_semaphoresManager();


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

int8_t sem_checkUse(uint8_t sem);
