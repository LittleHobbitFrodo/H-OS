//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

void kernel() {


	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");

	for (u8 i = 0; i < 20; i++) {
		print("tick:\t"); printu(tick); endl();
		for (u32 ii = 0; ii < MAX_U16; ii++);
	}

}