//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

//TODO:		reimplement gdt (with right datastructures lol)

void kernel() {

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");

}