//
//	k_management.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../k_management.h"

void init() {
	//	gather information about framebuffer
	screen_init();
	//	flush screen
	screen_flush();

	//	initialize font (bad implementation)
	font_init();

	//	initialize output structure
	output_init();

	#ifdef KERNEL_DEBUG
		report("starting in DEBUG mode\n", report_debug);
	#endif

	//	parse command line arguments
	parse_cmd();
	//	better parse cmd earlier

	//	initialize memory
	memory_init();

	//	initialize interrupts
	interrupts_init();

	//	basic hardware initialization
	hardware_init();

	//	reclaim memory
	//memmap_reclaim();
}


void panic(enum panic_codes code) {
	output.color = col.critical;
	print("PANIC: CRITICAL ERROR");
	output.color = col.white;
	print(":\t");
	switch (code) {
		case panic_code_memmap_not_found: {
			printl("memory map was not found");
			break;
		}
		case panic_code_unsupported_paging_mode: {
			printl("current paging mode is not supported");
			break;
		}
		case panic_code_cannot_allocate_memory_for_kernel_heap: {
			printl("cannot allocate memory for kernel heap");
			memmap_display_original();
			break;
		}
		case panic_code_cannot_locate_kernel_entry: {
			printl("cannot locate kernel entry");
			break;
		}
		case panic_code_unable_to_allocate_paging_table: {
			printl("unable to allocate paging table");
			break;
		}
		case panic_code_base_addresses_not_available: {
			printl("base addressed (physical and virtual) are not available");
			break;
		}
		case panic_code_cannot_locate_kernel_stack: {
			printl("cannot locate kernel stack");
			break;
		}
		case panic_code_paging_initialization_failure: {
			printl("memory protection initialization failed");
			break;
		}
		case panic_code_gdt_initialization_failure: {
			printl("legacy memory protection initialization failed");
			break;
		}
		case panic_code_unable_to_locate_RSDP: {
			printl("unable to locate ACPI tables");
			break;
		}
		case panic_code_unable_to_pick_acpi_memmap_entry: {
			printl("unable to pick ACPI memory map entry");
			break;
		}
		case panic_code_failed_to_initialize_acpi: {
			printl("failed to initialize ACPI");
			break;
		}
		case panic_code_acpi_validation_failed: {
			printl("ACPI data structure validation failed");
			break;
		}
		default: {
			printl("unknown critical error");
			break;
		}
	}

	print("\t->\t");
	output.color = col.critical;
	switch (kernel_status) {
		case k_state_init_memory: {
			printl("FAILED TO INITIALIZE MEMORY");
			break;
		}
		case k_state_init_interrupts: {
			printl("FAILED TO INITIALIZE INTERRUPTS");
			break;
		}
		case k_state_init_hardware: {
			printl("FAILED TO INITIALIZE HARDWARE");
			break;
		}
		default: break;
	}

	output.color = col.white;
	printl("\n\nhalting system...");
	hang();

	__builtin_unreachable();
}

void report(const char *msg, enum report_seriousness seriousness) {
	switch (seriousness) {
		case report_note: {
			output.color = col.blue;
			print("NOTE");
			break;
		}
		case report_warning: {
			output.color = col.yellow;
			print("WARNING");
			break;
		}
		case report_problem: {
			output.color = col.orange;
			print("PROBLEM");
			break;
		}
		case report_error: {
			output.color = col.red;
			print("ERROR");
			break;
		}
		case report_critical: {
			output.color = col.critical;
			print("CRITICAL ERROR");
			break;
		}
		case report_debug: {
			output.color = col.cyan;
			print("DEBUG");
			break;
		}
	}
	output.color = col.white;
	print(":\t");
	print(msg);
}

void shutdown() {
	output.color = col.green;
	printl("THE SYSTEM IS SHUTTING DOWN ... ");

	for (size_t i = 0; i < 1000; i++) {
		asm volatile("hlt");
	}

	if (uefi.supported) {
		uefi_reset(uefi__shutdown);
	} else {
		outw(0x604, 0x2000);	//	shutdown?
	}
	__builtin_unreachable();
}


void __parse_cmd_out_of_bounds(const string *token) {
	report("command line argument error: expected anything for \'", report_problem);
	prints(token);
	printl("\', got nothing");
}

#ifndef KERNEL_DEBUG
__attribute__((always_inline)) inline
#endif
void __parse_cmd_report(const char* msg, enum report_seriousness seriousness) {
	report("command line argument:\t", seriousness);
	print(msg);
}

size_t __parse_cmd_next_switch(const char* cmd, size_t len, char token[CMD_MAX_TOKEN_LEN], size_t start) {
	memnull(token, CMD_MAX_TOKEN_LEN);

	size_t i = start;

	//	find start of token
	for (; (cmd[i] != '-') && (i < len); i++);
	++i;

	//	write token into token variable
	for (size_t ii = 0; (((cmd[i] >= 'A') && (cmd[i] <= 'z')) || (cmd[i] == '-')) && (i < len) && (ii < CMD_MAX_TOKEN_LEN-1); ii++, i++) {
		token[ii] = cmd[i];
	}

	return i - start;
}

size_t __parse_cmd_next_token(const char* cmd, size_t len, char token[CMD_MAX_TOKEN_LEN], size_t start) {
	memnull(token, CMD_MAX_TOKEN_LEN);

	size_t i = start;

	for (; ((cmd[i] < 'A') || (cmd[i] > 'z')) && (i < len); i++);

	for (size_t ii = 0; (((cmd[i] >= 'A') && (cmd[i] <= 'z')) || (cmd[i] == '-')) && (i < len) && (ii < CMD_MAX_TOKEN_LEN-1); ii++, i++) {
		token[ii] = cmd[i];
	}

	return i - start;
}

void parse_cmd() {
	//	current work: make it not depend on heap

	char token[CMD_MAX_TOKEN_LEN];

	if ((req_kernel_file.response == null) || (req_kernel_file.response->kernel_file == null)) {
		report("unable to find kernel command line arguments -> using default values\n", report_warning);
		return;
	}
	const char* cmd = req_kernel_file.response->kernel_file->cmdline;
	const size_t len = strlen(cmd);
	if (len <= 1) {
		//	no parameters given
		return;
	}

	for (size_t i = 0; i < len; i++) {
		i += __parse_cmd_next_switch(cmd, len, token, i);
		//	parse token
		if (strcmpb(token, "vocality")) {

			i += __parse_cmd_next_token(cmd, len ,token, i);
			if (strcmpb(token, "stfu")) {
				vocality = vocality_stfu;
			} else if (strcmpb(token, "quiet-please")) {
				vocality = vocality_quiet_please;
			} else if (strcmpb(token, "normal")) {
				vocality = vocality_normal;
			} else if (strcmpb(token, "vocal")) {
				vocality = vocality_vocal;
			} else if (strcmpb(token, "report-everything")) {
				vocality = vocality_report_everything;
				report("setting kernel vocality to report everything\n", report_note);
			} else {
				__parse_cmd_report("unknown word \"", report_problem);
				print(token); printl("\" for switch \"-vocality\"");
			}
		} else if (strcmpb(token, "kaslr")) {

			i += __parse_cmd_next_token(cmd, len, token, i);

			if (strcmpb(token, "enable")) {
				kaslr = true;
				if (vocality >= vocality_report_everything) {
					report("enabling kernel address space randomization\n", report_note);
				}
			} else if (strcmpb(token, "disable")) {
				if (vocality >= vocality_report_everything) {
					report("disabling kernel address space randomization\n", report_note);
				}
			} else {
				__parse_cmd_report("unknown workd \"", report_problem);
				print(token); printl("\" for switch \"-kaslr\"");
			}

		}

	}


	return;

	struct limine_file *file = req_kernel_file.response->kernel_file;
	if (file == null) {
		report("kernel file (provided by bootloader) is NULL => no parameters taken\n", report_problem);
		return;
	}
	const char *cmd_ = file->cmdline;
	if (file->cmdline == null) {
		report("no command line arguments given\n", report_note);
		return;
	}

	//	tokenize cmd
	strvec_t tokens_;	//	strings
	strvec_construct(&tokens_, 0);
	str_tokenize(cmd_, &tokens_);

	string *s = tokens_.data;

	for (size_t i = 0; i < tokens_.len; i++) {
		if (str_cmpb(&s[i], "-vocality")) {
			if (++i < tokens_.len) {
				if (unlikely(str_cmpb(&s[0], "stfu"))) {
					vocality = vocality_stfu;
				} else if (unlikely(str_cmpb(&s[i], "quiet-please"))) {
					vocality = vocality_quiet_please;
				} else if (unlikely(str_cmpb(&s[i], "normal"))) {
					vocality = vocality_normal;
				} else if (unlikely(str_cmpb(&s[i], "vocal"))) {
					vocality = vocal;
				} else if (unlikely(str_cmpb(&s[i], "report-everything"))) {
					vocality = vocality_report_everything;
					report("setting kernel vocality to \'report-everything\'\n", report_note);
				} else {
					__parse_cmd_report("unknown word \'", report_problem);
					prints(&s[i]);
					printl("\' for \'-vocality\' switch, setting default vocality to \'normal\'");
				}
			} else {
				__parse_cmd_out_of_bounds(&s[i - 1]);
			}
		} else if (str_cmpb(&s[i], "-kaslr")) {
			if (++i < tokens_.len) {
				if (likely(str_cmpb(&s[i], "enable"))) {
					if (vocality >= vocality_vocal) {
						report("enabling KASLR\n", report_note);
					}
					kaslr = true;
				} else if (unlikely(str_cmpb(&s[i], "disable"))) {
					if (vocality >= vocality_vocal) {
						report("disabling KASLR\n", report_note);
					}
					kaslr = false;
				} else {
					__parse_cmd_report("expected \'enable\' or \'disable\' for \'", report_problem);
					prints(&s[i]);
					printl("\', skipping");
				}
			} else {
				__parse_cmd_out_of_bounds(&s[i - 1]);
			}
		} else {
			__parse_cmd_report("unknown word \'", report_problem);
			prints(&s[i]);
			printl("\', skipping");
		}
	}

	//	free vector memory
	strvec_destruct(&tokens_);
}
