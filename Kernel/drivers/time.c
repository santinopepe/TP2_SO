#include <time.h>
#include <stdint.h>
#include <lib.h>
static uint64_t ticks = 0;

void timerHandler() {
	ticks++;
}

uint64_t ticksElapsed() {
	return ticks;
}

int secondsElapsed() {
	return ticks / 18;
}

void wait_time(int delta) {
	int currentTicks = ticks;
	while (ticks - currentTicks < delta) {
		_hlt();
	}
}