//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

void kernel() {

	printl("kernel info:");
	print("version:\t\t"); printl(KERNEL_VERSION);
	print("kernel name:\t"); printl(KERNEL_NAME);
	print("build time\t"); printl(KERNEL_BUILD_TIME);
	print("build guid:\t"); printl(KERNEL_BUILD_GUID);



}