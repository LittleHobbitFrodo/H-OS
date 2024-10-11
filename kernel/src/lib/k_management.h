//
//	k_management.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

extern void kernel();

extern const void* init();
	//	returns pointer to kernel stack

extern void shutdown();

void parse_cmd();

enum kernel_states {
	k_state_init_memory,
	k_state_init_interrupts,
	k_state_init_hardware,
} kernel_states;

enum vocal {
	vocality_stfu = 0,		//	critical errors only
	vocality_quiet_please = 1,	//	errors only
	vocality_normal = 2,	//	warnings
	vocality_vocal = 3,		//	warnings, important notes
	vocality_report_everything = 4	//	notes
} vocal;

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
	panic_code_gdt_initialization_failure,
	panic_code_unable_to_locate_RSDP,
	panic_code_unable_to_pick_acpi_memmap_entry,
	panic_code_failed_to_initialize_acpi
} panic_codes;

enum report_seriousness {
	//	report_seriousness corresponds with enum vocal
	//	stuff should be reported only if (vocality <= (size_t)seriousness)
	report_debug = 255,
	report_note = 4, //	notify user
	report_warning = 3, //	warn user
	report_problem = 2, //	warn user (serious)
	report_error = 1, //	report error
	report_critical = 0 //	report critical error (mostly system failure)
} report_seriousness;


extern void panic(enum panic_codes code);

void report(const char *msg, enum report_seriousness seriousness);


static enum vocal vocality = vocality_normal;

static enum kernel_states kernel_status = k_state_init_memory;