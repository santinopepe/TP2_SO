#include <stdint.h>
#include <video.h>
#include <keyboard.h>
#include <lib.h>
#include <color.h>
#include <speaker.h>
#include <time.h>
#include <registers.h>
#include <MemoryManager.h>
#include <process.h>
#include <pipeManager.h>
#include <semaphoresManager.h>
#include <scheduler.h>
#include <globals.h>
#include "../include/time.h"

/* File Descriptors*/
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

static int syscall_read(uint32_t fd, char * buffer, uint32_t size); 
static int syscall_write(uint32_t fd, char *buffer, uint64_t size);
static void syscall_clear();
static uint32_t syscall_seconds();
static uint64_t * syscall_registerArray(uint64_t * regarr);
static void syscall_fontSize(uint8_t size);
static uint32_t syscall_resolution();
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
static uint64_t syscall_getTicks();
static void syscall_getMemory(uint64_t pos, uint8_t * vec);
static void syscall_playSound(uint64_t frequency, uint64_t ticks);
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b);
static uint32_t syscall_getFontColor();

typedef uint64_t (*Syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t); 

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    static Syscall syscalls[] = {
        (Syscall)syscall_read,
        (Syscall)syscall_write,
        (Syscall)syscall_clear,
        (Syscall)syscall_seconds,
        (Syscall)syscall_registerArray,
        (Syscall)syscall_fontSize,
        (Syscall)syscall_resolution,
        (Syscall)syscall_drawRect,
        (Syscall)syscall_getTicks,
        (Syscall)syscall_getMemory,
        (Syscall)syscall_playSound,
        (Syscall)syscall_setFontColor,
        (Syscall)syscall_getFontColor,
        (Syscall)malloc,
        (Syscall)free,
        (Syscall)getMemoryInfo,
        (Syscall)sem_open,
        (Syscall)sem_close,
        (Syscall)sem_wait,
        (Syscall)sem_post,
        (Syscall)create_sem,
        (Syscall)openPipe,
        (Syscall)closePipe,
        (Syscall)writePipe,
        (Syscall)readPipe,
        (Syscall)createProcess,
        (Syscall)killProcess,
        (Syscall)setPriority,
        (Syscall)setStatus,
        (Syscall)unblockProcess,
        (Syscall)blockProcess,
        (Syscall)yield,
        (Syscall)getPid,
        (Syscall)waitForChildren,
        (Syscall)processInfo,
        (Syscall)wait_time,
        (Syscall)getMemoryType,
        (Syscall)getUsedMemory,
        (Syscall)getFreeMemory,
        (Syscall)changeFDS,
    };
	return syscalls[nr](arg0, arg1, arg2, arg3, arg4, arg5);
}

// Read char
static int syscall_read(uint32_t fd, char * buffer, uint32_t size){
    int16_t fdValue = fd < 3 ? getFileDescriptor(fd) : fd;
    if (fdValue < 0)
        return -1;

    if (fdValue == STDIN) {
        for (uint64_t i = 0; i < size; i++) {
            buffer[i] = getAscii();
            if ((int) buffer[i] == EOF)
                return i + 1;
        }
        return size;
    }
    else if (fdValue >= 3) {
        int ret = readPipe(fdValue, buffer, size);
        yield();
        return (ret >= 0) ? ret : -1;
    }
    return -1;
}


static int syscall_write(uint32_t fd, char *buffer, uint64_t size) {
    if (fd == STDERR)
        return 0; // O implementá lógica para STDERR

    int64_t fdProcess = fd < 3 ? getFileDescriptor(fd) : fd;
    if (fdProcess < 0)
        return -1;

    if (fdProcess == STDOUT) {
        for (int i = 0; i < size; i++) {
            printChar(buffer[i]);
        }
        return size;
    }
    else if (fdProcess >= 3) {
        int ret = writePipe(fdProcess, buffer, size);
        yield();
        return (ret >= 0) ? ret : -1;
    }
    return -1;
}

// Clear
static void syscall_clear(){
    videoClear();
}

// Get time in seconds
static uint32_t syscall_seconds(){
    uint8_t h, m, s;
    getTime(&h, &m, &s);
    return s + m * 60 + ((h + 24 - 3) % 24) * 3600;
}

// Get register snapshot array
static uint64_t * syscall_registerArray(uint64_t * regarr){
    uint64_t * snapshot = getLastRegSnapshot();
    for(int i = 0; i < QTY_REGS; i++)
        regarr[i] = snapshot[i];
    return regarr;
}

// Set fontsize
static void syscall_fontSize(uint8_t size){
    setFontSize(size - 1);
}

// Get screen resolution
static uint32_t syscall_resolution(){
    return getScreenResolution();
}

// DrawRect
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    ColorInt myColor = { bits: color };
    drawRect(x, y, width, height, myColor.color);
}

// GetTicks
static uint64_t syscall_getTicks(){
    return ticksElapsed();
}

//PrintMem
static void syscall_getMemory(uint64_t pos, uint8_t * vec){
    memcpy(vec, (uint8_t *) pos, 32);
}

//playSound
static void syscall_playSound(uint64_t frequency, uint64_t ticks){
    playSound(frequency, ticks);
}

//Set fontsize
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b){
    setFontColor((Color){b, g, r});
}

//Get fontsize
static uint32_t syscall_getFontColor(){
    ColorInt c = { color: getFontColor() };
    return c.bits;
}