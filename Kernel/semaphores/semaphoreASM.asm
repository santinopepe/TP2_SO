GLOBAL acquire
GLOBAL release

section .text

acquire:
    mov al, 1
.retry:
    xchg [rdi], al
    test al, al
    jz .retry
    ret

release:
    mov al, 0
    mov [rdi], al
    ret