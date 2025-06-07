#include <syscalls.h>
#include <stdlib.h>
#include <globals.h>
#include <stdio.h>


#define MAX_PHYLOS 10
#define MIN_PHYLOS 5
#define PHYLO_BUFFER_SIZE 3

#define MUTEX_ID 16
#define PRINT_ID 17

#define LEFT(phyloId) (((phyloId) + phylosCount - 1) % phylosCount)
#define RIGHT(phyloId) (((phyloId) + 1) % phylosCount)

typedef enum { THINKING, HUNGRY, EATING } state_t;

int phylosCount = 0;

state_t state[MAX_PHYLOS] = {0};
int philosopherPids[MAX_PHYLOS] = {0};

void takeForks(int phyloId);
void putForks(int phyloId);
void test(int phyloId);
void wait();
void render();

int philosopher(int argc, char *argv[]);
void addPhilosopher();
void removePhilosopher();

void startDining() {
	char c;
	while (1) {
		c = getchar();
		/*if (c == 0) {
			continue; // Ignore null characters
		}*/
		if(c == 'A') {
			if(phylosCount == MAX_PHYLOS) {
				sem_wait(PRINT_ID);
				printf("Maximum philosophers reached\n");
				sem_post(PRINT_ID);
				continue;
			}
			sem_wait(PRINT_ID);
			printf("Adding philosopher\n");
			sem_post(PRINT_ID);
			addPhilosopher();
		} else if (c == 'R') {
			removePhilosopher();
		} else if (c == 'Q') {
			while(phylosCount > 0) {
				removePhilosopher();
			}
			break;
		}
	}

	/*for (int i = 0; i < phylosCount; i++) {
		if (killProcess(philosopherPids[i]) == -1) {
			printf("Error killing philosopher %d\n", i);
			
			return;
		}
		if (sem_close(i) == -1) {
			printf("Error closing semaphore %d\n", i);
		
			return;
		}
	}*/

	sem_post(PRINT_ID);
	if (sem_close(MUTEX_ID) == -1) {
		printf("Error closing semaphpore mutex\n");
		return;
	}
	if (sem_close(PRINT_ID) == -1) {
		printf("Error closing semaphpore print\n");
		return;
	}
	return;
}

void addPhilosopher() {
	sem_wait(MUTEX_ID);
	state[phylosCount] = THINKING;
	char phyloBuff[PHYLO_BUFFER_SIZE] = {0};
	sem_close(phylosCount);
	if (create_sem(phylosCount, 0) == -1) {
		printf("Error creating semaphore %d\n", phylosCount);
		return;
	}

	itoa(phylosCount, phyloBuff, 10);
	char *params[] = {"philosopher", phyloBuff};
	

	philosopherPids[phylosCount] = createProcess((EntryPoint) philosopher, params, 2, 1, fileDescriptors);
	if (philosopherPids[phylosCount] < 0) {
		printf("Error creating philosopher %d\n", phylosCount);
		return;
	}
	if (unblockProcess(philosopherPids[phylosCount]) == -1) {
		return;
	}

	phylosCount++;
	sem_post(MUTEX_ID);
}

void removePhilosopher() {
	/*if (phylosCount <= MIN_PHYLOS) {
		sem_wait(PRINT_ID);
		printf("Minimum philosophers reached\n");
		sem_post(PRINT_ID);
		return;
	}*/
	phylosCount--;
	sem_wait(PRINT_ID);
	printf("Removing philosopher: %d\n", phylosCount);
	sem_post(PRINT_ID);

	sem_wait(MUTEX_ID);
	while (state[LEFT(phylosCount)] == EATING && state[RIGHT(phylosCount)] == EATING) {
		sem_post(MUTEX_ID);	
		if(sem_wait(phylosCount)==-1){
			printf("Devolvió -1");
		}
		sem_wait(MUTEX_ID);
	}
		
	if (killProcess(philosopherPids[phylosCount]) == -1) {
		printf("Error killing philosopher %d\n", phylosCount);
		return;
	}
	if (sem_close(phylosCount) == -1) {
		printf("Error closing semaphore %d\n", phylosCount);
		return;
	}

	sem_post(MUTEX_ID);
}

int philosopher(int argc, char *argv[]) {
	int i = atoi(argv[1]);
	while (1) {
		wait();
		takeForks(i);
		wait();
		putForks(i);
	}
	return 0;
}

void takeForks(int phyloId) {
	sem_wait(MUTEX_ID);
	state[phyloId] = HUNGRY;
	test(phyloId);
	sem_post(MUTEX_ID);
	sem_wait(phyloId);
}

void putForks(int phyloId) {
	sem_wait(MUTEX_ID);
	state[phyloId] = THINKING;
    for (int i = 0; i < phylosCount; i++) {
        test(i);
    }
	sem_post(MUTEX_ID);
}

void test(int phyloId) {
	if (state[phyloId] == HUNGRY && state[LEFT(phyloId)] != EATING && state[RIGHT(phyloId)] != EATING) {
		state[phyloId] = EATING;
		sem_post(phyloId);
	}
	render();
}

void render() {
	sem_wait(PRINT_ID);
	for (int i = 0; i < phylosCount; i++) {
		printf(state[i] == EATING ? "E " : ". ");
	}
	putchar('\n');
	sem_post(PRINT_ID);
}

void wait() {
	for (int i = 0; i < 5000000; i++)
		;
}

void phylo(int argc, char *argv[]) {
	if (argc != 2) {
		printf("You must insert ONE parameter indicating the amount of philosophers you desire to start with\n");
		return;
	}
	int aux = atoi(argv[1]);
	if (aux < MIN_PHYLOS || aux > MAX_PHYLOS) {
		printf("The amount of philosophers must be a number between %d and %d\n", MIN_PHYLOS, MAX_PHYLOS);
		return;
	}
	if(sem_checkUse(MUTEX_ID) == 1) {
		if(sem_close(MUTEX_ID) == -1) {
			printf("Error closing semaphore mutex\n");
			return;
		}
	}
	if (create_sem(MUTEX_ID, 1) == -1) {
		printf("Error creating semaphore mutex\n");
		return;
	}
	if(sem_checkUse(PRINT_ID) == 1) {
		sem_post(PRINT_ID);
		if(sem_close(PRINT_ID) == -1) {
			printf("Error closing semaphore print\n");
			return;
		}
	}
	if (create_sem(PRINT_ID, 1) == -1) {
		printf("Error creating semaphore print\n");
		return;
	}

	printf("Welcome to the Dining Philosophers\n");
	printf("Commands: (A)dd, (R)emove, (Q)uit\n");
	printf("Starting...\n");
	for (int i = 0; i < aux; i++) {
		addPhilosopher();
	}

	startDining();
	phylosCount = 0;
	return;
}