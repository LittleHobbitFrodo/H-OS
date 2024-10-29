//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

#include "../../k_management.h"

page_table_t* page_find() {
	page_table_t* ret;
	asm volatile("mov %0, cr3" : "=r"(ret));
	return ret;
}

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

	memnull(&pages, sizeof(pages_t));
	pages.hhdm = (void*)req_page_hhdm.response->offset;
	pages.kernel.physical = (void*)req_k_address.response->physical_base;
	pages.kernel.virtual = (void*)req_k_address.response->virtual_base;

	req_k_address.response = null;
	req_page_hhdm.response = null;
	pages.pml4 = ((size_t)page_find() + pages.hhdm);

	//	1)	find place for both heaps
	if (!heap_reserve_memory(false) || !page_heap_reserve_memory()) {
		report("failed to find space for kernel heap", report_critical);
		panic(panic_code_cannot_allocate_memory_for_kernel_heap);
	}

}

/*void page_init() {
	//	allocate virtual addresses for heap, stack and reserved areas
	if (req_page_mode.response == null) {
		report("cannot get paging mode, continuing blind\n", report_warning);
	} else if (req_page_mode.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
		report("5 level paging mode is not supported\n", report_critical);
		panic(panic_code_unsupported_paging_mode);
		__builtin_unreachable();
	}

	if (req_page_hhdm.response == null) {
		report("cannot get offset for higher half direct map\n", report_critical);
		panic(panic_code_paging_initialization_failure);
		__builtin_unreachable();
	}
	base.hhdm = (void*)req_page_hhdm.response->offset;


	memnull(&page_heap, sizeof(page_heap_t));
	vvecs(&page_heap.segments, sizeof(page_heap_segment_t));

	//	allocate pages for page_heap
		//	calculate minimal page heap size
	page_heap.size = ((size_t)heap.end_physical - (size_t)heap.start);
	//print("page heap init size:\t"); printu(page_heap.init_size); endl();

	page_heap.size = (page_heap.size / PAGE_SIZE) + (page_heap.size % PAGE_SIZE != 0);
		//	get page count
	page_heap.size = (((page_heap.size / PAGE_COUNT) + 1) * PAGE_COUNT);
		//	round to 512

	aptrs(&page_heap.tmp.pdpt, 4096);

	//	pre-initialize page heap
	memmap_entry* page_heap_memmap_ent = page_heap_reserve_memory();
	if (page_heap_memmap_ent == null) {
		report("cannot allocate memory for kernel page heap\n", report_error);
		page_heap.size = 0;
	}
	page_heap.base.physical = (void*)page_heap_memmap_ent->base;
	page_table_t* pml4 = page_find();

	if (page_heap.size < PAGE_COUNT*PAGE_COUNT) {	//	fits into one pd table
		bool found = false;
		union virtual_union vbase;
		vbase.u64 = 0;
		page_table_t* pdpt;

		for (size_t i = PAGE_COUNT - 1; (i & (1 << 8)) != 0; i--) {
			//	find null entry in higher-half part of pml4
			if ((*pml4)[i].address == 0) {
				vbase.virtual_address.pml4 = i;
				found = true;
				break;
			}
		}
		if (!found) {
			report("cannot find place to allocate page heap in higher-half memory\n", report_warning);
			hang();
		}
		vbase.virtual_address.sign = 0xFFFF;
			//	mark that the pointer is in higher-half of virtual memory

		//	allocate and prepare pdpt
		aptr_alloc(&page_heap.tmp.pdpt, PAGE_TABLE_SIZE);
		memnull(page_heap.tmp.pdpt.ptr, PAGE_TABLE_SIZE);
		(*pml4)[vbase.virtual_address.pml4].address = (size_t)page_heap.tmp.pdpt.ptr >> PAGE_SHIFT;
		(*pml4)[vbase.virtual_address.pml4].present = true;
		(*pml4)[vbase.virtual_address.pml4].execute_disable = true;
		(*pml4)[vbase.virtual_address.pml4].write = true;
		pdpt = page_heap.tmp.pdpt.ptr;

		//	map pdpt page to itself
		(*pdpt)[0].address = (size_t)pdpt >> PAGE_SHIFT;
		(*pdpt)[0].present = true;
		(*pdpt)[0].execute_disable = true;
		(*pdpt)[0].write = true;

		//	since pdpt is mapped to itself it now counts as pd
		for (size_t i = 0; i < (page_heap.size / PAGE_COUNT); i++) {
			(*pdpt)[i+1].page_size = true;
			(*pdpt)[i+1].address = ((size_t)page_heap.base.physical + ((2*MB) * i)) >> PAGE_SHIFT;
			(*pdpt)[i+1].present = true;
			(*pdpt)[i+1].execute_disable = true;
			(*pdpt)[i+1].write = true;
		}
		vbase.virtual_address.pd = 1;

		page_heap.base.virtual_ = (void*)vbase.u64;

		//	initialize page heap
		page_heap_init();
	}*//*


	if (vocality >= vocality_report_everything) {
		report("memory protection initialized successfully\n", report_note);
	}
}*/


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
	page_table_t* pages = page_find();
	page_entry ent = (*pages)[address.pml4];
	if ((!ent.present) || (ent.address == 0)) {
		#ifndef KERNEL_DEBUG
		printl("\t\tphysical():\tpage not present: 3");
		#endif
		return null;
	}
	pages = (page_table_t*)(page_address(ent));
	ent = (*pages)[address.pdpt];
	if ((!ent.present) || (ent.address == 0)) {
		printl("\t\tphysical():\tpage not present: 2");
		return null;
	}
	pages = (page_table_t*)((size_t)ent.address << VA_SHIFT);
	ent = (*pages)[address.pd];
	if ((!ent.present) || (ent.address == 0)) {
		printl("\t\tphysical():\tpage not present: 1");
		return null;
	}
	pages = (page_table_t*)((size_t)ent.address << VA_SHIFT);
	ent = (*pages)[address.pt];
	if ((!ent.present) || (ent.address == 0)) {
		printl("\t\tphysical:\tpage not present: 0");
		return null;
	}
	return (void*)((size_t)ent.address << VA_SHIFT);
}

