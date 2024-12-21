//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

#include "../../k_management.h"

void page_init() {

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("proceeding with memory protection initialization\n", report_note);
	}

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
		tmp = (u64*)&pages.system.pdpt.table;
		u64* tmp2 = (u64*)&pages.system.quick.table;
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			//tmp[i] = (tmp2[i] = 0b11 | ((u64)1 << 63));
				//	present, write, exec disable
			tmp[i] = (tmp2[i] = page_bit_present | page_bit_write | page_bit_exec_disable);
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
	pages.system.pdpt.physical = (size_t)&pages.system.pdpt.table - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;
	pages.system.pdpt.virtual = virt.voidptr;

	ssize_t pml4 = -1;
	for (ssize_t i = 511; i >= 0; i--) {
		//	find empty pml4 entry
		if ((*pages.pml4)[i].address == 0) {
			pml4 = i;
			break;
		}
	}
	if (pml4 == -1) {
		report("could not find empty pml4 entry\n", report_critical);
		panic(panic_code_paging_initialization_failure);
	}

	pages.system.quick.physical = (size_t)&pages.system.quick.table - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;
	virt.virtual_address.pdpt = pml4;
	pages.system.quick.virtual = virt.voidptr;

	{	//	connect pdpt to pml4
		page_entry* ent = &((*pages.pml4)[virt.virtual_address.pml4]);
		ent->address = pages.system.pdpt.physical >> PAGE_SHIFT;
		ent->present = true;
		ent->write = true;
		ent->exec_disable = true;
	}

	{	//	connect quick to pdpt
		page_entry* ent = &pages.system.pdpt.table[511];
		ent->address = pages.system.quick.physical >> PAGE_SHIFT;
		ent->present = true;
		ent->write = true;
		ent->exec_disable = true;
	}

		//	map quick to itself
	pages.system.quick.table[0].address = pages.system.quick.physical >> PAGE_SHIFT;
	virt.virtual_address.pt = 1;
	pages.system.quick.virtual = virt.voidptr;

	pages.system.quick.table[1].address = pages.system.quick.physical >> PAGE_SHIFT;

	{	//	heap pages -> make them null
			//	recursive
		tmp = (u64*)&pages.system.heap.table;
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			tmp[i] = page_bit_present | page_bit_write | page_bit_exec_disable;
		}
		pages.system.heap.table[0].address = (size_t)((size_t)&pages.system.heap.table - (size_t)pages.kernel.virtual + pages.kernel.physical) >> PAGE_SHIFT;

		for (ssize_t i = 511; i >= 0; i--) {
			//	find pdpt entry
			if (pages.system.pdpt.table[i].address == 0) {
				pages.system.pdpt.table[i].address = pages.system.heap.table[0].address;
				union virtual_union vu = {.voidptr = pages.system.pdpt.virtual};
				vu.virtual_address.pdpt = i;
				vu.virtual_address.pt = 1;
				pages.system.heap.virtual = vu.voidptr;
				break;
			}
		}
	}

	{	//	page heap pages
		tmp = (u64*)&pages.system.page_heap.pd;
		u64* tmp2 = (u64*)&pages.system.page_heap.pt;
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			tmp[i] = (tmp2[i] = page_bit_present | page_bit_write | page_bit_exec_disable);
		}

		pages.system.page_heap.physical_pd = (size_t)((size_t)&pages.system.page_heap.pd - (size_t)pages.kernel.virtual + pages.kernel.physical);
		pages.system.page_heap.physical_pt = (size_t)((size_t)&pages.system.page_heap.pt - (size_t)pages.kernel.virtual + pages.kernel.physical);
		pages.system.page_heap.pd[0].address = pages.system.page_heap.physical_pt >> PAGE_SHIFT;

		for (ssize_t i = 511; i >= 0; i--) {
			//	find empty pdpt entry
			if (pages.system.pdpt.table[i].address == 0) {
				pages.system.pdpt.table[i].address = pages.system.page_heap.physical_pd >> PAGE_SHIFT;
				union virtual_union vu = {.voidptr = pages.system.pdpt.virtual};
				vu.virtual_address.pdpt = i;
				pages.system.page_heap.virtual = vu.voidptr;
				break;
			}
		}
	}

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
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
		if (pages.system.quick.table[i].address == 0) {
			pages.system.quick.table[i].address = (size_t)physical >> PAGE_SHIFT;
			*ent = &pages.system.quick.table[i];

			union virtual_union ret = {.voidptr = pages.system.quick.virtual};
			ret.virtual_address.pt = i;
			return ret.voidptr;
		}
	}
	return null;
}

page_entry* page_find_empty_pdpt() {
	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.pdpt.table[i].address == 0) {
			return &pages.system.pdpt.table[i];
		}
	}
	return null;
}

