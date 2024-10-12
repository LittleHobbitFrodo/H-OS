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

void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	print("acpi version:\t"); printu(acpi.version); endl();

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
		if (str_cmpb(str, "time")) {
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
			if ((cmd.len >= 2) && (str_cmpb(&str[1], "original"))) {
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
		} else {
			output.color = col.white;
			report("unrecognized command \'", report_error);
			prints(str); printl("\'\n");
		}
		output.color = col.white;
		vec_clear(&cmd, (void (*)(void*))str_clear);
	} while (running);

}