//
//	k_management.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_K_MANAGEMENT_H
	#define H_OS_LIB_K_MANAGEMENT_H

		extern void kernel();
		extern void init();

		static volatile struct limine_kernel_file_request kernel_file = {
			.id = LIMINE_KERNEL_FILE_REQUEST,
			.revision = 0
		};

		void parse_cmd();

		enum kernel_states {
			init_memory,
			init_interrupts
		} kernel_states;

		enum vocal {
			stfu = 0,
			quiet_please = 1,
			normal = 2,
			vocal_ = 3,
			report_everything = 4
		} vocal;

		static enum vocal vocality = normal;

		static enum kernel_states kernel_state = init_memory;

		enum panic_codes {
			ok,
			memmap_not_found,
			unsupported_paging_mode,
			cannot_allocate_memoey_for_kernel_heap,
			cannot_locate_kernel_entry,
			cannot_allocate_memory_for_stacks,
			unable_to_allocate_paging_table,
			base_addresses_not_available,
			cpu_vendor_not_found,
			cannot_locate_kernel_stack,
			paging_initialization_failure,
			gdt_initialization_failure
		} panic_codes;

		extern void panic(enum panic_codes code);

		enum report_seriousness {
				//	report_seriousness corresponds with enum vocal
					//	stuff should be reported only if (vocality <= (size_t)seriousness)
			note = 4,		//	notify user
			warning = 3,	//	warn user
			problem = 2,	//	warn user (serious)
			error = 1,		//	report error
			critical = 0	//	report critical error (mostly system failure)
		} report_seriousness;

		void report(const char* msg, enum report_seriousness seriousness);

#endif
//	#warning k_management.h already included
//#endif

