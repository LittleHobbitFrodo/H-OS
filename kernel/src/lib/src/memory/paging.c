//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

page_table_t* page_find() {
	page_table_t* ret;
	asm volatile("mov %0, cr3" : "=r"(ret));
	return ret;
}

void page_init() {
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

	//	allocate pages for page_heap
		//	calculate minimal page heap size
	page_heap.size = ((size_t)heap.end_physical - (size_t)heap.start);
	//print("page heap init size:\t"); printu(page_heap.init_size); endl();

	page_heap.size = (page_heap.size / PAGE_SIZE) + (page_heap.size % PAGE_SIZE != 0);
		//	get page count
	page_heap.size = (((page_heap.size / PAGE_COUNT) + 1) * PAGE_COUNT);
		//	round to 512

	aptrs(&page_heap.tmp.pdpt, 4096);
	aptrs(&page_heap.tmp.pd, 4096);
	aptrs(&page_heap.tmp.pt, 4096);

	memmap_entry* phme = page_heap_reserve_memory();
	if (phme == null) {
		report("cannot allocate memory for kernel page heap\n", report_error);
		page_heap.size = 0;
	}
	page_heap.base.physical = (void*)phme->base;
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
				print("page heap virtual base (pml4):\t"); printu(vbase.virtual_address.pml4); endl();
				found = true;
				break;
			}
		}
		if (!found) {
			report("cannot find place to allocate page heap in higher-half memory\n", report_warning);
			hang();
		}
		vbase.virtual_address.sign = 0xFFFF;

		//	allocate and prepare pdpt
		aptr_alloc(&page_heap.tmp.pdpt, sizeof(page_entry) * PAGE_COUNT);
		memnull(page_heap.tmp.pdpt.ptr, sizeof(page_entry) * PAGE_COUNT);
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

		page_heap.base.virtual_ = vbase.voidptr;
	}


	if (vocality >= vocality_report_everything) {
		report("memory protection initialized successfully\n", report_note);
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


/*void *physical(void *virt) {
	page_entry ent = pml4[va_index(virt, 3)];
	if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
		print("\t\tlayer:\t4:\t");
		printp((void *) pml4[va_index(virt, 3)]);
		endl();
		return null;
	}
	for (i16 i = 2; i >= 0; --i) {
		ent = PAGE_ALIGN_DOWN(((page_entry*)page_address(ent))[va_index(virt, i)]);
		if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
			print("\t\tlayer:\t");
			printu(i);
			print(":\t");
			printp((void *) ent);
			endl();
			return null;
		}
	}
	return (void *) ((size_t) page_address(ent) + va_offset(virt));
}*/

/*extern void* _start;

void page_init() {

	printl("pages:");
	page_entry* pages = page_find();
	print("_start:\t"); printp(_start); endl();
	size_t hhdm = (size_t)_start;//req_k_address.response->virtual_base;
	if (page_address(pages[va_index((void*)hhdm, 3)]) != null) {
		print("page["); printu(va_index((void*)hhdm, 3)); printl("] is valid");
		pages = (page_entry*)page_address(pages[va_index((void*)hhdm, 3)]);
		if (page_address(pages[va_index((void*)hhdm, 2) + 1]) != null) {
			print("page[x]["); printu(va_index((void*)hhdm, 2) + 1); printl("] is valid");
			pages = (page_entry*)page_address(pages[va_index((void*)hhdm, 2) + 1]);
			if (page_address(pages[va_index((void*)hhdm, 1)]) != null) {
				print("page[x][y]["); printu(va_index((void*)hhdm, 1)); printl("] is valid");
				pages = (page_entry*)page_address(pages[va_index((void*)hhdm, 1)]);
				if (page_address(pages[va_index((void*)hhdm, 0)]) != null) {
					print("page[x][y][z]["); printu(va_index((void*)hhdm, 0)); printl("] is valid");
					pages = (page_entry*)page_address(pages[va_index((void*)hhdm, 0)]);
					print("address:\t"); printp(pages); endl();
					print("base physical:\t"); printp(physical_base); endl();
				} else {
					print("page[x][y][z]["); printu(va_index((void*)hhdm, 0)); printl("] is invalid");
				}
			} else {
				print("page[x][y]["); printu(va_index((void*)hhdm, 1)); printl("] is invalid");
				print("\n\n\nscan:\t"); printu(va_index((void*)hhdm, 2)); endl();
				for (size_t i = 0; i < PAGE_COUNT; i++) {
					if (page_address(pages[i]) != null) {
						printu(i); print(" : "); printp((void*)pages[i]); endl();
					}
				}
			}
		} else {
			print("page[x]["); printu(va_index((void*)hhdm, 2)); printl("] is invalid");
		}
	} else {
		print("page["); printu(va_index((void*)hhdm, 3)); printl("] is invalid");
	}

}

void page_entry_info(page_entry ent) {
	enum page_flags pflags = present;
	u32 c = output.color;
	output.color = col.cyan;
	if (ent & pflags) {
		print("present ");
	}
	pflags = global;
	if (ent & pflags) {
		print("global ");
	} {
		void *addr = page_address(ent);
		print("page entry:\t");
		if (addr == null) {
			printp(addr);
			endl();
		} else {
			printl("NULL");
		}
	}
	pflags = write;
	print("\tperms:\t");
	print((ent & pflags) ? "rw" : "r");
	pflags = no_exec;
	printc('x' * (ent & pflags));
	endl();
	pflags = cache_disable;
	print("\tcaching:\t");
	if (ent & pflags) {
		printl("disabled");
	} else {
		pflags = write_through;
		printl((ent & pflags) ? "write-through" : "write-back");
	}
	pflags = accessed;
	print("\tstate:\t");
	if (ent & pflags) {
		print("accessed, ");
	}
	pflags = dirty;
	printl((ent & pflags) ? "dirty" : "clean");
	pflags = PS;
	print("\tsize:\t\t");
	printl((ent & pflags) ? "2MiB" : "4kb");
	output.color = c;
}

void *physical(void *virt) {
	page_entry ent = pml4[va_index(virt, 3)];
	if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
		print("\t\tlayer:\t4:\t");
		printp((void *) pml4[va_index(virt, 3)]);
		endl();
		return null;
	}
	for (i16 i = 2; i >= 0; --i) {
		ent = PAGE_ALIGN_DOWN(((page_entry*)page_address(ent))[va_index(virt, i)]);
		if (unlikely((!(ent & present)) || (page_address(ent) == null))) {
			print("\t\tlayer:\t");
			printu(i);
			print(":\t");
			printp((void *) ent);
			endl();
			return null;
		}
	}
	return (void *) ((size_t) page_address(ent) + va_offset(virt));
}

void page_map_all() {

}*/