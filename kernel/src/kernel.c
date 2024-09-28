//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

//TODO:	more debug stuff
	//	paging
		//	port stacks and heap into virtual address space

void kernel() {

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");
	time_update();
	char* str = format_time((timespec_t*)&timespec, time_format_str);

	print("string: "); printl(str);

	free(str);

}