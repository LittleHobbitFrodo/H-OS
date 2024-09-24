#
#   interrupts.asm
#       part of the CORE kernel belonging to the H-OS project
#

.intel_syntax noprefix

.macro INTERRUPT_ENTER
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
.endm

.macro INTERRUPT_LEAVE
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
.endm

.section .text
.global isr_int_timer_pit
.extern interrupt_timer_pit





isr_int_timer_pit:
	INTERRUPT_ENTER
	call interrupt_timer_pit
	INTERRUPT_LEAVE
	iretq
