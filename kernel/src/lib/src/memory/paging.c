//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

#define PKEY_MASK 0x7800000000000000
#define PKEY_SHIFT 59

page_entry *page_find() {
	page_entry *ret;
	asm volatile("mov %0, cr3" : "=r"(ret));
	return ret;
}

void page_init() {
	memnull(&pages, sizeof(pages_base_t));

	//	prepare structure
	pages.base.physical = (void*)req_k_address.response->physical_base;
	pages.base.virtual = (void*)req_k_address.response->virtual_base;
	req_k_address.response = null;

	aptrse(&pages.pml4, sizeof(page_entry) * PAGE_COUNT, 4096);
	aptrse(&pages.kernel.pdpt,  sizeof(page_entry) * PAGE_COUNT, 4096);
	memnull(pages.pml4.ptr, sizeof(page_entry) * PAGE_COUNT);
	memnull(pages.kernel.pdpt.ptr, sizeof(page_entry) * PAGE_COUNT);

	//	connect pdpt to pml4

	page_entry* page = &((page_entry*)pages.pml4.ptr)[va_index(pages.base.virtual, VA_INDEX_PML4)];

	page_set_address(page, pages.kernel.pdpt.ptr);
	*page |= pf_present | pf_write;

	//	allocate and fill kernel pd
	aptrs(&pages.kernel.pd.ptr, 4096);
	memmap_entry* ent = null;
	for (size_t i = 0; i < memmap.len; i++) {
		memmap_entry* e = (memmap_entry*)vec_at(&memmap, i);
		if (e->type == memmap_kernel) {
			ent = e;
			break;
		}
	}
	if (ent == null) {
		report("could not find kernel memory map entry\n", report_critical);
		panic(panic_code_cannot_locate_kernel_entry);
	}

	pages.kernel.pd.size = (ent->len / (2 * MB)) + (ent->len % (2 * MB) != 0);
	printu(pages.kernel.pd.size); endl();
	aptr_alloc(&pages.kernel.pd.ptr, PAGE_COUNT * sizeof(page_entry) * pages.kernel.pd.size);
	memnull(pages.kernel.pd.ptr.ptr, PAGE_COUNT * sizeof(page_entry) * pages.kernel.pd.size);


	//	connect pd to pdpt
	page = &((page_entry*)pages.kernel.pdpt.ptr)[va_index(pages.base.virtual, VA_INDEX_PDPT)];
	page_set_address(page, pages.kernel.pd.ptr.ptr);
	*page |= pf_present | pf_write;

	//	allocate and fill pts
	aptrs(&pages.kernel.pt.ptr, 4096);
	pages.kernel.pt.size = (ent->len / PAGE_SIZE) + (ent->len % PAGE_SIZE != 0);
	aptr_alloc(&pages.kernel.pt.ptr, PAGE_COUNT * sizeof(page_entry) * pages.kernel.pt.size);
	memnull(pages.kernel.pt.ptr.ptr, PAGE_COUNT * sizeof(page_entry) * pages.kernel.pt.size);

	page = &((page_entry*)pages.kernel.pd.ptr.ptr)[va_index(pages.base.virtual, VA_INDEX_PD)];
	for (size_t i = 0; i < pages.kernel.pt.size; i++) {
		page_set_address(&page[i], (void*)((size_t)pages.kernel.pt.ptr.ptr + (i * PAGE_SIZE)));
		page[i] |= pf_present | pf_write;
	}

	page = &((page_entry*)pages.kernel.pt.ptr.ptr)[va_index(pages.base.virtual, VA_INDEX_PT)];
	for (size_t i = 0; i * PAGE_SIZE < ent->len; i++) {
		page_set_address(page, (void*)(ent->base + (i * PAGE_SIZE)));
		page_set_flags(page, pf_present | pf_write);
	}

	//asm volatile("mov cr3, %0" :: "r"(pages.pml4.ptr));

	//printp((void*)((page_entry*)pages.pml4.ptr)[va_index(pages.base.virtual, VA_INDEX_PML4)]);

}

void page_entry_info(page_entry ent) {
	enum page_flags pflags = pf_present;
	u32 c = output.color;
	output.color = col.cyan;
	if (ent & pflags) {
		print("present ");
	}
	pflags = pf_global;
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
	pflags = pf_write;
	print("\tperms:\t");
	print((ent & pflags) ? "rw" : "r");
	pflags = pf_no_exec;
	printc('x' * (ent & pflags));
	endl();
	pflags = pf_cache_disable;
	print("\tcaching:\t");
	if (ent & pflags) {
		printl("disabled");
	} else {
		pflags = pf_write_through;
		printl((ent & pflags) ? "write-through" : "write-back");
	}
	pflags = pf_accessed;
	print("\tstate:\t");
	if (ent & pflags) {
		print("accessed, ");
	}
	pflags = pf_dirty;
	printl((ent & pflags) ? "dirty" : "clean");
	pflags = pf_PS;
	print("\tsize:\t\t");
	printl((ent & pflags) ? "2MiB" : "4kb");
	output.color = c;
}

void *physical(void *virt) {
	page_entry ent = ((page_entry*)pages.pml4.ptr)[va_index(virt, 3)];
	if (unlikely((!(ent & pf_present)) || (page_address(ent) == null))) {
		printl("layer 3");
		return null;
	}
	for (i16 i = 2; i >= 0; --i) {
		ent = PAGE_ALIGN_DOWN(((page_entry*)page_address(ent))[va_index(virt, i)]);
		if (unlikely((!(ent & pf_present)) || (page_address(ent) == null))) {
			print("layer "); printu(i); endl();
			return null;
		}
	}
	return (void *) ((size_t) page_address(ent) + va_offset(virt));
}

