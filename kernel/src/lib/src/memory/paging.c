//
//	memory/paging.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/paging.h"

page_entry *page_find() {
	page_entry *ret;
	asm volatile("mov %0, cr3" : "=r"(ret));
	return ret;
}

void page_init() {
	virtual_base = (void *) req_k_address.response->virtual_base;
	physical_base = (void *) req_k_address.response->physical_base;

	//	default paging layout
	//	pml4[(highest used)]
	//	pdpt[(highest unused)]
	//	pd[1] ->	paging
	//	pd[1 .. x]	heap

	enum page_flags nullf = present | no_exec;

	pml4 = page_find();
	pd.ptr = (aligned_ptr){.ptr = null, .align = 4096, .offset = 0};
	pd.count = PAGE_COUNT;
	pt.ptr = (aligned_ptr){.ptr = null, .align = 4096, .offset = 0};
	pt.count = 0;

	memmap_entry *hent = null, *pent = null; {
		//	page count
		memmap_entry *mem;
		for (size_t i = 0; i < memmap.len; i++) {
			switch ((mem = ((memmap_entry *) vec_at(&memmap, i)))->type) {
				case memmap_heap: {
					hent = mem;
					break;
				}
				case memmap_paging: {
					pent = mem;
					break;
				}
				default: break;
			}
		}
		u8 check = (hent == null) | ((pent == null) << 1);
		if (check != 0) {
			if (check & 0b1) {
				report("could not find heap memory map entry\n", report_critical);
			}
			if ((check & 0b10) != 0) {
				report("could not find page table memory map entry\n", report_critical);
			}
			panic(panic_code_paging_initialization_failure);
		}
	}
	//	hent, sent, pent != null

	//	find highest used pml4 entry
	page_entry *connect = null; {
		page_entry *pdpt = null;
		for (ssize_t i = PAGE_COUNT - 1; (i >= 0) && (connect == null); i--) {
			if ((pdpt = page_address(pml4[i])) != null) {
				for (ssize_t ii = PAGE_COUNT - 1; ii >= 0; ii--) {
					if (page_address(pdpt[ii]) == null) {
						connect = &pdpt[ii];
						va_set_index(&kernel_stack_address, i, 3);
						va_set_index(&kernel_stack_address, ii, 2);
						break;
					}
				}
			}
		}
		if (connect == null) {
			report("could not find valid pdpt entry\n", report_critical);
			panic(panic_code_paging_initialization_failure);
		}
	}

	//	allocate pd and pt
	aptr_alloc(&pd.ptr, pd.count * sizeof(page_entry));
	for (size_t i = 0; i < pd.count; i++) {
		((page_entry *) pd.ptr.ptr)[i] = (page_entry) nullf;
	}

	//	calculate page entries for ring0 data
	pt.count = (((hent->len / PAGE_SIZE) / PAGE_COUNT * PAGE_COUNT)) + 1;
	pt.count += (((pent->len / PAGE_SIZE) / PAGE_COUNT) * PAGE_COUNT) + 1;
	pt.count *= PAGE_COUNT;

	aptr_alloc(&pt.ptr, pt.count * sizeof(page_entry));
	output.color = col.red;
	va_set_index(&kernel_stack_address, 0, 1);
	for (size_t i = 0; i < pt.count; i++) {
		((page_entry *) pt.ptr.ptr)[i] = (page_entry) nullf;
	}

	//	connect pdpt to pd, pd to pts
	page_set_address(connect /*pdpt*/, pd.ptr.ptr); {
		size_t len = pt.count / PAGE_COUNT;
		page_entry *pd_ = (page_entry *) pd.ptr.ptr, *pt_ = (page_entry *) pt.ptr.ptr;
		for (size_t i = 0; i < len; i++) {
			page_set_address(&pd_[i], &pt_[i * PAGE_COUNT]);
		}
	}

	//	point pt addresses to actual memory locations
	page_entry *pt_ = (page_entry *) pt.ptr.ptr;
	size_t limit;
	//	paging (index 0)
	{
		limit = (pent->len / PAGE_SIZE) + (pent->len % PAGE_SIZE != 0);
		for (size_t i = 0; i < limit; i++) {
			page_set_address(&pt_[i], (void *) (pent->base + (i * PAGE_SIZE)));
		}
	}
	//	heap (index 1)
	{
		heap_virtual_base = kernel_stack_address;
		pt_ += PAGE_COUNT;
		limit = (hent->len / PAGE_SIZE) + (pent->len % PAGE_SIZE != 0);
		for (size_t i = 0; i < limit; i++) {
			page_set_address(&pt_[i], (void *) (hent->base + (i * PAGE_SIZE)));
		}
		va_set_index(&heap_virtual_base, 1, 1);
		va_set_index(&heap_virtual_base, 0, 0);
		va_set_offset(&heap_virtual_base, 0);
		output.color = col.yellow;
		output.color = col.white;
	}

	if (vocality >= vocality_vocal) {
		report("paging initialization completed\n", report_note);
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

