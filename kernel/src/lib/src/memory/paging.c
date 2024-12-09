//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

#include "../../k_management.h"

/*void page_init() {

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

}*/

void page_init() {

	u64* tmp = null;

	//	check initial paging setup
	if (req_page_mode.response == null) {
		report("cannot retrieve paging mode\n", report_critical);
		panic(panic_code_paging_initialization_failure);
		__builtin_unreachable();
	}
	if (req_page_mode.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
		report("unsupported paging mode (", report_critical);
		if (req_page_mode.response->mode == LIMINE_PAGING_MODE_X86_64_5LVL) {
			printl("5 level)");
		} else {
			printl("unknown)");
		}
	}
	req_page_mode.response = null;




	//	read the pml4 physical address
	asm volatile("mov %0, cr3" : "=r"(pages.pml4));
	if (pages.pml4 == null) {
		report("cannot retrieve paging table address\n", report_critical);
		panic(panic_code_unable_to_allocate_paging_table);
		__builtin_unreachable();
	}




	//	detect HHDM
	if (req_page_hhdm.response == null) {
		report("cannot get higher half direct map offset\n", report_critical);
		panic(panic_code_paging_initialization_failure);
		__builtin_unreachable();
	}
	if (req_page_hhdm.response->revision != 0) {
		report("unsupported revision for higher half direct map => it can cause unexpected behaviour\n", report_warning);
	}
	pages.hhdm = req_page_hhdm.response->offset;
	req_page_hhdm.response = null;
	tmp = (u64*)&pages.pml4;
	*tmp += pages.hhdm;



	//	retrieve kernel address
	if (req_k_address.response == null) {
		report("could not find kernel address\n", report_warning);
	} else {
		pages.kernel.virtual = (void*)req_k_address.response->virtual_base;
		pages.kernel.physical = (void*)req_k_address.response->physical_base;
		req_k_address.response = null;
	}





	//	system pages
	union virtual_union virt = {.u64 = 0};
	virt.virtual_address.sign = 0xffff;
	{
		tmp = (u64*)&pages.system.pdpt.page;
		u64* tmp2 = (u64*)&pages.system.random.page;
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			tmp[i] = (tmp2[i] = 0b11 | ((u64)1 << 63));
				//	present, write, exec disable
		}
	}

	for (size_t i = 511; i > 255; i--) {
		//	find unused pml4 entry
		if ((*pages.pml4)[i].address == 0) {
			virt.virtual_address.pml4 = i;
			break;
		}
	}
	if (virt.virtual_address.pml4 == 0) {
		report("could not find any free virtual memory in higher half\n", report_critical);
		panic(panic_code_paging_initialization_failure);
		__builtin_unreachable();
	}

		//	system pages addresses
	pages.system.pdpt.physical = (size_t)&pages.system.pdpt.page - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;
	pages.system.pdpt.virtual = virt.voidptr;

	pages.system.random.physical = (size_t)&pages.system.random.page - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;
	virt.virtual_address.pdpt = 511;
	pages.system.random.virtual = virt.voidptr;

	{	//	connect pdpt to pml4
		page_entry* ent = &((*pages.pml4)[virt.virtual_address.pml4]);
		ent->address = pages.system.pdpt.physical >> PAGE_SHIFT;
		ent->present = true;
		ent->write = true;
		ent->exec_disable = true;
	}

	{	//	connect random to pdpt
		page_entry* ent = &pages.system.pdpt.page[511];
		ent->address = pages.system.random.physical >> PAGE_SHIFT;
		ent->present = true;
		ent->write = true;
		ent->exec_disable = true;
	}

		//	map random to itself
	pages.system.random.page[0].address = pages.system.random.physical >> PAGE_SHIFT;
	virt.virtual_address.pt = 1;
	pages.system.random.virtual = virt.voidptr;

	pages.system.random.page[1].address = pages.system.random.physical >> PAGE_SHIFT;

	//	find place for kernel heaps
	if ((!page_heap_reserve_memory()) || (!heap_reserve_memory(false))) {
		report("failed to find space for kernel heap", report_critical);
		panic(panic_code_cannot_allocate_memory_for_kernel_heap);
		__builtin_unreachable();
	}

}


void va_info(void* a) {
	union virtual_union address = {.voidptr = a};
	if (address.virtual_address.sign > 0) {
		print("higherhalf address ");
	} else {
		print("lowerhalf address ");
	}
	printp(a); printl(":");
	print("\tpml4:\t"); printu(address.virtual_address.pml4); endl();
	print("\tpdpt:\t"); printu(address.virtual_address.pdpt); endl();
	print("\tpd:\t\t"); printu(address.virtual_address.pd); endl();
	print("\tpt:\t\t"); printu(address.virtual_address.pt); endl();
	print("\toffset:\t"); printu(address.virtual_address.offset); endl();
}

void page_flush() {
	//	reload cr3
	asm volatile("mov cr3, %0" :: "a"((size_t)pages.pml4 - pages.hhdm));
}


void* physical(void* a) {
	union virtual_union address = {.voidptr = a};

	//print("physical:\t"); printp(((union virtual_union)address).voidptr); endl();
	page_entry ent = (*pages.pml4)[address.virtual_address.pml4];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpml4");
		return null;
	} else if (ent.page_size) {
		//printl("\tpage size:\tpml4");
		return (void*)((size_t)ent.address << PAGE_SHIFT);
	}

	ent = (*((page_table_t*)((size_t)ent.address << PAGE_SHIFT)))[address.virtual_address.pdpt];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpdpt");
		return null;
	} else if (ent.page_size) {
		//printl("\tpage size:\tpdpt");
		return (void*)((size_t)ent.address << PAGE_SHIFT);
	}

	ent = (*((page_table_t*)((size_t)ent.address << PAGE_SHIFT)))[address.virtual_address.pd];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpd");
		return null;
	} else if (ent.page_size) {
		//printl("\tpage size:\tpd");
		return (void*)((size_t)ent.address << PAGE_SHIFT);
	}

	ent = (*((page_table_t*)((size_t)ent.address << PAGE_SHIFT)))[address.virtual_address.pt];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpt");
		return null;
	}
	//printl("\tvalid");
	return (void*)(((size_t)ent.address << PAGE_SHIFT) | address.virtual_address.offset);
}


void* page_quick_map(void* physical, page_entry** ent) {
	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.random.page[i].address == 0) {
			pages.system.random.page[i].address = (size_t)physical >> PAGE_SHIFT;
			*ent = &pages.system.random.page[i];

			union virtual_union ret = {.voidptr = pages.system.random.virtual};
			ret.virtual_address.pt = i;
			return ret.voidptr;
		}
	}
	return null;
}

page_entry* page_find_empty_pdpt() {
	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.pdpt.page[i].address == 0) {
			return &pages.system.pdpt.page[i];
		}
	}
	return null;
}

