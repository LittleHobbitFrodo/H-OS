//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

extern void* KERNEL_START;

void kernel() {

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");
}