//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"
#include "./lib/builtin-shell.h"

//TODO: create TODO text file

//TODO:	write driver for NVME

//TODO: fix paging (again)

//TODO: heap redo: atomic, non-linear address space

//TODO:	redo PCI init
//	static vector disks should be diskptr_vector

//TODO: implement hashmap (you lazy idiot :D)

//TODO: implement limine device tree blob feature

//TODO: (someday) resolve SIMD and GPRs
//	-mavx512f?
//	for faster page table copying


void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	print("nvm_capabilities_t size:\t"); printu(sizeof(nvm_capabilities_t)); endl();


	shell();

}
