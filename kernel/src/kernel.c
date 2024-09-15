//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"

//	paging TODO:
	//	check if all reserved bits are same
		//Bits marked as reserved must all be set to 0, otherwise, a page fault will occur with a reserved error code.
		//	https://wiki.osdev.org/Paging

#define PAGE_ALIGN_DOWN(x) (((x) / PAGE_SIZE)*PAGE_SIZE)

void* virt_to_phys(void* addr) {
	size_t KERNEL_MEMORY_MASK = 0xfffffffffff00000;
	uint16_t pml1 = ((size_t)addr >> (12   )) & 0x1ff;
	uint16_t pml2 = ((size_t)addr >> (12+9 )) & 0x1ff;
	uint16_t pml3 = ((size_t)addr >> (12+18)) & 0x1ff;
	uint16_t pml4_ = ((size_t)addr >> (12+27)) & 0x1ff;
	page_entry* pml3_addr = (page_entry*)PAGE_ALIGN_DOWN(pml4[pml4_]);
	if(!pml3_addr) return 0;
	pml3_addr = (page_entry*)((uintptr_t)pml3_addr | KERNEL_MEMORY_MASK);

	page_entry* pml2_addr = (page_entry*)PAGE_ALIGN_DOWN(pml3_addr[pml3]);
	if(!pml2_addr) return 0;
	pml2_addr = (page_entry*)((uintptr_t)pml2_addr | KERNEL_MEMORY_MASK);

	page_entry* pml1_addr = (page_entry*)PAGE_ALIGN_DOWN(pml2_addr[pml2]);
	if(!pml1_addr) return 0;

	pml1_addr = (page_entry*)((uintptr_t)pml1_addr | KERNEL_MEMORY_MASK);

	return (void*)PAGE_ALIGN_DOWN(pml1_addr[pml1]);
}


void kernel() {

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	printl("hello world!");

}