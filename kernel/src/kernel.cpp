//
//  kernel.cpp
//      part of the H-OS project
//


#include "include.h"

extern "C" void set_rsp() {
	asm("mov %%rsp, %0" : "=r"(stack::kernel_ptr));
}

void* rrsp() {
	u64 rsp;
	asm("mov %0, %%rsp": "=r"(rsp) /*:: "CC"*/);
	return (void*)rsp;
}

extern "C" void kernel() {
	screen::flush();
	output << "hello world!" << out::endl;

	mem::map::display();

	output << "kstack:\t" << stack::kernel_ptr << out::endl;
	output << "istack:\t" << stack::interrupt_ptr << out::endl;
	output << "rsp: " << rrsp() << out::endl;


	//	TODO0
		//	apply GPL
		//	dodelat readme


	//	TODO1
		//	chack stack size (misscalculation ?)
		//	implement stack change

	//	TODO2
		//	setup stack for interrupt handling
			//	part of the last usable memmap entry
			//	4 kb
			//	change gdt, memmap
		//	implement TSS (only kstack and interrupt handler stack yet)
		//	implement IDT
			//	if interrupt routines will be more complex, make the ih stack bigger

}