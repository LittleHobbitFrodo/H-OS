//
//	k_management.c
//		part of the CORE kernel belonging to the H-OS project
//

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

			output.color = col.green;
			printl("starting CORE initialization...");
			output.color = col.white;

			//	initialize cpu (cpuid)
			if (cpu_init() != ok_) {
				panic(cpu_vendor_not_found);
			}

			if (memmap_req.response == null) {
				panic(memmap_not_found);
			}

			enum panic_codes code;
			if ((code = memory_init()) != ok) {
				panic(code);
			}

			output.color = col.green;
			printl("CORE initialization completed...");
			print("starting ");
			printl(KERNEL_NAME);
			output.color = col.white;
		}


		void panic(enum panic_codes code) {
			output.color = col.critical;
			print("PANIC: CRITICAL ERROR");
			output.color = col.white;
			print(":\t");
			switch (code) {
				case memmap_not_found: {
					printl("memory map was not found");
					break;
				}
				case unsupported_paging_mode: {
					printl("current paging mode is not supported");
					break;
				}
				case cannot_allocate_memoey_for_kernel_heap: {
					printl("cannot allocate memory for kernel heap");
					break;
				}
				case cannot_locate_kernel_entry: {
					printl("cannot locate kernel entry");
					break;
				}
				case unable_to_allocate_paging_table: {
					printl("unable to allocate paging table");
					break;
				}
				case base_addresses_not_available: {
					printl("base addressed (physical and virtual) are not available");
					break;
				}
				case cannot_locate_kernel_stack: {
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
				case init_memory: {
					printl("FAILED TO INITIALIZE MEMORY");
					break;
				}
				case init_interrupts: {
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
				case note: {
					output.color = col.blue;
					print("NOTE");
					break;
				}
				case warning: {
					output.color = col.yellow;
					print("WARNING");
					break;
				}
				case problem: {
					output.color = col.orange;
					print("PROBLEM");
					break;
				}
				case error: {
					output.color = col.red;
					print("ERROR");
					break;
				}
				case critical: {
					output.color = col.critical;
					print("CRITICAL ERRROR");
					break;
				}
			}
			output.color = col.white;
			print(":\t");
			print(msg);
		}


		void __parse_cmd_out_of_bounds(const string* token) {
			report("command line argument error: expected anything for \'", problem);
			prints(token);
			printl("\', got nothing");
		}

		void parse_cmd() {
			struct limine_file* file = kernel_file.response->kernel_file;
			if (file == null) {
				report("kernel file (provided by bootloader) is NULL => no parameters taken\n", problem);
				return;
			}
			const char* cmd = file->cmdline;
			if (file->cmdline == null) {
				report("no given command line arguments\n", note);
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
							vocality = stfu;
						} else if (unlikely(str_cmpb(&s[i], "quiet-please"))) {
							vocality = quiet_please;
						} else if (unlikely(str_cmpb(&s[i], "normal"))) {
							vocality = normal;
						} else if (unlikely(str_cmpb(&s[i], "vocal"))) {
							vocality = vocal;
						} else if (unlikely(str_cmpb(&s[i], "report-everything"))) {
							vocality = report_everything;
							report("setting kernel vocality to \'report-everything\'\n", note);
						} else {
							report("command line argument error: unknown word \'", problem);
							prints(&s[i]);
							printl("\' for \'-vocality\' switch, setting default vocality to \'normal\'");
						}
					} else {
						__parse_cmd_out_of_bounds(&s[i-1]);
					}
				} else {
					report("command line argument error: unknown word \'", problem);
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

	#else
		#warning k_management.c already included
	#endif
#else
	#error k_management.c: k_management.h not included
#endif