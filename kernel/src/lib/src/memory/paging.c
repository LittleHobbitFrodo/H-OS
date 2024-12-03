//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

#include "../../k_management.h"

#define WAIT_INT (MAX_U16 << 10)

void page_init() {

	if (req_page_mode.response == null) {
		report("cannot get paging mode, continuing blind\n", report_warning);
	} else if (req_page_mode.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
		report("5 level paging mode is not supported\n", report_critical);
		panic(panic_code_unsupported_paging_mode);
		__builtin_unreachable();
	}

	//	0)	initialize pages structure
	if ((req_k_address.response == null) || (req_page_hhdm.response == null)) {
		report("cannot get virtual and physical base addresses\n", report_critical);
		panic(panic_code_base_addresses_not_available);
		__builtin_unreachable();
	}

	//memnull(&pages, sizeof(pages_t));
	pages.hhdm = req_page_hhdm.response->offset;
	pages.kernel.physical = (void*)req_k_address.response->physical_base;
	pages.kernel.virtual = (void*)req_k_address.response->virtual_base;

	req_k_address.response = null;
	req_page_hhdm.response = null;
	size_t pml4_addr;
	asm volatile("mov %0, cr3" : "=r"(pml4_addr));
	pages.pml4 = (void*)(pml4_addr + (size_t)pages.hhdm);

	//	1)	find place for both heaps
	if (!heap_reserve_memory(false) || !page_heap_reserve_memory()) {
		report("failed to find space for kernel heap", report_critical);
		panic(panic_code_cannot_allocate_memory_for_kernel_heap);
	}

}

void va_info(virtual_address address) {
	union virtual_union vu;
	vu.virtual_address = address;
	if (address.sign > 0) {
		print("higherhalf address ");
	} else {
		print("lowerhalf address ");
	}
	printp(vu.voidptr); printl(":");
	print("\tpml4:\t"); printu(address.pml4); endl();
	print("\tpdpt:\t"); printu(address.pdpt); endl();
	print("\tpd:\t\t"); printu(address.pd); endl();
	print("\tpt:\t\t"); printu(address.pt); endl();
	print("\toffset:\t"); printu(address.offset); endl();
}

void* physical(virtual_address address) {
	page_entry ent = (*pages.pml4)[address.pml4];
	if ((!ent.present) || (ent.address == 0)) {
		#ifndef KERNEL_DEBUG
		printl("\t\tphysical():\tpage not present: 3");
		#endif
		return null;
	}
	pages.pml4 = (page_table_t*)(page_address(ent));
	ent = (*pages.pml4)[address.pdpt];
	if ((!ent.present) || (ent.address == 0)) {
		printl("\t\tphysical():\tpage not present: 2");
		return null;
	}
	pages.pml4 = (page_table_t*)((size_t)ent.address << VA_SHIFT);
	ent = (*pages.pml4)[address.pd];
	if ((!ent.present) || (ent.address == 0)) {
		printl("\t\tphysical():\tpage not present: 1");
		return null;
	}
	pages.pml4 = (page_table_t*)((size_t)ent.address << VA_SHIFT);
	ent = (*pages.pml4)[address.pt];
	if ((!ent.present) || (ent.address == 0)) {
		printl("\t\tphysical:\tpage not present: 0");
		return null;
	}
	return (void*)((size_t)ent.address << VA_SHIFT);
}

