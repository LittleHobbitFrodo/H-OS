//
//	int-handler.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_INT_HANDLER_H
	#define H_OS_LIB_INT_HANDLER_H

	//extern void isr_wrapper();

	/*__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_divide_by_zero(int_stack_frame* frame);			//	fault
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_debug(int_stack_frame* frame);					//	fault/trap
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_nonmaskable_interrupt(int_stack_frame* frame);	//	interrupt
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_breakpoint(int_stack_frame* frame);				//	trap
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_overflow(int_stack_frame* frame);					//	trap
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_out_of_range(int_stack_frame* frame);				//	fault
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_invalid_opcode(int_stack_frame* frame);			//	fault
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_device_not_available(int_stack_frame* frame);		//	fault
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_double_fault(exception_stack_frame* frame);		//	abort
	[[maybe_unused]] __attribute__((interrupt, target("general-regs-only"))) static void isr_ex_segment_overrun(int_stack_frame* frame);			//	unused?
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_invalid_tss(exception_stack_frame* frame);				//	fault
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_segment_not_present(exception_stack_frame* frame);		//	fault
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_stack_segment_fault(exception_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_general_protection_fault(exception_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_page_fault(exception_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_floating_point(int_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_alignment(exception_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_machine_check(int_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_simd(int_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_virtualization(int_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_control_protection(exception_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_hypervisor_injection(int_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_vmm_communication(exception_stack_frame* frame);
	__attribute__((interrupt, target("general-regs-only"))) static void isr_ex_security(exception_stack_frame* frame);*/



#define INTERRUPT_ENTER asm volatile("pushq rax\npushq rbx\npushq rcx\npushq rdx\npushq rsi\npushq rdi\npush rbp\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\nmov rdi, rsp" ::: "memory")
#define INTERRUPT_LEAVE asm volatile("pop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop rbp\npop rdi\npop rsi\npop rdx\npop rcx\npop rbx\npop rax" ::: "memory");


#endif