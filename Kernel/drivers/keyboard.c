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
            else if(key==D_HEX && _bufferSize < BUFFER_CAPACITY -1){ //ctrl+D
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
    MemoryInfoADT memInfo;
    memInfo = getMemoryInfo(memInfo); 

    if (memInfo == NULL)
    {
        printf("Error al obtener la informacion de memoria.\n");
        return;
    }

    printf("Tipo de memoria: %s\n", memInfo->memoryType);
    printf("Tamanio de pagina: %d bytes\n", memInfo->pageSize);
    printf("Total de paginas: %d\n", memInfo->totalPages);
    printf("Memoria total: %d bytes\n", memInfo->totalMemory);

    printChar('\n');

    
    int bar_width = 15; 

    if (memInfo->totalMemory > 0) // Evitar división por cero si totalMemory es 0
    {

        printf("Memoria total: %d bytes [", memInfo->totalMemory);
        int total_chars = (int)(((double)memInfo->totalMemory / memInfo->totalMemory) * bar_width);
        for (int i = 0; i < total_chars; i++)
        {
            printChar('='); // Usar un caracter diferente para la memoria total
        }
        for (int i = total_chars; i < bar_width; i++)
        {
            printChar(' '); // Rellenar el resto con espacios
        }
        printf("] 100.00%%\n");

        // Barra para memoria libre
        printf("Memoria libre: %d bytes [", memInfo->freeMemory);
        int free_chars = (int)(((double)memInfo->freeMemory / memInfo->totalMemory) * bar_width);
        for (int i = 0; i < free_chars; i++)
        {
            printChar('=');
        }
        for (int i = free_chars; i < bar_width; i++)
        {
            printChar(' '); // Rellenar el resto con espacios
        }
        // Calcular porcentaje para memoria libre
        uint64_t free_percentage_scaled = ((uint64_t)memInfo->freeMemory * 10000) / memInfo->totalMemory;
        printf("] %d.", (int)(free_percentage_scaled / 100)); // Parte entera
        if ((free_percentage_scaled % 100) < 10) {
            printChar('0'); // Añadir cero inicial si es necesario
        }
        printf("%d%%\n", (int)(free_percentage_scaled % 100)); // Parte decimal


        
        printf("Memoria usada: %d bytes [", memInfo->usedMemory);
        int used_chars = (int)(((double)memInfo->usedMemory / memInfo->totalMemory) * bar_width);
        for (int i = 0; i < used_chars; i++)
        {
            printChar('#'); 
        }
        for (int i = used_chars; i < bar_width; i++)
        {
            printChar(' '); 
        }
        uint64_t used_percentage_scaled = ((uint64_t)memInfo->usedMemory * 10000) / memInfo->totalMemory;
        printf("] %d.", (int)(used_percentage_scaled / 100)); // Parte entera
        if ((used_percentage_scaled % 100) < 10) {
            printChar('0'); 
        }
        printf("%d%%\n", (int)(used_percentage_scaled % 100));
    }
    else
    {
        printf("Memoria libre: %d bytes [ No disponible ]\n", memInfo->freeMemory);
        printf("Memoria usada: %d bytes [ No disponible ]\n", memInfo->usedMemory);
    }

    printChar('\n');
 
}