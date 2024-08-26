//
//  kernel.cpp
//      part of the H-OS project
//


#include "include.h"

//void* rrbp() {
//	u64 rbp;
//	asm("mov %0, %%rbp" : "=r"(rbp));
//	return (void*)rbp;
//}

//void* rrsp() {
//    u64 rsp;
//    asm("mov %0, %%rsp": "=r"(rsp) /*:: "CC"*/);
//    return (void*)rsp;
//}


extern "C" void kernel() {
    output << "hello world!" << out::endl;

	output << "physical base: " << mem::base::physical << out::endl;
	output << "virtual base:  " << mem::base::virtual_ << out::endl;

	//	find stack
		//	reclaimable -> bigger than 65536 (65536 aligned to 4096)
			//	test -> rewrite data in reclaimable | crashes = OK


	//	TODO
		//	reverse paging lookup for stack position


	//	TODO
		//	apply GPL
		//	dodelat readme

	//	TODO
		//	setup stack for interrupt handling
			//	part of the last usable memmap entry
			//	4 kb
			//	change gdt, memmap
		//	implement TSS (only kstack and interrupt handler stack yet)
		//	implement IDT
			//	if interrupt routines will be more complex, make the ih stack bigger

}