//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

//TODO:	test on real hw
	//	create -force kernel parameter and document it
		//	-force kb-scan-code 1
			//	forces scan code 1 (wont ask to change it)
void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");

	string input;
	vector cmd;

	bool running = true;

	do {

		print("enter your command:\t");

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