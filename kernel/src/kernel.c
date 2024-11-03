//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

char* logo[5] = {".__",
	"|  |___",
	"|  |   |",
	"|   \\  |",
	"|___|__|"};

//TODO: map reserved areas (+ heaps)
	//	only those areas needed for runtime


//TODO: (someday) resolve SIMD and GPRs
	//	-mavx512f?
	//	for faster page table copying


void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	string input;
	vector cmd;

	bool running = true;

	do {

		print("user <-> H-OS: ");

		input = keyboard_getline(true);
		if (input.data == null) {
			continue;
		}


		str_pushc(&input, '\0');
		str_tokenize(input.data, &cmd);
		str_clear(&input);

		if (cmd.data == null) {
			continue;
		}

		string* str = vec_at(&cmd, 0);

		output.color = col.blue;
		if (str_cmpb(str, "help")) {
			printl("commands:");
			output.color = col.white; print("time"); output.color = col.blue; printl(":\t\tdisplay current time");
			output.color = col.white;print("echo"); output.color = col.blue; printl(":\t\tprints text");
			printl("\t-no-linebreak switch will disable line braking between tokens\n");
			output.color = col.white; print("exit"); output.color = col.blue; printl(":\t\tcloses shell and turn the computer off");
			output.color = col.white; print("clear"); output.color = col.blue; printl(":\t\tclears screen");
			output.color = col.white; print("memmap"); output.color = col.blue; printl(":\t\tdisplays memory map");
			printl("\t\'original\' subcommand prints memory map given by bootloader (if available)\n");
			output.color = col.white; print("microfetch"); output.color = col.blue; printl(":\t\tdisplays device info with style");
			output.color = col.white; print("shutdown"); output.color = col.blue; printl(":\t\tturns the computer down");
		} else if (str_cmpb(str, "time")) {
			time_update();
			char* out = format_time((timespec_t*)&timespec, time_format_str);
			print("time:\t"); printl(out);
			free(out);
		} else if (str_cmpb(str, "echo")) {
			if (cmd.len < 2) {
				vec_clear(&cmd, (void (*)(void*))str_clear);
				continue;
			}
			if (str_cmpb(((string*)vec_at(&cmd, 1)), "-no-linebreak")) {
				for (size_t i = 2; i < cmd.len; i++) {
					prints(((string*)vec_at(&cmd, i))); printc(' ');
				}
			} else {
				for (size_t i = 1; i < cmd.len; i++) {
					printsl(((string*)vec_at(&cmd, i)));
				}
			}
		} else if (str_cmpb(str, "exit")) {
			running = false;
		} else if (str_cmpb(str, "clear")) {
			screen_flush();
		} else if (str_cmpb(str, "memmap")) {
			if ((cmd.len > 1) && (str_cmpb(&str[1], "original"))) {
				memmap_display_original();
			} else {
				memmap_display();
			}
		} else if (str_cmpb(str, "microfetch")) {
			printl(logo[0]);
			print(logo[1]); output.color = col.white; print("\t\t\tOS:\t\t"); printl(KERNEL_NAME);
			time_update();
			char* tm = format_time((timespec_t*)&timespec, time_format_str);
			output.color = col.blue; print(logo[2]); output.color = col.white; print("\t\ttime:\t"); printl(tm);
			free(tm);
			output.color = col.blue; print(logo[3]); output.color = col.white; print("\t\tmemory:\t");
			size_t mem = meminfo.usable, mul = KB;
			for (;mem / mul >= 1024; mul *= 1024);
			printu(mem / mul);
			switch (mul) {
				case KB: {
					print("KB ");
					break;
				}
				case MB: {
					print("MB ");
					break;
				}
				case GB: {
					print("GB ");
					break;
				}
				case ((size_t)1024 * 1024 * 1024 * 1024): {
					print("TB ");
				}
				default: break;
			}
			print(" / ");
			mul = KB;
			mem = meminfo.used;
			for (;mem / mul >= 1024; mul *= 1024);
			printu(mem / mul);
			switch (mul) {
				case KB: {
					print("KB ");
					break;
				}
				case MB: {
					print("MB ");
					break;
				}
				case GB: {
					print("GB ");
					break;
				}
				case ((size_t)1024 * 1024 * 1024 * 1024): {
					print(" TB ");
				}
				default: break;
			}
			endl();

			output.color = col.blue; printl(logo[4]);
		} else if (str_cmpb(str, "shutdown")) {
			shutdown();
		} else if (str_cmpb(str, "heap")) {
			if ((cmd.len > 1) && (str_cmpb(&str[1], "page"))) {
				page_heap_debug();
			} else {
				heap_debug();
			}
		} else if (str_cmpb(str, "smbios")) {
			if (!smbios.supported) {
				report("smbios is not supported\n", report_error);
				continue;
			}
			if ((cmd.len > 1) && (str_cmpb(&str[1], "tables"))) {
				smbios_tableptr_t* tables = smbios.tables.data;
				print("detected "); printu(smbios.tables.len); printl(" tables");
				if (smbios.structure_table != null) {
					print("\tstructure table:\t"); printp(smbios.structure_table); endl();
				} else {
					report("structure table not found\n", report_error);
				}

				for (size_t i = 0; i < smbios.tables.len; i++) {
					printu(i); print(":\t");
					switch (tables[i].type) {
						case smbios_header_bios: {
							printl("bios");
							break;
						}
						case smbios_header_system: {
							printl("system");
							break;
						}
						case smbios_header_mainboard: {
							printl("mainboard");
							break;
						}
						case smbios_header_cpu: {
							printl("cpu");
							break;
						}
						case smbios_header_slots: {
							print("slots");
							break;
						}
						default: {
							printl("other");
							break;
						}
					}
				}
			} else {

			}
		} else {
			output.color = col.white;
			report("unrecognized command \'", report_error);
			prints(str); printl("\'\n");
		}
		output.color = col.white;
		vec_clear(&cmd, (void (*)(void*))str_clear);
	} while (running);

}