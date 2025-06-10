// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <globals.h>
#include <syscalls.h>
#include <test_util.h>

#define SEM_ID 20
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void printValue(int value);

void slowInc(int64_t *p, int64_t inc) {
	uint64_t aux = *p;
	yield();
	aux += inc;
	*p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
	uint64_t n;
	int8_t inc;
	int8_t use_sem;

	if (argc != 4) {
		return -1;
	}
	if ((n = satoi(argv[1])) <= 0) {
		return -1;
	}
	if ((inc = satoi(argv[2])) == 0) {
		return -1;
	}
	if ((use_sem = satoi(argv[3])) < 0) {
		return -1;
	}
	if (use_sem)
		if (sem_open(SEM_ID) == -1) {
			printf("test_sync: ERROR opening semaphore\n");
			return -1;
		}

	uint64_t i;
	for (i = 0; i < n; i++) {
		if (use_sem) {
			sem_wait(SEM_ID);
		}
		slowInc(&global, inc);
		if (use_sem) {
			sem_post(SEM_ID);
		}
	}

	return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
	uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

	if (argc != 3)
		return -1;

	int8_t use_sem = satoi(argv[2]);
	if (use_sem) {
		if (create_sem(SEM_ID, 1) == -1) {
			printf("test_sync: ERROR creating semaphore\n");
			return -1;
		}
	}

	char *argvDec[] = {"my_process_dec", argv[1], "-1", argv[2]};
	char *argvInc[] = {"my_process_inc", argv[1], "1", argv[2]};
	uint16_t fileDescriptors[] = {STDIN, STDOUT, STDERR}; // stdin, stdout, stderr

	global = 0;

	uint64_t i;
	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		pids[i] = createProcess((EntryPoint) my_process_inc, argvDec, 4, 1, fileDescriptors);
		unblockProcess(pids[i]);
		pids[i + TOTAL_PAIR_PROCESSES] = createProcess((EntryPoint) my_process_inc, argvInc, 4, 1, fileDescriptors);
		unblockProcess(pids[i + TOTAL_PAIR_PROCESSES]);
	}

	waitForChildren();
	printf("test_sync: Final value: ");
  printValue(global);
  printf("\n");

	if (use_sem)
		sem_close(SEM_ID);

	return 0;
}

void printValue(int value) {
  if (value < 0) {
    putchar('-');
    value = -value;
  }
  if (value == 0) {
    putchar('0');
    return;
  }
  char buffer[12]; // Suficiente para un int32_t
  int i = 0;
  while (value > 0) {
    buffer[i++] = '0' + (value % 10);
    value /= 10;
  }
  while (i > 0) {
    putchar(buffer[--i]);
  }
}