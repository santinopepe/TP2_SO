// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <stdio.h>
#include <stdarg.h>
#include <sound.h>
#include <stdlib.h>

#define CURSOR_FREQ 10  /* Frecuencia en Ticks del dibujo del cursor*/

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3
#define MAX_CHARS 256

/**
 * @brief Funcion auxiliar para printf y printfc
 * @note La cantidad de parametros no es fija
 * @param fmt Formato de lo que se desea escribir de STDOUT
 * @param args lista de argumentos
 */
static void vprintf(char * fmt, va_list args);

void putchar(char c) {
    write(STDOUT, &c, 1);
}

void putcharErr(char c) {
    write(STDERR, &c, 1);
}

void puts(const char * s) {
    while (*s)
        putchar(*s++); 
}

void printErr(const char * s) {
    while (*s) putcharErr(*s++); 
}

int getchar() {
    char c;
    int ret = read(STDIN, &c, 1);
    if (ret == 1 ) { // Si se leyo un caracter o EOF
        return c;
    } else if (ret == 0) { 
        return 0;
    } 
    return EOF; 
}


void printf(char * fmt, ...) {
    va_list v;
    va_start(v, fmt);
    vprintf(fmt, v);
    va_end(v);
}

void vprintf(char * fmt, va_list args) {
    char buffer[MAX_CHARS] = {0};
    char * fmtPtr = fmt;
    while (*fmtPtr) {
 	    if (*fmtPtr == '%') {
            fmtPtr++;
            int dx = strtoi(fmtPtr, &fmtPtr);
            int len;

            switch (*fmtPtr) {
                case 'c':
                    putchar(va_arg(args, int));
                    break;
                case 'd':
                    len = itoa(va_arg(args, uint64_t), buffer, 10);
                    printNChars('0', dx-len);
                    puts(buffer);
                    break;
                case 'x':
                    len = itoa(va_arg(args, uint64_t), buffer, 16);
                    printNChars('0', dx-len);
                    puts(buffer);
                    break;
                case 's':
                    printNChars(' ', dx); // A diferencia %x y %d, la cantidad de espacios es igual al numero
                    puts((char *) va_arg(args, char *));
                    break;
            }
        } else {
            putchar(*fmtPtr);
        }
        fmtPtr++;
    }
}

void printfc(Color color, char * fmt, ...){    
    Color prevColor = getFontColor();
    setFontColor(color.r, color.g, color.b);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    setFontColor(prevColor.r, prevColor.g, prevColor.b);
}


void printNChars(char c, int n) {
    for (int i = 0; i < n; i++)
        putchar(c);
}



static char * _regNames[] = {"RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
void printRegisters(const uint64_t * rsp) {
    for (int i = 0; i < sizeof(_regNames)/sizeof(char *); i++)
        printf("%s: 0x%x\n", _regNames[i], rsp[sizeof(_regNames)/sizeof(char *)-i-1]);
}