//
//	exceptions.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

typedef struct exception_stack_frame {
	u64 cr2;
	u64 r15;
	u64 r14;
	u64 r13;
	u64 r12;
	u64 r11;
	u64 r10;
	u64 r9;
	u64 r8;
	u64 rbp;
	u64 rdi;
	u64 rsi;
	u64 rdx;
	u64 rcx;
	u64 rbx;
	u64 rax;
	u64 type;
	u64 code;
	u64 rip;
	u64 cs;
	u64 flags;
	u64 rsp;
	u64 ss;
} __attribute__((packed)) exception_stack_frame;

extern void handle_exception(exception_stack_frame *frame);

enum exceptions {
	exception_divide_by_zero = 0,
	exception_debug = 1,
	exception_nonmaskable_interrupt = 2,
	exception_breakpoint = 3,
	exception_overflow = 4,
	exception_out_of_range = 5,
	exception_invalid_opcode = 6,
	exception_device_not_available = 7,
	exception_double_fault = 8,
	exception_segment_overrun = 9, //	unused
	exception_invalid_tss = 10,
	exception_segment_not_present = 11,
	exception_stack_segment_fault = 12,
	exception_general_protection = 13,
	exception_page_fault = 14,
	exception_floating_point = 16,
	exception_alignment = 17,
	exception_machine_check = 18,
	exception_simd = 19,
	exception_virtualization = 20,
	exception_control_protection = 21,
	//	22 - 27 are reserved
	exception_hypervisor_injection = 28,
	exception_vmm_communication = 29,
	exception_security = 30,
} exceptions;

extern void isr_ex_divide_by_zero(void);

extern void isr_ex_debug(void);

extern void isr_ex_nonmaskable_interrupt(void);

extern void isr_ex_breakpoint(void);

extern void isr_ex_overflow(void);

extern void isr_ex_out_of_range(void);

extern void isr_ex_invalid_opcode(void);

extern void isr_ex_device_not_available(void);

extern void isr_ex_double_fault(void);

//	9 is unused
extern void isr_ex_invalid_tss(void);

extern void isr_ex_segment_not_present(void);

extern void isr_ex_stack_segment_fault(void);

extern void isr_ex_general_protection(void);

extern void isr_ex_page_fault(void);

//	15 is reserved
extern void isr_ex_floating_point(void);

extern void isr_ex_alignment(void);

extern void isr_ex_machine_check(void);

extern void isr_ex_simd(void);

extern void isr_ex_virtualization(void);

extern void isr_ex_control_protection(void);

//	22 - 27 are reserved
extern void isr_ex_hypervisor_injection(void);

extern void isr_ex_vmm_communication(void);

extern void isr_ex_security(void);

//	31 is reserved
