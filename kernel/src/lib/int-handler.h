//
//	int-handler.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_INT_HANDLER_H
	#define H_OS_LIB_INT_HANDLER_H

	__attribute__((interrupt, target("general-regs-only"))) static void interrupt_timer_pit(int_stack_frame* frame);

	#define INTERRUPT_ENTER asm volatile("push rax\npush rbx\npush rcx\npush rdx\npush rsi\npush rdi\npush rbp\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\nmov rax, cr2\npush rax\ncld\nmov rdi, rsp");
	#define INTERRUPT_LEAVE asm volatile("add rsp, 8\npop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop rbp\npop rdi\npop rsi\npop rdx\npop rcx\npop rbx\npop rax");

	//	leave routine: deleted ```add rsp, 0x10``` and iretq instruction

#endif