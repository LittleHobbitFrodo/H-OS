//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

#include "../../k_management.h"

void _paging_init_critical_error(const char* msg, size_t line, enum panic_codes code) {
	if (vocality >= vocality_report_everything) {
		report_status("CRITICAL FAILURE", line, col.critical);
	}
	report(msg, report_critical);
	if (code != ok) {
		panic(code);
	}
}

void paging_init() {

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("working on memory protection\n", report_note);
	}

	u64* tmp = null;

	//	check initial paging setup
	if (req_page_mode.response == null) {
		_paging_init_critical_error("cannot retrieve paging mode\n", line, panic_code_paging_initialization_failure);
		__builtin_unreachable();
	}
	if (req_page_mode.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
		_paging_init_critical_error("unsupported paging mode (", line, ok);
		if (req_page_mode.response->mode == LIMINE_PAGING_MODE_X86_64_5LVL) {
			printl("5 level)");
		} else {
			printl("unknown)");
		}
		panic(panic_code_unsupported_paging_mode);
	}
	req_page_mode.response = null;






	//	detect HHDM
	if (req_page_hhdm.response == null) {
		_paging_init_critical_error("cannot get higher half direct map offset\n", line, panic_code_paging_initialization_failure);
		__builtin_unreachable();
	}
	if (req_page_hhdm.response->revision != 0) {
		report("detected unsupported revision for HHDM, this may cause some troubles\n", report_warning);
	}
	pages.hhdm = req_page_hhdm.response->offset;
	req_page_hhdm.response = null;


	//	read the pml4 physical address
	asm volatile("mov %0, cr3" : "=r"(pages.pml4));
	if (pages.pml4 == null) {
		_paging_init_critical_error("cannot retrieve paging table address\n", line, panic_code_unable_to_allocate_paging_table);
		__builtin_unreachable();
	}
	tmp = (u64*)&pages.pml4;
	*tmp += pages.hhdm;


	//	retrieve kernel address
	if (req_k_address.response == null) {
		_paging_init_critical_error("kernel addresses cannot be found\n", line, panic_code_base_addresses_not_available);
		__builtin_unreachable();
	}
	pages.kernel.virtual = (void*)req_k_address.response->virtual_base;
	pages.kernel.physical = (void*)req_k_address.response->physical_base;
	req_k_address.response = null;



	union virtual_address virt = {.u64 = 0};
	virt.virtual_address.sign = 0xffff;

	//	system pages
	{
		tmp = (u64*)&pages.system.pdpt.table;

		//	system pdpt (global?)
		union any_page_entry ape = {.unsized = {.present = true, .write = true, .exec_disable = true}};
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			tmp[i] = ape.u64;
		}

		//	pages for quick mapping
		tmp = (u64*)&pages.system.quick.table;
		ape.u64 = 0;	//	clear all flags
		ape.sized = (sized_page_entry){.present = true, .write = true, .exec_disable = true, .page_size = true};
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			tmp[i] = ape.u64;
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
		_paging_init_critical_error("could not find any free virtual memory in higher half memory\n", line, panic_code_paging_initialization_failure);
		__builtin_unreachable();
	}

		//	system pages addresses
	pages.system.pdpt.physical = (size_t)&pages.system.pdpt.table - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;
	pages.system.pdpt.virtual = virt.voidptr;

		//	prepare quick
	pages.system.quick.physical = (size_t)&pages.system.quick.table - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;
	virt.virtual_address.pdpt = virt.virtual_address.pml4;
	pages.system.quick.virtual = virt.voidptr;

	{	//	connect pdpt to pml4
		unsized_page_entry* ent = &((*pages.pml4)[virt.virtual_address.pml4]);
		unsized_page_set_address(*ent, pages.system.pdpt.physical);
		ent->present = true;
		ent->write = true;
		ent->exec_disable = true;
	}

	{	//	connect quick to pdpt
		//	NOTE:	quick is sized (and deprecated)
		unsized_page_entry* ent = &pages.system.pdpt.table[511];
		unsized_page_set_address(*ent, pages.system.quick.physical);
		ent->present = true;
		ent->write = true;
		ent->exec_disable = true;
	}

	sized_page_entry ent = {.present = true, .write = true, .exec_disable = true, .page_size = true};

	{	//	heap pages (2mb) -> make them null
		sized_page_table* table = (sized_page_table*)&pages.system.heap.table;
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			(*table)[i] = ent;
		}
		pages.system.heap.physical = (size_t)&pages.system.heap.table - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;

		for (ssize_t i = PAGE_COUNT-1; i >= 0; i--) {
			//	find unused pdpt entry connect it to heap table and set heap virtual address
			if (pages.system.pdpt.table[i].address == 0) {
				unsized_page_set_address(pages.system.pdpt.table[i], pages.system.heap.physical);

				union virtual_address address = {.voidptr = pages.system.pdpt.virtual};
				address.virtual_address.pdpt = i;
				pages.system.heap.virtual = address.voidptr;
				break;
			}
		}

		if (pages.system.pdpt.virtual == null) {
			_paging_init_critical_error("failed to create system heap virtual address space\n", line, panic_code_paging_initialization_failure);
			__builtin_unreachable();
		}

	}

	{	//	table heap (2mb)
		sized_page_table* table = &pages.system.table_heap.table;
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			(*table)[i] = ent;
		}
		pages.system.table_heap.physical = (size_t)&pages.system.table_heap.table - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;

		for (ssize_t i = PAGE_COUNT-1; i >= 0; i--) {
			//	find unused pdpt entry and connect it
			if (pages.system.pdpt.table[i].address == 0) {
				unsized_page_set_address(pages.system.pdpt.table[i], pages.system.table_heap.physical);

				union virtual_address address = {.voidptr = pages.system.pdpt.virtual};
				address.virtual_address.pdpt = i;
				pages.system.table_heap.virtual = address.voidptr;
				break;
			}
		}

		if (pages.system.table_heap.virtual == null) {
			_paging_init_critical_error("failed to create system table heap virtual address space\n", line, panic_code_paging_initialization_failure);
			__builtin_unreachable();
		}

	}

	{	//	pd
		unsized_page_table* table = &pages.system.pd.table;
		const unsized_page_entry e = {.present = true, .write = true, .exec_disable = true};
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			(*table)[i] = e;
		}

		pages.system.pd.physical = (size_t)&pages.system.pd.table - (size_t)pages.kernel.virtual + (size_t)pages.kernel.physical;

		for (size_t i = 0; i < PAGE_COUNT; i++) {
			if (pages.system.pdpt.table[i].address == 0) {
				unsized_page_set_address(pages.system.pdpt.table[i], pages.system.pd.physical);

				union virtual_address address = {.voidptr = pages.system.pdpt.virtual};
				address.virtual_address.pdpt = i;
				pages.system.pd.virtual = address.voidptr;
				break;
			}
		}

		if (pages.system.pd.virtual == null) {
			_paging_init_critical_error("failed to connect pd layer to pdpt\n", line, panic_code_paging_initialization_failure);
			__builtin_unreachable();
		}

	}

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}

}


void va_info(void* a) {
	union virtual_address address = {.voidptr = a};
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


void* physical([[maybe_unused]] void* a) {
	return null;
	/*union virtual_address_t address = {.voidptr = a};

	//print("physical:\t"); printp(((union virtual_address_t)address).voidptr); endl();
	unsized_page_entry ent = (*pages.pml4)[address.virtual_address_t.pml4];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpml4");
		return null;
	} else if (ent.page_size) {
		//printl("\tpage size:\tpml4");
		return (void*)((size_t)ent.address << PAGE_SHIFT);
	}

	ent = (*((page_table_t*)((size_t)ent.address << PAGE_SHIFT)))[address.virtual_address_t.pdpt];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpdpt");
		return null;
	} else if (ent.page_size) {
		//printl("\tpage size:\tpdpt");
		return (void*)((size_t)ent.address << PAGE_SHIFT);
	}

	ent = (*((page_table_t*)((size_t)ent.address << PAGE_SHIFT)))[address.virtual_address_t.table];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpd");
		return null;
	} else if (ent.page_size) {
		//printl("\tpage size:\tpd");
		return (void*)((size_t)ent.address << PAGE_SHIFT);
	}

	ent = (*((page_table_t*)((size_t)ent.address << PAGE_SHIFT)))[address.virtual_address_t.pt];
	if ((ent.present == false) || (ent.address == 0)) {
		//printl("\t!present || address:\tpt");
		return null;
	}
	//printl("\tvalid");
	return (void*)(((size_t)ent.address << PAGE_SHIFT) | address.virtual_address_t.offset);*/
}


void* page_quick_map(size_t physical, sized_page_entry** ent) {
	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.quick.table[i].address == 0) {
			print("quick map: found empty page at "); printu(i); endl();
			sized_page_set_address(pages.system.quick.table[i], physical);
			*ent = &pages.system.quick.table[i];

			union virtual_address address = {.voidptr = pages.system.quick.virtual};
			address.virtual_address.pd = i;
			print("address:\t"); printp(address.voidptr); endl();
			return address.voidptr;
		}
	}
	return null;
}

unsized_page_entry* page_find_empty_pdpt() {
	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.pdpt.table[i].address == 0) {
			return &pages.system.pdpt.table[i];
		}
	}
	return null;
}

page_table* random_table_alloc(table_allocator_t* alloc, void** virtual, size_t physical) {

	//	allocates one pt and connects it to pd

	ssize_t pd = -1;
	*virtual = null;

	{	//	find pd entry
		for (size_t i = 0; i < PAGE_COUNT; i++) {
			if (pages.system.pd.table[i].address == 0) {
				pd = (ssize_t)i;
				union virtual_address address = {.voidptr = pages.system.pd.virtual};
				address.virtual_address.pd = i;
				*virtual = address.voidptr;
				break;
			}
		}
		if (pd < 0) {
			return null;
		}
	}


	page_table* table = (page_table*)table_alloc(alloc, 1);
	if (table == null) {
		return null;
	}

	//	initialize table
	for (size_t i = 0; i < PAGE_COUNT; i++) {
		(*table)[i] = (page_table_entry){.present = true, .write = true, .exec_disable = true};
	}

	unsized_page_set_address(pages.system.pd.table[pd], table_physical(alloc, (void*)table));

	unsized_page_set_address((*table)[0], physical);

	return table;
}


void* kmem_map_sized(size_t physical, size_t size, sized_page_table** table) {

	union virtual_address address = {.voidptr = pages.system.pdpt.virtual};
	sized_page_table* tab;

	for (size_t i = 0; i < PAGE_COUNT; i++) {
		if (pages.system.pdpt.table[i].address == 0) {
			address.virtual_address.pdpt = i;
			goto pdpt_found;
		}
	}

	if (table != null) {
		*table = null;
	}
	return null;

	pdpt_found:

	tab = (sized_page_table*)table_alloc(&pages.heap.global, 1);
	if (tab == null) {
		if (table != null) {
			*table = null;
		}
		return null;
	}

	physical = align(physical, 2*MB);

	for (size_t i = 0; i < PAGE_COUNT; i++) {
		(*tab)[i] = (sized_page_entry){.present = true, .write = true, .exec_disable = true, .page_size = true};
	}
	unsized_page_set_address(pages.system.pdpt.table[address.virtual_address.pdpt], table_physical(&pages.heap.global, (void*)tab));
	for (size_t i = 0; (i < PAGE_COUNT) && ((i*(2*MB)) < size); i++) {
		sized_page_set_address((*tab)[i], physical + (i*(2*MB)));
	}

	if (table != null) {
		*table = tab;
	}

	return address.voidptr;
}