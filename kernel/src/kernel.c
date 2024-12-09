//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"
#include "./lib/builtin-shell.h"


void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	print("nvm_capabilities_t size:\t"); printu(sizeof(nvm_capabilities_t)); endl();


	shell();

}
