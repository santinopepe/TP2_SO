global setUpStackFrame

section .text

setUpStackFrame:
    push rbp 
    mov rbp, rsp

    mov rsp, rdi 
    and rsp, -16
    push 0x0
    push rdi
    push 0x202
    push 0x8
    push rsi

    push rax
    push rbx
    push rdi
    push rsi
    push rbp

    push rdx
    push rcx
    
    push r8
    push r9
    push r10
	push r11
	push r12
	push r13
	push r14
	push r15

    mov rax, rsp
    mov rsp, rbp
    pop rbp
    ret