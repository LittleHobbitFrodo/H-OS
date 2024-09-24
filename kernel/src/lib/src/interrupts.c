//
//	interrupts.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_INTERRUPTS_H
	#ifndef H_OS_LIB_INTERRUPTS_C
		#define H_OS_LIB_INTERRUPTS_C

		void interrupts_init() {

			//	set all idt entries to null
			for (size_t i = 0; i < IDT_SIZE; i++) {
				idt_null(&idt[i]);
				idt[i].flags = idt_gate_type_interrupt;
				idt[i].cs = 0x8;	//	index 1
			}
			for (size_t i = 0; i < 33; i++) {
				idt[i].flags |= idt_bits_present;
			}

			//	prepare idt pointer
			idt_pointer.address = (u64)&idt;
			idt_pointer.size = (IDT_USED_SIZE * sizeof(idt_entry)) - 1;

			idt_set_address(&idt[0], (void*)&isr_ex_divide_by_zero);
			idt_set_address(&idt[1], (void*)&isr_ex_debug);
			idt_set_address(&idt[2], (void*)&isr_ex_nonmaskable_interrupt);
			idt_set_address(&idt[3], (void*)&isr_ex_breakpoint);
			idt_set_address(&idt[4], (void*)&isr_ex_overflow);
			idt_set_address(&idt[5], (void*)&isr_ex_out_of_range);
			idt_set_address(&idt[6], (void*)&isr_ex_invalid_opcode);
			idt_set_address(&idt[7], (void*)&isr_ex_device_not_available);
			idt_set_address(&idt[8], (void*)&isr_ex_double_fault);
			//	9 is unused
			idt_set_address(&idt[10], (void*)&isr_ex_invalid_tss);
			idt_set_address(&idt[11], (void*)&isr_ex_segment_not_present);
			idt_set_address(&idt[12], (void*)&isr_ex_stack_segment_fault);
			idt_set_address(&idt[13], (void*)&isr_ex_general_protection);
			idt_set_address(&idt[14], (void*)&isr_ex_page_fault);
			//	15 is reserved
			idt_set_address(&idt[16], (void*)&isr_ex_floating_point);
			idt_set_address(&idt[17], (void*)&isr_ex_alignment);
			idt_set_address(&idt[18], (void*)&isr_ex_machine_check);
			idt_set_address(&idt[19], (void*)&isr_ex_simd);
			idt_set_address(&idt[20], (void*)&isr_ex_virtualization);
			idt_set_address(&idt[21], (void*)&isr_ex_control_protection);
			//	22 - 27 are reserved
			idt_set_address(&idt[28], (void*)&isr_ex_hypervisor_injection);
			idt_set_address(&idt[29], (void*)&isr_ex_vmm_communication);
			idt_set_address(&idt[30], (void*)&isr_ex_security);
			//	31 is reserved

			//	timer interrupt
			idt_set_address(&idt[32], (void*)&isr_int_timer_pit);

			asm volatile("lidt %0" :: "m"(idt_pointer));

			pic_remap();		//	reconfigure interrupt controller

			asm volatile("sti");

			if ((vocality >= vocality_report_everything)) {
				report("interrupt initialization completed\n", report_note);
			}
		}

	#endif
#else
	#error interrupts.c: interrupts.h not included
#endif