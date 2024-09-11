//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

//extern void* KERNEL_START;

void kernel() {

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");

	void* ptr = alloc(8);

	print("allocated memory: "); printu(*((size_t*)ptr)); endl();

	free(ptr);

}