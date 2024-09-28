//
//	int-handler.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./interrupts.h"

extern void isr_int_timer_pit(void);

//extern void interrupt_timer_pit(int_stack_frame *frame);

//#define INTERRUPT_ENTER asm volatile("\npush rax\npush rbx\npush rcx\npush rdx\npush rsi\npush rdi\npush rbp\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\nmov rax, cr2\npush rax\ncld\nmov rdi, rsp");
//#define INTERRUPT_LEAVE asm volatile("add rsp, 8\npop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop rbp\npop rdi\npop rsi\npop rdx\npop rcx\npop rbx\npop rax");
