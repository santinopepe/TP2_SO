GLOBAL write
GLOBAL read
GLOBAL getSeconds
GLOBAL clear
GLOBAL getInfoReg
GLOBAL setFontSize
GLOBAL getScreenResolution
GLOBAL drawRect
GLOBAL getTicks
GLOBAL getMemory
GLOBAL playSound
GLOBAL kaboom
GLOBAL setFontColor
GLOBAL getFontColor
GLOBAL malloc
GLOBAL free
GLOBAL getMemoryInfo
GLOBAL sem_open
GLOBAL sem_close
GLOBAL sem_wait
GLOBAL sem_post
GLOBAL create_sem
GLOBAL openPipe
GLOBAL closePipe
GLOBAL writePipe
GLOBAL readPipe
GLOBAL createProcess
GLOBAL killProcess
GLOBAL setPriority
GLOBAL setStatus
GLOBAL unblockProcess
GLOBAL blockProcess
GLOBAL yield
GLOBAL getPid


read:
    mov rax, 0
    int 80h
    ret

write:
    mov rax, 1
    int 80h
    ret

clear:
    mov rax, 2
    int 80h
    ret

getSeconds:
    mov rax, 3
    int 80h
    ret

getInfoReg:
    mov rax, 4
    int 80h
    ret

setFontSize:
    mov rax, 5
    int 80h
    ret

getScreenResolution:
    mov rax, 6
    int 80h
    ret

drawRect:
    mov rax, 7
    mov r10, rcx
    int 80h
    ret

getTicks:
    mov rax, 8
    int 80h
    ret

getMemory:
    mov rax, 9
    int 80h
    ret

playSound:
    mov rax, 10
    int 80h
    ret

setFontColor:
    mov rax, 11
    int 80h
    ret

getFontColor:
    mov rax, 12
    int 80h
    ret

malloc: 
    mov rax, 13
    int 80h
    ret

free:
    mov rax, 14
    int 80h
    ret

getMemoryInfo:
    mov rax, 15
    int 80h
    ret

sem_open:
    mov rax, 16
    int 80h
    ret

sem_close
    mov rax, 17
    int 80h
    ret

sem_wait:
    mov rax, 18
    int 80h
    ret

sem_post:
    mov rax, 19
    int 80h
    ret

create_sem:
    mov rax, 20
    int 80h
    ret

openPipe:
    mov rax, 21
    int 80h
    ret

closePipe:
    mov rax, 22
    int 80h
    ret

writePipe:
    mov rax, 23
    int 80h
    ret

readPipe:
    mov rax, 24
    int 80h
    ret

createProcess:
    mov rax, 25
    int 80h
    ret

killProcess:
    mov rax, 26
    int 80h
    ret

setPriority:
    mov rax, 27
    int 80h
    ret

setStatus:
    mov rax, 28
    int 80h
    ret

unblockProcess:
    mov rax, 29
    int 80h
    ret

blockProcess:
    mov rax, 30
    int 80h
    ret

yield:
    mov rax, 31
    int 80h
    ret

getPid:
    mov rax, 32
    int 80h
    ret
    