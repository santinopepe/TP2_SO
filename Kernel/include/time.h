#include <stdint.h>

/**
 * @brief Handler del timer tick
 */
void timerHandler();
/**
 * @return Devuelve los ticks 
 */
uint64_t ticksElapsed();

/**
 * @return  Devuelve los segundos
 */
int secondsElapsed();

void wait_time(int delta);


