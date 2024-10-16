//
//	exceptions.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../exceptions.h"

void handle_exception(exception_stack_frame *frame) {
	#ifdef KERNEL_DEBUG
	print("exception ");
	printu(frame->type);
	endl();
	#endif
	switch (frame->type) {
		case exception_divide_by_zero: {
			//	fault
			//	once in userspace -> kill process
			report("divide by zero\n", report_critical);
			hang();
			break;
		}
		case exception_debug: {
			report("debug exception\n", report_note);
			break;
		}
		case exception_nonmaskable_interrupt: {
			report("nonmaskable interrupt\n", report_error);
			break;
		}
		case exception_breakpoint: {
			report("breakpoint\n", report_note);
			break;
		}
		case exception_overflow: {
			report("overflow\n", report_error);
			hang();
			break;
		}
		case exception_out_of_range: {
			report("out of range\n", report_error);
			break;
		}
		case exception_invalid_opcode: {
			report("invalid opcode\n", report_critical);
			hang();
			break;
		}
		case exception_device_not_available: {
			report("device not available\n", report_error);
			break;
		}
		case exception_double_fault: {
			report("double fault\n", report_critical);
			hang();
			break;
		}
		//	segment overrun is unused
		case exception_invalid_tss: {
			report("invalid tss\n", report_critical);
			hang();
			break;
		}
		case exception_segment_not_present: {
			#ifdef KERNEL_DEBUG
			report("segment not present\n", report_error);
			print("rip:\t");
			printp((void *) frame->rip);
			endl();
			print("code:\t");
			printu(frame->code);
			endl();
			print("cs:\t"); printp((void*)frame->cs); endl();
			print("ss:\t"); printp((void*)frame->ss); endl();
			//u16 seg;
			//asm volatile("mov %0, ds" : "=r"(seg));
			//print("ds:\t"); printu(seg); endl();
			#endif
			//hang();


			if ((frame->cs & 0b11) == 0) {
				//	repair segments
				frame->cs = 0x08;
				frame->ss = 0x10;
				asm volatile("mov ax, 16\nmov ds, ax\nmov ss, ax\nmov ax, 0\nmov es, ax\nmov fs, ax\nmov gs, ax" ::: "rax");
			}
			break;
		}
		case exception_stack_segment_fault: {
			report("stack segment fault\n", report_critical);
			hang();
			break;
		}
		case exception_general_protection: {
			report("general protection\n", report_error);
			#ifdef KERNEL_DEBUG
			print("rip:\t");
			printp((void *) frame->rip);
			endl();
			print("code:\t");
			printu(frame->code);
			endl();
			print("cs:\t"); printp((void*)frame->cs); endl();
			print("ss:\t"); printp((void*)frame->ss); endl();
			#endif
			hang();
			break;
		}
		case exception_page_fault: {
			report("page fault", report_critical);
			if (frame->cr2 == 0) {
				print(" (NULL)");
			}
			endl();
			hang();
			break;
		}
		case exception_floating_point: {
			report("floating point exception\n", report_error);
			break;
		}
		case exception_alignment: {
			report("alignment exception\n", report_error);
			break;
		}
		case exception_machine_check: {
			report("machine check\n", report_error);
			break;
		}
		case exception_simd: {
			report("simd exception\n", report_error);
			break;
		}
		case exception_virtualization: {
			report("virtualization exception\n", report_error);
			break;
		}
		case exception_control_protection: {
			report("control protection exception\n", report_error);
			break;
		}
		case exception_hypervisor_injection: {
			report("hypervisor injection\n", report_critical);
			hang();
			break;
		}
		case exception_vmm_communication: {
			report("vmm communication exception\n", report_error);
			break;
		}
		case exception_security: {
			report("security exception\n", report_error);
			break;
		}
		default: {
			printl("unknown exception\n");
			hang();
			break;
		}
	}
}
