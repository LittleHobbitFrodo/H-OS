//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

extern void* KERNEL_START;

void kernel() {
	printl("hello world!");

	print("virtual base:\t"); printp(base_virtual); endl();
	print("physical base:\t"); printp(base_physical); endl();

	print("\n\nstack base:\t"); printp(kernel_stack_base); endl();

	print("\n\nused pml4 entries:\t");

	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if ((page_address(pml4[i])) != null) {
			printu(i); print(":\t"); printp(page_address(pml4[i])); print("\t\t"); printp(physical(page_address(pml4[i]))); endl();
		}
	}

	//memmap_display(false);

	/*print("kernel start:\t"); printp(base_virtual); endl();

	print("index: 4:\t"); printu(va_index(base_virtual, 3)); endl();
	print("index: 3:\t"); printu(va_index(base_virtual, 2)); endl();
	print("index: 2:\t"); printu(va_index(base_virtual, 1)); endl();
	print("index: 1:\t"); printu(va_index(base_virtual, 0)); endl();
	print("index: offset:\t"); printu(va_offset(base_virtual)); endl();*/
}