//
//	builtin-shell.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

	//	act as header

static char* __shell_os_logo[5] = {".__",
	"|  |___",
	"|  |   |",
	"|   \\  |",
	"|___|__|"};

typedef struct shell_cmd_t {
	char** cmd;
	size_t size;
} shell_cmd_t;

static shell_cmd_t cmds[] = {{.cmd = (char*[]){"help"}, .size = 1},
	{.cmd = (char*[]){"time"}, .size = 1},
	{.cmd = (char*[]){"echo", "-no-linebreak"}, .size = 2},
	{.cmd = (char*[]){"exit"}, .size = 1},
	{.cmd = (char*[]){"clear"}, .size = 1},
	{.cmd = (char*[]){"memmap", "original", "new"}, .size = 3},
	{.cmd = (char*[]){"microfetch"}, .size = 1},
	{.cmd = (char*[]){"shutdown"}, .size = 1},
	{.cmd = (char*[]){"heap", "regular", "page"}, .size = 3}};
static size_t cmdc = 9;
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
	output.color = col.grey;
	print(str);
	output.column = column;
	output.color = bcol;
}

void shell() {
	//	some serious piece of shit code :D

	string input;
	vector tokens;

	str(&input);
	vecs(&tokens, sizeof(string));

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
			shell_cmd_t* completion_base = null;
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
						ssize_t i = (ssize_t)input.size - 1;
						for (; i > 0; i--) {
							if (input.data[i] == ' ') {
								word = &input.data[i+1];
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
						for (size_t i = 0; i < (size_t)word - (size_t)input.data; i++) {
							if ((word[i] < 'A') || (word[i] > 'z')) {
								wlen = i;
							}
						}
						if (wlen == 0) {
							wlen = input.size;
						}
					}
					if (wrd != 0) {
						continue;
					}

					for (size_t i = 0; i < cmdc; i++) {
						if (strncmpb(word, cmds[i].cmd[0], wlen)) {
							completion_base = &cmds[i];
							word = &completion_base->cmd[0][wlen];
							completion = word;
							break;
						}
					}

					if (completion_base != null) {
						size_t column = output.column;
						u32 bcol = output.color;
						output.color = col.grey;
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

		str_tokenize(input.data, &tokens);
		str_clear(&input);

		string* str = vec_at(&tokens, 0);

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
		} else if (str_cmpb(str, "time")) {		//	time
			time_update();
			char* out = format_time((timespec_t*)&timespec, time_format_str);
			print("time:\t"); printl(out);
			free(out);
		} else if (str_cmpb(str, "echo")) {		//	echo
			if (tokens.len < 2) {
				vec_clear(&tokens, (void (*)(void*))str_clear);
				continue;
			}
			if (str_cmpb(((string*)vec_at(&tokens, 1)), "-no-linebreak")) {
				for (size_t i = 2; i < tokens.len; i++) {
					prints(((string*)vec_at(&tokens, i))); printc(' ');
				}
			} else {
				for (size_t i = 1; i < tokens.len; i++) {
					printsl(((string*)vec_at(&tokens, i)));
				}
			}
		} else if (str_cmpb(str, "exit")) {		//	exit
			vec_clear(&tokens, (void (*)(void*))str_clear);
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
			printl(__shell_os_logo[0]);
			print(__shell_os_logo[1]); output.color = col.white; print("\t\t\tOS:\t\t"); printl(KERNEL_NAME);
			time_update();
			char* tm = format_time((timespec_t*)&timespec, time_format_str);
			output.color = col.blue; print(__shell_os_logo[2]); output.color = col.white; print("\t\ttime:\t"); printl(tm);
			free(tm);
			output.color = col.blue; print(__shell_os_logo[3]); output.color = col.white; print("\t\tmemory:\t");
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

			output.color = col.blue; printl(__shell_os_logo[4]);
		} else if (str_cmpb(str, "shutdown")) {		//	shutdown
			shutdown();
			__builtin_unreachable();
		} else if (str_cmpb(str, "heap")) {		//	heap
			if ((tokens.len > 1) && (str_cmpb(&str[1], "page"))) {
				page_heap_debug();
			} else {
				heap_debug();
			}
		} else {
			report("unknown command \"", report_error);
			print(str->data);
			printl("\"");
		}


		vec_clear(&tokens, (void (*)(void*))str_clear);
		output.color = col.green;
		print("used <=> H-OS: ");
	}

}