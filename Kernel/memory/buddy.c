#include <MemoryManager.h>
#include <memoryInfo.h>
#include <video.h>
#include <string.h>

#define SPLIT 1
#define USED 2

#define POW_TWO(x) ((uint64_t) 1 << (x))
#define MIN_EXPONENT 6
#define MIN_ALLOC POW_TWO(MIN_EXPONENT)
#define MAX_EXPONENT 28
#define MAX_ALLOC POW_TWO(MAX_EXPONENT)
#define NODES (POW_TWO(MAX_EXPONENT - MIN_EXPONENT + 1) - 1)

typedef struct {
	uint8_t use;
} Node;

typedef struct MemoryManagerCDT {
	uint8_t *firstAddress;
	Node *tree;
	uint64_t totalMemory;
	uint64_t usedMemory;
	char memoryType[32]; 
} MemoryManagerCDT;

static MemoryManagerADT memoryManager = NULL;

static MemoryManagerADT getMemoryManager();
static uint64_t getNode(uint8_t exponent);
static uint8_t getExponent(uint64_t request);
static int64_t lookFreeNode(uint8_t exponent);
static uint8_t *adress(uint64_t node, uint8_t exponent);
static int64_t searchNode(uint8_t *ptr, uint8_t *exponent);
static uint8_t getExponentAddress(void *ptr);
static void treeUpdate(uint64_t node);
static void setSplit(uint64_t node);
static void setChildren(uint64_t node);
static void releaseNode(uint64_t node);

static MemoryManagerADT getMemoryManager() {
	return memoryManager;
}

void createMemoryManager(void *start, uint64_t size) {
	memoryManager = start;

	if (size < MIN_ALLOC) {
		return;
	}

	memoryManager->firstAddress = start + sizeof(MemoryManagerCDT);
	memoryManager->tree = (Node *) (memoryManager->firstAddress + HEAP_SIZE);

	memoryManager->totalMemory = HEAP_SIZE;
	memoryManager->usedMemory = 0;

	strcpy(memoryManager->memoryType, "Buddy");
}

void *malloc(uint64_t size) {
	MemoryManagerADT memoryManager = getMemoryManager();
	if (size > memoryManager->totalMemory || size == 0) {
		return NULL;
	}

	uint8_t exponent = getExponent(size);
	if (exponent > MAX_EXPONENT) {
		return NULL;
	}

	int64_t node = lookFreeNode(exponent);
	if (node == -1) {
		return NULL;
	}

	treeUpdate(node);
	memoryManager->tree[node].use = USED;

	memoryManager->usedMemory += POW_TWO(exponent);

	return adress(node, exponent);
}

void free(void *ptr) {
	MemoryManagerADT memoryManager = getMemoryManager();
	if (ptr == NULL) {
		return;
	}
	uint8_t exponent = getExponentAddress(ptr);
	uint64_t node = searchNode(ptr, &exponent);

	releaseNode(node);
	memoryManager->usedMemory -= POW_TWO(exponent);


}


static inline uint64_t getNode(uint8_t exponent) {
	return POW_TWO(MAX_EXPONENT - exponent) - 1;
}

static uint8_t *adress(uint64_t node, uint8_t exponent) {
	MemoryManagerADT memoryManager = getMemoryManager();
	return memoryManager->firstAddress + ((node - getNode(exponent)) << exponent);
}

static int64_t searchNode(uint8_t *ptr, uint8_t *exponent) {
	MemoryManagerADT memoryManager = getMemoryManager();
	int64_t node = 0;
	uint8_t auxExponent = *exponent;

	while (auxExponent > 0 && memoryManager->tree[node].use != USED) {
		node = ((ptr - memoryManager->firstAddress) >> auxExponent) + getNode(auxExponent);
		auxExponent--;
	}
	*exponent = auxExponent;
	return node;
}

static uint8_t getExponentAddress(void *ptr) {
	MemoryManagerADT memoryManager = getMemoryManager();
	uint64_t addr = (uint8_t *) ptr - memoryManager->firstAddress;
	uint8_t exponent = MAX_EXPONENT;
	uint64_t size = MAX_ALLOC;
	while (addr % size != 0) {
		exponent--;
		size >>= 1;
	}
	return exponent;
}

static uint8_t getExponent(uint64_t request) {
	uint8_t exponent = MIN_EXPONENT;
	uint64_t size = MIN_ALLOC;
	while (size < request) {
		exponent++;
		size <<= 1;
	}
	return exponent;
}

static int64_t lookFreeNode(uint8_t exponent) {
	MemoryManagerADT memoryManager = getMemoryManager();
	uint64_t node = getNode(exponent);
	uint64_t lastnode = getNode(exponent - 1);
	while (node < lastnode && (memoryManager->tree[node].use == USED || memoryManager->tree[node].use == SPLIT)) {
		node++;
	}
	return (node == lastnode) ? -1 : node;
}

static void treeUpdate(uint64_t node) {
	setSplit(node);
	setChildren(node);
}

static void setSplit(uint64_t node) {
	MemoryManagerADT memoryManager = getMemoryManager();
	while (node != 0) {
		node = ((node - 1) >> 1);
		memoryManager->tree[node].use = SPLIT;
	}
}

static void setChildren(uint64_t node) {
	MemoryManagerADT memoryManager = getMemoryManager();
	if (node >= NODES)
		return;
	memoryManager->tree[node].use = USED;
	setChildren((node << 1) + 2);
	setChildren((node << 1) + 1);
}

static void releaseNode(uint64_t node) {
    MemoryManagerADT memoryManager = getMemoryManager();
    memoryManager->tree[node].use = 0; 

    while (node != 0) {
        uint64_t parent = (node - 1) >> 1;
        uint64_t left = (parent << 1) + 1;
        uint64_t right = (parent << 1) + 2;

       
        if (memoryManager->tree[left].use == 0 && memoryManager->tree[right].use == 0) {
            memoryManager->tree[parent].use = 0;
            node = parent;
        } else {
            break;
        }
    }
}


char * getMemoryType(){
	MemoryManagerADT memoryManager = getMemoryManager();
	return memoryManager->memoryType;
}

int getUsedMemory(){
	MemoryManagerADT memoryManager = getMemoryManager();
	return memoryManager->usedMemory; 
}


int getFreeMemory(){
	MemoryManagerADT memoryManager = getMemoryManager();
	return memoryManager->totalMemory - memoryManager->usedMemory;
}