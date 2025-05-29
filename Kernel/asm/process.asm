GLOBAL setUpStackFrame


section .text

; setUpStackFrame(basePointer, rip, argc, argv, originalEntryPoint)
; rdi = basePointer (stack top, alineado)
; rsi = rip (entry point)
; rdx = argc
; rcx = argv
; r8 = originalEntryPoint

setUpStackFrame:
    mov r9, rsp 	; Preservar rsp
	mov r10, rbp		; Preservar rbp
    
    mov     rsp, rdi         ; rsp = basePointer (ya alineado)
    push    0x0              ; SS (no usado en modo kernel, pero requerido por iretq)
    push    rdi              ; RSP (valor inicial del stack del proceso)
    push    0x202            ; RFLAGS (interrupt enable)
    push    0x8              ; CS (kernel code segment)
    push    rsi              ; RIP (entry point)

	; Seteamos los argumentos para processWrapper
	mov rdi, r8      ; originalEntryPoint
    mov rsi, rdx     ; argc
    mov rdx, rcx     ; argv

    push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
    
    mov     rax, rsp         ; return value: nuevo rsp
	
    mov	 	rbp,r10	; rbp = basePointer original (para restaurar al salir del proceso)
	mov     rsp, r9         ; restaurar rsp original (para que no se pierda el stack de kernel)
    
	ret