//
//	exceptions.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_EXCEPTIONS_H
	#ifndef H_OS_LIB_EXCEPTIONS_C
		#define H_OS_LIB_EXCEPTIONS_C

		__attribute__((target("general-regs-only"))) void handle_exception(exception_stack_frame* frame) {
			print("exception "); printu(frame->type); endl();
			switch (frame->type) {
				case exception_divide_by_zero: {
					//	fault
					//	once in userspace -> kill process
					report("divide by zero\n", report_critical);
					hang();
					break;
				}
				case exception_debug: {

					report("debug exception", report_note);
					break;
				}
				case exception_nonmaskable_interrupt: {

					report("nonmaskable interrupt", report_error);
					break;
				}
				case exception_breakpoint: {

					report("breakpoint", report_note);
					break;
				}
				case exception_overflow: {

					report("overflow", report_error);
					hang();
					break;
				}
				case exception_out_of_range: {

					report("out of range", report_error);
					break;
				}
				case exception_invalid_opcode: {
					report("invalid opcode", report_critical);
					hang();
					break;
				}
				case exception_device_not_available: {
					report("device not available", report_error);
					break;
				}
				case exception_double_fault: {
					report("double fault", report_critical);
					hang();
					break;
				}
				//	segment overrun is unused
				case exception_invalid_tss: {
					report("invalid tss", report_critical);
					hang();
					break;
				}
				case exception_segment_not_present: {
					report("segment not present", report_error);
					break;
				}
				case exception_stack_segment_fault: {
					report("stack segment fault", report_error);
					break;
				}
				case exception_general_protection: {
					report("general protection", report_error);
					hang();
					break;
				}
				case exception_page_fault: {
					report("page fault", report_critical);
					break;
				}
				case exception_floating_point: {
					report("floating point exception", report_error);
					break;
				}
				case exception_alignment: {
					report("alignment exception", report_error);
					break;
				}
				case exception_machine_check: {
					report("machine check", report_error);
					break;
				}
				case exception_simd: {
					report("simd exception", report_error);
					break;
				}
				case exception_virtualization: {
					report("virtualization exception", report_error);
					break;
				}
				case exception_control_protection: {
					report("control protection exception", report_error);
					break;
				}
				case exception_hypervisor_injection: {
					report("hypervisor injection", report_critical);
					hang();
					break;
				}
				case exception_vmm_communication: {
					report("vmm communication exception", report_error);
					break;
				}
				case exception_security: {
					report("security exception", report_error);
					break;
				}
				default: {
					printl("unknown exception\n");
					hang();
					break;
				}
			}
		}

	#endif
#else
	#error exceptions.c: exceptions.h not included
#endif