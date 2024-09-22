//
//	k_management.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_K_MANAGEMENT_H
	#ifndef H_OS_LIB_K_MANAGEMENT_C
		#define H_OS_LIB_K_MANAGEMENT_C

		extern void init() {

			//	gather information about framebuffer
			screen_init();
			//	flush screen
			screen_flush();

			//	initialize font (bad implementation)
			font_init();

			//	initialize output structure
			output_init();

			//	initialize cpu (cpuid)
			cpu_init();

			//	initialize memory
			memory_init();

			//	initialize interrupts
			idt_init();

			//	basic hardware initialization

			if (vocality >= vocality_report_everything) {
				report("proceeding to initialize hardware\n", report_warning);
				for (size_t i = 0; i < MAX_U16; i++);
			}

			hardware_init();

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
				default: {
					printl("unknown critical error");
					break;
				}
			}

			print("\t->\t");
			output.color = col.critical;
			switch (kernel_state) {
				case kstate_init_memory: {
					printl("FAILED TO INITIALIZE MEMORY");
					break;
				}
				case k_state_init_interrupts: {
					printl("FAILED TO INITIALIZE INTERRUPTS");
					break;
				}
			}

			output.color = col.white;
			printl("\n\nhalting system...");
			hang();
		}

		void report(const char* msg, enum report_seriousness seriousness) {
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
			}
			output.color = col.white;
			print(":\t");
			print(msg);
		}


		void __parse_cmd_out_of_bounds(const string* token) {
			report("command line argument error: expected anything for \'", report_problem);
			prints(token);
			printl("\', got nothing");
		}

		void parse_cmd() {
			struct limine_file* file = req_kernel_file.response->kernel_file;
			if (file == null) {
				report("kernel file (provided by bootloader) is NULL => no parameters taken\n", report_problem);
				return;
			}
			const char* cmd = file->cmdline;
			if (file->cmdline == null) {
				report("no given command line arguments\n", report_note);
				return;
			}

			//	tokenize cmd
			vector tokens;	//	strings
			vecs(&tokens, sizeof(string));

			{
				string token;
				str(&token);
				size_t tlen = 0;
				char* tstart = null;

				size_t cmdlen = strlen(cmd);

				for (size_t i = 0; i < cmdlen; ++i) {
					switch (cmd[i]) {
						case ' ': case '\t': case '\n': {
							//	push
							if (likely(tlen > 0)) {
								str_setss(&token, tstart, tlen);	//	copy data from cmdline
								string* nstr = vec_push(&tokens, 1);	//	push to strings
								str(nstr);		//	initialize string
								str_take_over(nstr, &token);	//	take over token string
							}
							tlen = 0;
							tstart = null;
							break;
						}
						case '\0': {
							//	important
							break;
						}
						default: {
							if (tstart == null) {
								tstart = (char*)cmd + i;
							}
							tlen++;
							break;
						}
					}
				}
				//	push if last word is not pushed
				if (likely(tlen > 0)) {
					str_setss(&token, tstart, tlen);	//	copy data from cmdline
					string* nstr = vec_push(&tokens, 1);	//	push to strings
					str(nstr);		//	initialize new string
					str_take_over(nstr, &token);	//	take over token string
				}

			}

			string* s = tokens.data;

			for (size_t i = 0; i < tokens.len; i++) {
				if (str_cmpb(&s[i], "-vocality")) {
					if (++i < tokens.len) {
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
							report("command line argument error: unknown word \'", report_problem);
							prints(&s[i]);
							printl("\' for \'-vocality\' switch, setting default vocality to \'normal\'");
						}
					} else {
						__parse_cmd_out_of_bounds(&s[i-1]);
					}
				} else {
					report("command line argument error: unknown word \'", report_problem);
					prints(&s[i]);
					printl("\', skipping");
				}
			}

			//	clear strings
			for (size_t i = 0; i < tokens.len; i++) {
				str_clear(&s[i]);
			}
			//	free vector memory
			vec_free(&tokens);

			return;
		}

	#endif
	//	#warning k_management.c already included
	//#endif
#else
	#error k_management.c: k_management.h not included
#endif