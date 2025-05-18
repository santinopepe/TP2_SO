global setUpStackFrame

section .text

global setUpStackFrame

section .text

; setUpStackFrame(basePointer, rip, argc, argv)
; rdi = basePointer (stack top, alineado)
; rsi = rip (entry point)
; rdx = argc
; rcx = argv

setUpStackFrame:
    mov     rsp, rdi         ; rsp = basePointer (ya alineado)
    push    0x0              ; SS (no usado en modo kernel, pero requerido por iretq)
    push    rdi              ; RSP (valor inicial del stack del proceso)
    push    0x202            ; RFLAGS (interrupt enable)
    push    0x8              ; CS (kernel code segment)
    push    rsi              ; RIP (entry point)

    ; Opcional: pasar argc y argv en los registros según tu convención
    ; Si tu primer instrucción es un wrapper que hace "call entry(argc, argv)", no necesitas pushearlos

    mov     rax, rsp         ; return value: nuevo rsp
    ret