//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

//TODO: update cs (for real now)
	//	how tf do I use label declared in C?

//TODO:	kaslr enable/disable

void kernel() {

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");

	asm volatile("int 1");

}