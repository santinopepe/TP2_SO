#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <interrupts.h>
#include <video.h>
#include <test_mm.h>
#include <test_list.h>
#include <MemoryManager.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const heapStart = (void*)0x600000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void initializeKernelBinary()
{

	
	void * moduleAddresses[] = { sampleCodeModuleAddress, sampleDataModuleAddress };
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	createMemoryManager(heapStart, HEAP_SIZE); 
	
}

int main()
{	
	load_idt();
    
/* Agregue esto para tener la posiblidad de ejecutar el test_mm.c dentro de kernel space*/
#ifdef TEST_MM
    char max_mem_str[] = "16777216"; // 16MB
    char *test_argv[] = { max_mem_str, NULL }; 
    uint64_t test_argc = 1;
    printf("Starting Memory Manager test...\n"); 
    uint64_t test_result = test_mm(test_argc, test_argv);
    printf("Memory Manager test finished.\n"); 

    if (test_result != 0) {
        printf("--- Test del Memory Manager: FAILED ----\n"); 
        while(1) _hlt(); 
    } else {
        printf("--- Test del Memory Manager: PASSED ----\n"); 
    }
#endif

#ifdef TEST_LIST
	char *test_argv[] = { NULL }; 
	uint64_t test_argc = 0;
	printf("Starting List test...\n"); 
	uint64_t test_result = test_list(test_argc, test_argv);
	if(test_result != 0) {
		printf("--- Test de la lista: FAILED ----\n"); 
		while(1) _hlt(); 
	} else {
		printf("--- Test de la lista: PASSED ----\n"); 
	}
	printf("List test finished.\n"); 
#endif

	((EntryPoint)sampleCodeModuleAddress)();

    
	while(1) _hlt();
	return 0;
}

