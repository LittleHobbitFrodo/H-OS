//
//	builtin-shell.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

static char* __shell_os_logo[5] = {".__",
	"|  |___",
	"|  |   |",
	"|   \\  |",
	"|___|__|"};

static char* cmds[] = {"help", "time", "echo", "exit", "clear", "memmap", "microfetch", "shutdown", "heap", "devices"};
static size_t cmdc = 10;
	//	act as source

void __shell_flush_line_from(size_t line, size_t column) {
	for (; column * font.size < screen.w; column++) {
		u32* ptr = screen.address + ((line * screen.w * (font.size + output.space_between_lines))) + (column * font.size);
		for (u16 i = 0; i < font.size; i++) {
			for (size_t ii = 0; ii < font.size; ii++) {
				*(ptr + (i * screen.w) + (font.size - ii)) = 0;
			}
		}
	}
}

void __shell_hint(const char* str) {
	size_t column = output.column;
	u32 bcol = output.color;
	output.color = col.hint;
	print(str);
	output.column = column;
	output.color = bcol;
}

void shell() {
	//	some serious piece of shit code :D

	string input;
	strvec_t tokens;

	str(&input, &heap.global);
	strvec_construct(&tokens, 0, &heap.global);

	output.color = col.green;

	print("user <=> H-OS:\t");
	__shell_hint("the \'help\' command is here to help you");

	for (size_t i = 0;; i++) {

		output.color = col.white;

		//	getline
		{

			char c;
			size_t start = output.column;
			bool loop = true, updated = false;
			char* completion_base = null;
			char* completion = null;
			size_t wrd = 0;

			do {
				asm volatile("hlt");
				if (!keyboard_hit()) {
					continue;
				}
				c = keyboard_char();

				switch (c) {
					case '\n': {
						endl();
						loop = false;
						break;
					}
					case '\b': {
						if (output.column > start) {
							if (output.column > 0) {
								screen_flush_at(output.line, --output.column);
							}
							str_pop(&input, 1);
						}
						updated = true;
						break;
					}
					case '\t': {
						if (completion_base != null) {
							str_push(&input, completion);
							print(completion);
						}
						completion_base = null;
						completion = null;
						break;
					}
					default: {
						if ((c >= ' ') && (c <= '~')) {
							str_pushc(&input, c);
							if ((output.column+1) * font.size >= screen.w) {
								loop = false;
								break;
							}
							printc(c);
						}
						updated = true;
						break;
					}
				}

				if (updated) {
					completion = null;
					completion_base = null;
					//	find last word
					__shell_flush_line_from(output.line, output.column);
					if (output.column <= start) {
						continue;
					}
					char* word = null;
					{
						ssize_t ii = (ssize_t)input.size - 1;
						for (; ii > 0; ii--) {
							if (input.data[ii] == ' ') {
								word = &input.data[ii+1];
								wrd = 1;
								break;
							}
						}
					}
					if (word == null) {
						word = input.data;
					}
					size_t wlen = 0;
					{
						for (size_t ii = 0; ii < (size_t)word - (size_t)input.data; ii++) {
							if ((word[ii] < 'A') || (word[ii] > 'z')) {
								wlen = ii;
							}
						}
						if (wlen == 0) {
							wlen = input.size;
						}
					}
					if (wrd != 0) {
						continue;
					}

					for (size_t ii = 0; ii < cmdc; ii++) {
						if (strncmpb(word, cmds[ii], wlen)) {
							completion_base = cmds[ii];
							word = &completion_base[wlen];
							completion = word;
							break;
						}
					}

					if (completion_base != null) {
						size_t column = output.column;
						u32 bcol = output.color;
						output.color = col.hint;
						print(word);
						output.column = column;
						output.color = bcol;
					}
				}

			} while (loop);
		}


		if (input.data == null) {
			continue;
		}

		str_pushc(&input, '\0');

		str_tokenize(input.data, &tokens, &heap.global);
		str_clear(&input);

		string* str = strvec_at(&tokens, 0);

		if (str_cmpb(str, "help")) {		//	help
			printl("welcome to TBHSH (Temporary Built-in H-OS SHell)");
			printl("\tthis simple shell utility is made for debugging/troubleshooting reasons and will be removed someday");
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
			output.color = col.white; print("pci"); output.color = col.blue; printl("displays information about PCI");
			printl("\t\'devices\' subcommand will list all PCI devices");
		} else if (str_cmpb(str, "time")) {		//	time
			time_update();
			char* out = format_time((timespec_t*)&timespec, time_format_str);
			print("time:\t"); printl(out);
			heap.global.free(&heap.global, out);
		} else if (str_cmpb(str, "echo")) {		//	echo
			if (tokens.len < 2) {
				strvec_destruct(&tokens);
				continue;
			}
			string* at;
			if (str_cmpb((strvec_at(&tokens, 1)), "-no-linebreak")) {
				for (size_t ii = 2; ii < tokens.len; ii++) {
					at = strvec_at(&tokens, ii);
					printn(at->data, at->size); printc(' ');
				}
				endl();
			} else {
				for (size_t ii = 1; ii < tokens.len; ii++) {
					at = strvec_at(&tokens, ii);
					printn(at->data, at->size); endl();
				}
			}
		} else if (str_cmpb(str, "exit")) {		//	exit
			strvec_destruct(&tokens);
			return;
		} else if (str_cmpb(str, "clear")) {		//	clear
			screen_flush();
		} else if (str_cmpb(str, "memmap")) {		//	memmap
			if ((tokens.len > 1) && (str_cmpb(&str[1], "original"))) {
				memmap_display_original();
			} else {
				memmap_display();
			}
		} else if (str_cmpb(str, "microfetch")) {		//	microfetch
			output.color = col.blue;
			printl(__shell_os_logo[0]);
			print(__shell_os_logo[1]); output.color = col.white; print("\t\t\tOS:\t\t"); printl(KERNEL_NAME);
			time_update();
			char* tm = format_time((timespec_t*)&timespec, time_format_str);
			output.color = col.blue; print(__shell_os_logo[2]); output.color = col.white; print("\t\ttime:\t"); printl(tm);
			heap.global.free(&heap.global, tm);
			output.color = col.blue; print(__shell_os_logo[3]); output.color = col.white; print("\t\tmemory:\t");
			size_t mul, mem;

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
			print(" / ");
			mem = meminfo.usable;
			mul = KB;
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
			endl();
			output.color = col.blue; printl(__shell_os_logo[4]);
		} else if (str_cmpb(str, "shutdown")) {		//	shutdown
			shutdown();
			__builtin_unreachable();
		} else if (str_cmpb(str, "heap")) {		//	heap
			if ((tokens.len > 1) && (str_cmpb(&str[1], "page"))) {
				page_heap_debug();
			} else {
				heap_debug(&heap.global);
			}
		} else if (str_cmpb(str, "devices")) {
			/*if (devices.len == 0) {
				report("no devices found\n", report_error);
				continue;
			}

			device_t* device;
			for (size_t ii = 0; ii < devices.len; ii++) {
				device = devices_at(ii);
				output.color = col.blue;
				printu(ii); printc('\t');
				switch (device->type) {
					case device_type_undefined: {
						printl("undefined");
						break;
					}
					case device_type_unsupported: {
						printl("unsupported");
						break;
					}
					case device_type_disk: {
						printl("disk");
						output.color = col.white;
						if (device->ptr == null) {
							printl("NULL");
							break;
						}
						disk_t* disk = (disk_t*)device->ptr;
						print("type:\t");
						switch (disk->type) {
							case disk_type_undefined: {
								printl("undefined"); break;
							}
							case disk_type_ssd: {
								printl("ssd"); break;
							}
							case disk_type_nvm: {
								printl("NVM"); break;
							}
							case disk_type_hdd: {
								printl("HDD"); break;
							}
							case disk_type_unsupported: {
								printl("unsupported"); break;
							}
							default: {
								printl("unknown"); break;
							}
						}
						print("connection:\t");
						switch (disk->connect.type & 0xfff) {
							case device_connect_ahci: {
								printl("AHCI"); break;
							}
							case device_connect_ata_bus: {
								printl("ATA bus"); break;
							}
							case device_connect_nvm: {
								printl("Non-volatile memory controller"); break;
							}
							case device_connect_vendor_specific: {
								printl("vendor specific"); break;
							}
							case device_connect_unknown: {
								printl("unknown"); break;
							}
							default: {
								printl("unknown (default case)"); break;
							}
						}
						break;
					}
					case device_type_base_peripheral: {
						printl("base peripheral"); break;
					}
					case device_type_processor: {
						printl("processor"); break;
					}
					case device_type_display_controller: {
						printl("display controller"); break;
					}
					default: {
						printl("unknown device"); break;
					}
				}
			}*/

		} else {
			report("unknown command \"", report_error);
			print(str->data);
			printl("\"");
		}

		strvec_destruct(&tokens);
		output.color = col.green;
		print("used <=> H-OS: ");
	}

}