//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"
#include "./lib/builtin-shell.h"

//TODO: create TODO text file


//TODO: push to main

//TODO: prioritize NVME controller inplementation (better real hw testing (for me at least))
//	write driver for NCME

//TODO: heap redo: atomic, non-linear address space

//TODO: AHCI: enumerate AHCI

//TODO: (someday) resolve SIMD and GPRs
//	-mavx512f?
//	for faster page table copying



void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;


	shell();

}
