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
	//base.hhdm = (void*)req_page_hhdm.response->offset;

	//	allocates pages for heap
	vecs(&pages.tmp.page_heap, sizeof(aligned_ptr));



	/*print("virtual base:\t"); printp(base.virtual); endl();
	page_table_t* pages = page_find();
	page_entry ent = (*pages)[va_index(base.virtual, 3)];
	if ((!ent.present) || (ent.address == 0)) {
		report("page not present: 3\n", report_error);
		return;
	}
	pages = (page_table_t*)(page_address(ent));
	ent = (*pages)[va_index(base.virtual, 2)];
	if ((!ent.present) || (ent.address == 0)) {
		report("page not present: 2\n", report_error);
		return;
	}
	pages = (page_table_t*)((size_t)ent.address << VA_SHIFT);
	ent = (*pages)[va_index(base.virtual, 1)];
	if ((!ent.present) || (ent.address == 0)) {
		report("page not present: 1\n", report_error);
		return;
	}
	pages = (page_table_t*)((size_t)ent.address << VA_SHIFT);


	const size_t heap_size = (size_t)heap.end_physical - (size_t)heap.start;
	size_t pages_needed = (heap_size / PAGE_SIZE) + (heap_size % PAGE_SIZE != 0);

	u8 ad;
	for (size_t i = 0; i < pages_needed; i++) {
		ad = ((*pages)[i].accessed) | ((*pages)[i].dirty << 1);
		if (ad != 0) {
			print("page["); printu(i); print("]:\t");
			if (ad & 1) {
				print("accessed ");
			}
			if ((ad & 0b10) != 0) {
				print("dirty ");
			}
			endl();
		}
	}*/

	/*for (size_t i = 0; i < pages_needed; i++) {
		if ((*pages)[i].address != 0) {
			print("page "); printu(i+1); print("is not null");
			if (!(*pages)[i].present) {
				printl(" and also no present");
			}
			endl();
		} else if (!(*pages)[i].present) {
			print("page "); printu(i+1); printl("is not present");
		}

	}*/

	//ent = (*pages)[va_index(base.virtual, 0)];
	/*if ((!ent.present) || (ent.address == 0)) {
		report("page not present: 0\n", report_error);
		union page_void pv;
		pv.page_entry = ent;
		print("page:\t"); printp(pv.voidptr); endl();
		return;
	}*/
	/*union virtual_void vv;
	vv.ptr = base.virtual;*/

	if (vocality >= vocality_report_everything) {
		report("memory protection initialized successfully\n", report_note);
	}
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