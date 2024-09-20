//
//	k_management.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_K_MANAGEMENT_H
	#define H_OS_LIB_K_MANAGEMENT_H

		extern void kernel();
		extern void init();

		void parse_cmd();

		enum kernel_states {
			kstate_init_memory,
			k_state_init_interrupts
		} kernel_states;

		enum vocal {
			vocality_stfu = 0,
			vocality_quiet_please = 1,
			vocality_normal = 2,
			vocality_vocal = 3,
			vocality_report_everything = 4
		} vocal;

		static enum vocal vocality = vocality_normal;

		static enum kernel_states kernel_state = kstate_init_memory;

		enum panic_codes {
			panic_code_ok,
			panic_code_memmap_not_found,
			panic_code_unsupported_paging_mode,
			panic_code_cannot_allocate_memory_for_kernel_heap,
			panic_code_cannot_locate_kernel_entry,
			panic_code_cannot_allocate_memory_for_stacks,
			panic_code_unable_to_allocate_paging_table,
			panic_code_base_addresses_not_available,
			panic_code_cpu_vendor_not_found,
			panic_code_cannot_locate_kernel_stack,
			panic_code_paging_initialization_failure,
			panic_code_gdt_initialization_failure
		} panic_codes;

		extern void panic(enum panic_codes code);

		enum report_seriousness {
				//	report_seriousness corresponds with enum vocal
					//	stuff should be reported only if (vocality <= (size_t)seriousness)
			report_note = 4,		//	notify user
			report_warning = 3,	//	warn user
			report_problem = 2,	//	warn user (serious)
			report_error = 1,		//	report error
			report_critical = 0	//	report critical error (mostly system failure)
		} report_seriousness;

		void report(const char* msg, enum report_seriousness seriousness);

#endif
//	#warning k_management.h already included
//#endif

