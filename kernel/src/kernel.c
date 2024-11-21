//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"
#include "./lib/builtin-shell.h"


//TODO: (someday) resolve SIMD and GPRs
	//	-mavx512f?
	//	for faster page table copying

//TODO: redo PCI enumerator (choose better structures?)
	//  make it more simple and reliable



void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;


	shell();

}
