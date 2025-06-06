#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <interrupts.h>
#include <video.h>
#include <test_list.h>
#include <MemoryManager.h>
#include <scheduler.h>
#include <process.h>
#include <semaphoresManager.h>
#include <pipeManager.h>
#include <globals.h>


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

    _cli();
    createMemoryManager(heapStart, HEAP_SIZE); 
    createScheduler();
	createPipeManager();
	create_semaphoresManager();
    _sti();

}

int main()
{	
	load_idt();	

    createScheduler();
	static char arg0[] = "shell";
	static char *argv[] = {arg0, NULL};
	
	createProcess((EntryPoint)sampleCodeModuleAddress, argv, 1, 2, fileDescriptors);
	

    
	while(1) _hlt();
	return 0;
}

