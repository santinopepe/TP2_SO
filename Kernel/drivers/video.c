// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <video.h>
#include <lib.h>
#include <font.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define RGB_SIZE 3
#define MAX_RESOLUTION (64 * 128)   /* Longitud del buffer de caracteres */
#define MSG_BUFFER_EXCEEDED "Buffer de video excedido, la pantalla ha sido limpiada\n"

struct vbe_mode_info_structure {
    uint16_t attributes;        // en desuso, solo el bit 7 debería ser de su interés e indica que el modo admite un búfer de cuadro lineal.
    uint8_t window_a;           // en desuso
    uint8_t window_b;           // en desuso
    uint16_t granularity;       // en desuso; se utiliza al calcular los banks
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;      // en desuso; usado para cambiar banks de modo protegido sin volver a modo real
    uint16_t pitch;         // numero de bytes por linea horizontal
    uint16_t width;         // ancho en pixeles
    uint16_t height;            // altura en pixeles
    uint8_t w_char;         // unused...
    uint8_t y_char;         // ...
    uint8_t planes;
    uint8_t bpp;            // bits por pixel en este modo
    uint8_t banks;          // en desuso; numero total de banks en este modo
    uint8_t memory_model;
    uint8_t bank_size;      // en desuso; tamaño de un bank, casi siempre 64kb pero a veces 16kb
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer;       // direccion fisica del frame buffer lineal; escribi acá para dibujar en la pantalla
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;   // tamaño de memoria en el framebuffer pero sin ser mostrado en pantalla
    uint8_t reserved1[206];
} __attribute__ ((packed));

struct vbe_mode_info_structure* _screenData = (void*)0x5C00;
uint16_t _X = 0, _Y = 0;                /* Coordenadas de escritura de caracteres */
Color _fontColor = DEFAULT_COLOR;       /* Color de fuente */
uint8_t _charWidth = CHAR_WIDTH_12;     /* Ancho en pixeles de un caracter */
uint8_t _charHeight = CHAR_HEIGHT_12;   /* Altura en pixeles de un caracter */
char * _font = font_12;                 /* Mapa de bits de dibujo para los caracteres */
char _charBuffer[MAX_RESOLUTION];       /* Buffer de caracteres */
uint16_t _bufferIdx = 0;                /* Posicion de indice del buffer */

/**
 * @brief Reescribe los caracteres de _charBuffer
 * @note  Se llama cuando se reajusta el tamaño de fuente
 */
static void renderFonts();

/**
 * @brief  Obtiene la direccion de memoria del pixel (x,y) de la pantalla
 * @param  x: Coordenada x
 * @param  y: Coordenada y
 * @return Puntero al pixel (x,y)
 */
static void* getPtrToPixel(uint16_t x, uint16_t y);

static void* getPtrToPixel(uint16_t x, uint16_t y) {
    return (void*)(_screenData->framebuffer + RGB_SIZE * (x + (y * (uint64_t)_screenData->width)));
}

void videoClear() {
    void * pos = getPtrToPixel(0,0);
    memset(pos, 0, RGB_SIZE * (uint64_t)_screenData->width * _screenData->height);
    _X = _Y = 0;
    _bufferIdx = 0;
}

uint8_t coordinatesValid(uint16_t x, uint16_t y) {
    return x < _screenData->width && y < _screenData->height;
}

void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color) {
    if (!coordinatesValid(x, y))
        return;
    
    uint16_t maxWidth = _screenData->width - x;
    if (width > maxWidth) width = maxWidth;
    
    uint16_t maxHeight = _screenData->height - y;
    if (height > maxHeight) height = maxHeight;

    Color* ptr = (Color*)getPtrToPixel(x, y);
    uint16_t lineDiff = _screenData->width - width;
    for (int i=0; i<height; i++) {
        for (int c=0; c<width; c++)
            *(ptr++) = color;
        ptr += lineDiff;
    }
}

void setPosition(uint16_t x, uint16_t y) {
    uint16_t maxX = _screenData->width - _charWidth;
    uint16_t maxY = _screenData->height - _charHeight;

    _X = x < maxX ? x : maxX;
    _Y = y < maxY ? y : maxY;
}

void setFontColor(Color color) {
    _fontColor = color;
}

Color getFontColor(){
    return _fontColor;
}

void setFontSize(fontSize f){
    if(f >= FONT_12 && f <= FONT_36){
        _font = fonts[f];
        _charWidth = charWidths[f];
        _charHeight = charHeights[f];
        renderFonts();
    }
}

static void renderFonts() {
    int buffIdx = _bufferIdx;
    videoClear();
    printN(_charBuffer, buffIdx);
}

void printNewline(void) {
    _X = 0; 

    if (_Y + 2 * _charHeight <= _screenData->height) {
        _Y += _charHeight;
    } else {
        uint64_t len = RGB_SIZE * ((uint64_t)_screenData->width * (_screenData->height - _charHeight));
        memcpy(getPtrToPixel(0,0), getPtrToPixel(0, _charHeight), len);
        memset(getPtrToPixel(0, _screenData->height - _charHeight), 0, RGB_SIZE * (uint64_t)_screenData->width * _charHeight);
    }
}

void printChar(int c) {
    if (c == '\b') { // Borrar el caracter anterior 
        if (_X < _charWidth && _Y > 0) { 
            _Y -= _charHeight;
            _X = (_screenData->width / _charWidth) * _charWidth - _charWidth;
        } else {
            _X -= _charWidth;
        }
        drawRect(_X, _Y, _charWidth, _charHeight, BLACK);
        _bufferIdx--;
        return;
    }

    if (_bufferIdx == MAX_RESOLUTION) {
        videoClear();
        print(MSG_BUFFER_EXCEEDED);
    }

    _charBuffer[_bufferIdx++] = c;
    if (c == '\n') {
        printNewline();
        return;
    }
    
    if (c >= FIRST_CHAR && c <= LAST_CHAR) {
        /* Puntero al Bitmap de dibujo del caracter recibido */
	    const char* data = _font + _charHeight * _charWidth * (c-FIRST_CHAR) / 8;   
	    for (int h=0; h<_charHeight; h++) { // Iteracion por filas
    		Color* ptr = (Color*)getPtrToPixel(_X, _Y + h);
            uint8_t mask = 1;
            for (uint8_t i = 0; i < _charWidth; i++) { // Iteracion por columnas
                if (*data & mask) {
                    ptr[i] = _fontColor;
                }
                
                if (mask & 0b10000000) {
                    mask = 0b00000001;
                    data++; // Pasa al siguiente byte dentro de una fila o de la siguiente 
                } else {
                    mask <<= 1; // Pasa al siguiente bit
                }
            }
    	}
    }
    _X += _charWidth;
    if (_X > (_screenData->width / _charWidth) * _charWidth - _charWidth)
        printNewline();
}

void print(const char* s) {
    while (*s)
        printChar(*s++);
}

void printN(const char* s, uint32_t n) {
    if (!n)
        return;
    while (n-- && *s)
        printChar(*s++);
}

void printf(char * fmt, ...) {
    va_list v;
    va_start(v, fmt);
    char buffer[256] = {0};
    char * fmtPtr = fmt;
    while (*fmtPtr) {
 	    if (*fmtPtr == '%') {
            fmtPtr++;
            int dx = strtoi(fmtPtr, &fmtPtr);
            int len;

            switch (*fmtPtr) {
                case 'c':
                    printChar(va_arg(v, int));
                    break;
                case 'd':
                    len = itoa(va_arg(v, uint64_t), buffer, 10);
                    printNChars('0', dx-len);
                    print(buffer);
                    break;
                case 'x':
                    len = itoa(va_arg(v, uint64_t), buffer, 16);
                    printNChars('0', dx-len);
                    print(buffer);
                    break;
                case 's':
                    printNChars(' ', dx); // A diferencia %x y %d, la cantidad de espacios es igual al numero
                    print((char *) va_arg(v, char *));
                    break;
            }
        } else {
            printChar(*fmtPtr);
        }
        fmtPtr++;
    }
    va_end(v);
}

void printNChars(char c, int n) {
    for (int i = 0; i < n; i++)
        printChar(c);
}

uint32_t getScreenResolution(){
    return _screenData->width | _screenData->height << 16;
}

void printHex(uint64_t n) {
    char buffer[17];
    int i = 16;
    buffer[i--] = '\0';
    if (n == 0) {
        printChar('0');
        return;
    }
    while (n > 0 && i >= 0) {
        int digit = n % 16;
        buffer[i--] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        n /= 16;
    }
    printChar('0');
    printChar('x');
    for (i = i + 1; buffer[i] != '\0'; i++) {
        printChar(buffer[i]);
    }
}