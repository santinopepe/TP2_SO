// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
int philosopherSemIds[MAX_PHYLOS] = {0};

static void takeForks(int phyloId, int semId);
static void putForks(int phyloId, int semId);
static void test(int phyloId, int *semIds);
static void wait();
static void render();
static int getFreeSemaphoreId() ;

static int philosopher(int argc, char *argv[]);
static void addPhilosopher();
static void removePhilosopher();
static void startDining();

static void startDining() {
	char c;
	while (1) {
		c = getchar();
		if (c == 0) {
			continue; // Ignora los caracteres que sea null
		}
		if(c == 'A') {
			if(phylosCount == MAX_PHYLOS) {
				sem_wait(PRINT_ID);
				printf("Maximo de filosofos alcanzado\n");
				sem_post(PRINT_ID);
				continue;
			}
			sem_wait(PRINT_ID);
			printf("Agregando filosofo\n");
			sem_post(PRINT_ID);
			addPhilosopher();
		} else if (c == 'R') {
            if(phylosCount == MIN_PHYLOS){
                sem_wait(PRINT_ID);
                printf("Minimo de filosofos alcanzado\n");
                sem_post(PRINT_ID);
            }
            else{
			    removePhilosopher();
            }
		} else if (c == 'Q') {
			for (int i = phylosCount - 1; i >= 0; i--) {
				int semId = philosopherSemIds[i];
				sem_post(semId); // Desbloquea si está esperando
				killProcess(philosopherPids[i]);
				sem_close(semId);
				philosopherPids[i] = 0;
				state[i] = THINKING;
			}
			phylosCount = 0;
			break;
		}
	}

	if (sem_close(MUTEX_ID) == -1) {
		printf("Error cerrando semaphore mutex\n");
		return;
	}
	if (sem_close(PRINT_ID) == -1) {
		printf("Error cerrando semaphore print\n");
		return;
	}
	return;
}

static void addPhilosopher() {
    sem_wait(MUTEX_ID);
    state[phylosCount] = THINKING;
    char phyloBuff[PHYLO_BUFFER_SIZE] = {0};

    int semId = getFreeSemaphoreId();
    if (semId == -1) {
        printf("No semaforos libres disponibles para el filosofo %d\n", phylosCount);
        sem_post(MUTEX_ID);
        return;
    }

    philosopherSemIds[phylosCount] = semId;

    if (create_sem(semId, 0) == -1) {
        printf("Error creando el semaforo %d\n", semId);
        sem_post(MUTEX_ID);
        return;
    }

    itoa(phylosCount, phyloBuff, 10);
    char *params[] = {"philosopher", phyloBuff};

    philosopherPids[phylosCount] = createProcess((EntryPoint) philosopher, params, 2, 1, fileDescriptors);
    if (philosopherPids[phylosCount] < 0) {
        printf("Error creando el filosofo %d\n", phylosCount);
        sem_close(semId);
        sem_post(MUTEX_ID);
        return;
    }
    if (unblockProcess(philosopherPids[phylosCount]) == -1) {
        sem_close(semId);
        sem_post(MUTEX_ID);
        return;
    }

    phylosCount++;
    sem_post(MUTEX_ID);
}

static void removePhilosopher() {
    int idx = phylosCount - 1;
    if (idx < 0) return;

    sem_wait(PRINT_ID);
    printf("Removiendo el filosofo: %d\n", idx);
    sem_post(PRINT_ID);

    sem_wait(MUTEX_ID);

    int semId = philosopherSemIds[idx];

    if(phylosCount > 1){
        while (state[LEFT(idx)] == EATING && state[RIGHT(idx)] == EATING) {
            sem_post(MUTEX_ID);	
            if(sem_wait(semId)==-1){
                printf("Devolvió -1");
            }
            sem_wait(MUTEX_ID);
        }
    }
    sem_post(semId); // Desbloquea por si está esperando
    if (killProcess(philosopherPids[idx]) == -1) {
        printf("Error matando al filosofo %d\n", idx);
        sem_post(MUTEX_ID);
        return;
    }
    if (sem_close(semId) == -1) {
        printf("Error cerrando el semaforo %d\n", semId);
        sem_post(MUTEX_ID);
        return;
    }

    phylosCount--;
    sem_post(MUTEX_ID);
}

static int philosopher(int argc, char *argv[]) {
    int i = atoi(argv[1]);
    int semId = philosopherSemIds[i];
    while (1) {
        wait();
        takeForks(i, semId);
        wait();
        putForks(i, semId);
    }
    return 0;
}

static void takeForks(int phyloId, int semId) {
    sem_wait(MUTEX_ID);
    state[phyloId] = HUNGRY;
    test(phyloId, philosopherSemIds);
    sem_post(MUTEX_ID);
    sem_wait(semId);
}

static void putForks(int phyloId, int semId) {
    sem_wait(MUTEX_ID);
    state[phyloId] = THINKING;
    for (int i = 1; i <= phylosCount; i++) {
        int idx = (phyloId + i) % phylosCount;
        test(idx, philosopherSemIds);
    }
    sem_post(MUTEX_ID);
}

static void test(int phyloId, int *semIds) {
    if (state[phyloId] == HUNGRY && state[LEFT(phyloId)] != EATING && state[RIGHT(phyloId)] != EATING) {
        state[phyloId] = EATING;
        sem_post(semIds[phyloId]);
    }
    render();
}

static void render() {
	sem_wait(PRINT_ID);
	for (int i = 0; i < phylosCount; i++) {
		printf(state[i] == EATING ? "E " : ". ");
	}
	putchar('\n');
	sem_post(PRINT_ID);
}

static void wait() {
	for (int i = 0; i < 50000000; i++)
		;
}

void phylo(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Debes insertar UN parámetro que indique la cantidad de filósofos con los que deseas comenzar\n");
		return;
	}
	int aux = atoi(argv[1]);
	if (aux < MIN_PHYLOS || aux > MAX_PHYLOS) {
		printf("La cantidad de filosofos debe ser un numero entre %d y %d\n", MIN_PHYLOS, MAX_PHYLOS);
		return;
	}
	if(sem_checkUse(MUTEX_ID) == 1) {
		if(sem_close(MUTEX_ID) == -1) {
			printf("Error cerrando semaphore mutex\n");
			return;
		}
	}
	if (create_sem(MUTEX_ID, 1) == -1) {
		printf("Error creando semaphore mutex\n");
		return;
	}
	if(sem_checkUse(PRINT_ID) == 1) {
		sem_post(PRINT_ID);
		if(sem_close(PRINT_ID) == -1) {
			printf("Error cerrando semaphore print\n");
			return;
		}
	}
	if (create_sem(PRINT_ID, 1) == -1) {
		printf("Error creando semaphore print\n");
		return;
	}

	printf("Bienvenido a los filosofos comensales\n");
	printf("Commandos: (A)dd, (R)emove, (Q)uit\n");
	printf("Empezando...\n");
	for (int i = 0; i < aux; i++) {
		addPhilosopher();
	}

	startDining();

    for (int i = 0; i < MAX_PHYLOS; i++) {
        if (philosopherPids[i] > 0) {
			sem_post(i);
            killProcess(philosopherPids[i]);
            sem_close(i);
            philosopherPids[i] = 0;
        }
    }
    phylosCount = 0;
	for (volatile int j = 0; j < 10000000; j++);

	return;
}

static int getFreeSemaphoreId() {
    for (int i = 0; i < QUANTITY_OF_SEMAPHORES; i++) {
        if (sem_checkUse(i) == 0) {
            return i;
        }
    }
    return -1; // No hay semáforos libres
}