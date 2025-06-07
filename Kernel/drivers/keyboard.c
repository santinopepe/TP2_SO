#include <keyboard.h>
#include <stdint.h>
#include <lib.h>
#include <video.h>
#include <time.h>
#include <semaphoresManager.h>
#include <scheduler.h>
#include <stdlib.h>
#include <time.h>
#include <globals.h>
#include <MemoryManager.h>

#define BUFFER_CAPACITY 10                      /* Longitud maxima del vector _buffer */
#define LCTRL 29
#define LSHIFT 42
#define C_HEX 0x2E
#define D_HEX 0x20
#define R_HEX 0x13
#define M_HEX 0x32
#define RELEASED 0x80 /* Mascara para detectar si se solto una tecla */
#define SHIFTED 0x80

/* Scancode para el snapshot de registros */
static uint8_t _bufferStart = 0;                /* Indice del comienzo de la cola */
static char _bufferSize = 0;                    /* Longitud de la cola */
static uint8_t _buffer[BUFFER_CAPACITY] = {0};  /* Vector ciclico que guarda las teclas 
                                                 * que se van leyendo del teclado */

static uint8_t _ctrl = 0;					  /* Flag para detectar si se presiono ctrl */
static uint8_t _shift = 0;					  /* Flag para detectar si se presiono shift */

static const char charHexMap[] =			  /* Mapa de scancode a ASCII */
	{0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
	 '=', '\b', ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	 '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
	 ';', '\'', 0, 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
	 '.', '/', 0, '*', 0, ' '};

static const char charHexMapShift[] = /* Mapa de scancode con shift a ASCII */
	{0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
	 '+', '\b', ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	 '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	 ';', '"', 0, 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
	 '>', '?', 0, '*', 0, ' '};

/**
 * @brief  Obtiene el indice del elemento en la cola dado un corrimiento 
 * @param  offset: corrimiento
 * @return Indice del elemento en la cola
 */

static void writeKey(uint8_t key);

static void printMem(); 

static void printQuantityBars(uint64_t total, uint64_t consumed); 

static int getBufferIndex(int offset){
    return (_bufferStart+offset)%(BUFFER_CAPACITY);
}

void initializeKeyboardDriver(){
    create_sem(IO_SEM_ID, 0); // Inicializa el semáforo de IO
}

void keyboardHandler(){
    uint8_t key = getKeyPressed();
    if(!(key & RELEASED)){ // Si la tecla no fue liberada
        if(key==LCTRL){
            _ctrl = 1;
        }
        else if(key==LSHIFT){
            _shift=1;
        }
        else if(_ctrl){
            if(key==C_HEX){ //ctrl+C
                _bufferStart = _bufferSize = 0;
                int ret =  killForegroundProcess();
                if(ret == -1){
                    printf("No se pudo matar a la shell\n");
                }else if(ret == 0){
                    printf("^C\n");
                }
                
            }
            else if(key==R_HEX){ //ctrl+R
                saveRegisters();
            }
            else if(key==D_HEX && _bufferSize < BUFFER_CAPACITY - 1){ //ctrl+D
                writeKey(EOF);
            }
            else if(key==M_HEX){ //ctrl+M
                printMem();
            }
        }
        else if(_bufferSize < BUFFER_CAPACITY -1){ // Si el buffer no esta lleno
            if (_shift) {
                key = SHIFTED | key; // Si se presiono shift, se agrega el flag
            }
            writeKey(key); // Se escribe la tecla en el buffer
        }
    }
    else{
            if(key == (LCTRL | RELEASED)){
                _ctrl = 0;
            }
            else if(key == (LSHIFT | RELEASED)){
                _shift = 0;
            }
    }
}

static void writeKey(uint8_t key){
    if (((key & 0x7F) < sizeof(charHexMap) && charHexMap[key & 0x7F] != 0) || (int) key == EOF) {
		_buffer[getBufferIndex(_bufferSize)] = key;
		_bufferSize++;
		sem_post(IO_SEM_ID);
	}
}

uint8_t getScancode() {
    if(_bufferSize > 0){
        char c = _buffer[getBufferIndex(0)];
        _bufferStart = getBufferIndex(1);
        _bufferSize--;
        return c;
    }
    return 0;
}

uint8_t getAscii(){
    sem_wait(IO_SEM_ID);
    int scanCode = getScancode();
    if(scanCode == EOF){
        return EOF;
    }
    if(SHIFTED & scanCode){
        scanCode &= 0x7F;
        return charHexMapShift[(int) scanCode];
    }
    return charHexMap[(int) scanCode];
}

static void printMem(){

    uint64_t usedMemory = getUsedMemory();
    uint64_t freeMemory = getFreeMemory();
    uint64_t totalMemory = usedMemory + freeMemory;

    char *memoryType = getMemoryType();
    
    printf("Tipo de memoria: %s\n", memoryType);

    if(strcmp(memoryType, "Bitmap") == 0)
    {
        printf("Bloques totales: %d \n", totalMemory);
        printQuantityBars(totalMemory, totalMemory);
        printf("Bloques libres: %d \n", freeMemory);
        printQuantityBars(freeMemory, totalMemory);
        printf("Bloques usados: %d \n", usedMemory);
        printQuantityBars(totalMemory, usedMemory);
    } else{
        printf("Memoria total: %d bytes\n", totalMemory);
        printQuantityBars(totalMemory, totalMemory);
        printf("Memoria libre: %d bytes\n", freeMemory);
        printQuantityBars(totalMemory, freeMemory);
        printf("Memoria usada: %d bytes\n", usedMemory);
        printQuantityBars(totalMemory, usedMemory);
    }

}

static void printQuantityBars(uint64_t total, uint64_t consumed){
    int percentage = 0;
    if (total > 0) {
        percentage = (int)(((uint64_t)consumed * 100) / total);
    }

    
    if (percentage < 0) {
        percentage = 0;
    }
    if (percentage > 100) {
        percentage = 100;
    }

    int bars = percentage ; 

    printChar('[');
    for (int i = 0; i < bars; i++)
    {
        printChar('=');
    }
    for (int i = bars; i < 100; i++) 
    {
        printChar(' ');
    }
    printChar(']');
    printf(" %d", percentage); 
    printChar('%');
    printChar('\n');
}