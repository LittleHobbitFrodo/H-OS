//
//	memory/heap/table-heap.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../memory/heap/table-heap/table-heap.h"

void table_heap_reserve_memory() {

	//	finds place for global table allocator

	struct limine_memmap_entry* ent;
	size_t mlen = req_memmap.response->entry_count;


	//	try to find aligned place
	for (size_t i = 0; i < mlen; i++) {
		ent = req_memmap.response->entries[i];
		if (ent->type != LIMINE_MEMMAP_USABLE) {
			continue;
		}

		if ((ent->base == align(ent->base, 2*MB)) && (ent->length >= 4*MB)) {

			//	construct virtual address space
			sized_page_set_address(pages.system.table_heap.table[0], ent->base);

			pages.heap.global.data = pages.system.table_heap.virtual;
			table_heap_t* const h = pages.heap.global.data;

			memnull(h, sizeof(table_heap_t));

			h->meta.virtual.start = h;
			h->meta.virtual.end = (void*)((size_t)h + (2*MB));
			h->meta.physical.start = ent->base;
			h->meta.physical.end = ent->base + (2*MB);
			h->meta.table = &pages.system.table_heap.table;
			h->meta.allocator = &pages.heap.global;
			h->meta.size = 2*MB;
			h->meta.next = null;
			return;
		}
	}


	//	find valid place and align it to 2mb
	for (size_t i = 0; i < mlen; i++) {
		ent = req_memmap.response->entries[i];

		if ((ent->type == LIMINE_MEMMAP_USABLE) && (ent->length >= 4*MB)) {
			//	construct virtual address space
			sized_page_set_address(pages.system.table_heap.table[0], align(ent->base, 2*MB));

			pages.heap.global.data = pages.system.table_heap.virtual;
			table_heap_t* const h = pages.heap.global.data;

			memnull(h, sizeof(table_heap_t));

			h->meta.virtual.start = h;
			h->meta.virtual.end = (void*)((size_t)h + (2*MB));
			h->meta.physical.start = align(ent->base, 2*MB);
			h->meta.physical.end = align(ent->base, 2*MB) + (2*MB);
			h->meta.table = &pages.system.table_heap.table;
			h->meta.allocator = &pages.heap.global;
			h->meta.size = 2*MB;
			h->meta.next = null;
			return;
		}
	}

	if (vocality >= vocality_report_everything) {
		report_status("CRITICAL ERROR", *init_phase_status_line, col.critical);
	}
	report("could not allocate memory for table heap\n", report_critical);
	panic(panic_code_cannot_allocate_memory_for_kernel_heap);
	__builtin_unreachable();
}



void table_heap_debug([[maybe_unused]] table_allocator_t* alloc) {

}