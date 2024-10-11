//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(kerninfo.name);
	output.color = col.white;

	printl("hello world!");

	print("pml4:\t"); printp(pages.pml4.ptr); endl();
	for (u16 i = 0; i < 1000; i++) {asm volatile("hlt");}

	print("resolved:\t");
	page_entry ent = ((page_entry*)pages.pml4.ptr)[va_index(pages.base.virtual, 3)];
	if ((!(ent & pf_present)) || (page_address(ent) == null)) {
		report("layer 4 (pml4)", report_error);
		return;
	}
	print("4:\t"); printp((void*)ent); endl();
	for (u16 i = 0; i < 1000; i++) {asm volatile("hlt");}
	ent = ((page_entry*)page_address(ent))[va_index(pages.base.virtual, 2)];
	if ((!(ent & pf_present)) || (page_address(ent) == null)) {
		report("layer 3 (pdpt)", report_error);
		return;
	}
	print("3:\t"); printp((void*)ent); endl();
	for (u16 i = 0; i < 1000; i++) {asm volatile("hlt");}
	ent = ((page_entry*)page_address(ent))[va_index(pages.base.virtual, 1)];
	if ((!(ent & pf_present)) || (page_address(ent) == null)) {
		report("layer 2 (pd)", report_error);
		return;
	}
	print("2:\t"); printp((void*)ent); endl();
	for (u16 i = 0; i < 1000; i++) {asm volatile("hlt");}
	ent = ((page_entry*)page_address(ent))[va_index(pages.base.virtual, 0)];
	if ((!(ent & pf_present)) || (page_address(ent) == null)) {
		report("layer 1 (pt)", report_error);
		printp((void*)ent); endl();
		return;
	}
	print("physical address:\t"); printp(page_address(ent)); endl();



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
		} else if (str_cmpb(str, "memmap")) {
			memmap_display();
		} else if (str_cmpb(str, "exit")) {
			running = false;
		} else {
			output.color = col.white;
			report("unrecognized command \'", report_error);
			prints(str); printl("\'\n");
		}
		endl();
		output.color = col.white;
		vec_clear(&cmd, (void (*)(void*))str_clear);
	} while (running);

}