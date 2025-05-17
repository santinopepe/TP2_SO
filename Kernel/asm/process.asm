GLOBAL setUpStackFrame

section .text

setUpStackFrame:
    push rbp
    mov rbp, rsp

    ; Alinear rsp a 16 bytes
    mov rsp, rdi
    and rsp, -16

    ; Configurar stack frame para iret
    push 0x0        ; SS (selector de pila, modo kernel)
    push rdi        ; RSP original
    push 0x202      ; RFLAGS (interrupciones habilitadas)
    push 0x8        ; CS (selector de código, ajusta según GDT)
    push rsi        ; RIP (dirección inicial)

    ; Inicializar registros en 0
    push 0          ; rax
    push 0          ; rbx
    push 0          ; rdi
    push 0          ; rsi
    push 0          ; rbp
    push 0          ; rdx
    push 0          ; rcx
    push 0          ; r8
    push 0          ; r9
    push 0          ; r10
    push 0          ; r11
    push 0          ; r12
    push 0          ; r13
    push 0          ; r14
    push 0          ; r15

    ; Pasar argc (rdx) y argv (rcx) al stack
    push rcx        ; argv
    push rdx        ; argc

    ; Asegurar alineación a 16 bytes
    and rsp, -16

    ; Retornar el nuevo valor de rsp
    mov rax, rsp
    mov rsp, rbp
    pop rbp
    ret