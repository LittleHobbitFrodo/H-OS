;;
;;  interrupts.asm
;;      part of the CORE kernel belonging to the H-OS project
;;

%macro INTERRUPT_ENTER 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rax, cr2
    push rax
    cld
    mov rdi, rsp
%endmacro

%macro INTERRUPT_LEAVE 0
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

section .text
	global isr_int_timer_pit
	extern tick

isr_int_timer_pit:
	;   increase tick
	push rax
	mov rax, [rel tick]
	inc rax
	mov [rel tick], rax
	pop rax
	iretq
