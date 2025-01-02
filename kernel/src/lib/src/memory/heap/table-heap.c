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

any_page_table* table_alloc(table_allocator_t* alloc, u16 count) {
	u64* const map = alloc->data->bitmap;
	ssize_t index = bitmap_find_cleared(alloc->data->bitmap, 64, count);
	if (index >= 0) {
		//	found
		bitmap_set(map, index, count);
		table_ptr* ptr = &alloc->data->ptr[index];
		ptr->used = true;
		ptr->count = count;
		ptr->index = index;
		return &alloc->data->table[index];
	}
	return null;
}

void table_free(table_allocator_t* alloc, any_page_table* table) {
	table = (any_page_table*)align(table, 4096);
	const size_t index = (((size_t)table - (size_t)alloc->data) / sizeof(any_page_table)) - 1;
	table_ptr* ptr = &alloc->data->ptr[index];
	u16 count = ptr->count;
	ptr->used = false;

	bitmap_clear(alloc->data->bitmap, index, count);
}


void table_shrink(table_allocator_t* alloc, any_page_table* table, u16 count) {
	const size_t index = (((size_t)table - (size_t)alloc->data)/sizeof(any_page_table)) - 1;
	table_ptr* ptr = &alloc->data->ptr[index];
	if (count >= ptr->count) {
		return;
	}
	const size_t c = ptr->count;
	ptr->count = count;
	bitmap_clear(alloc->data->bitmap, index + count, c - count);
}


void table_heap_debug(const table_allocator_t* alloc) {
	//	prints only used blocks
	const u32 c = output.color;
	output.color = col.cyan;
	print("table allocator "); printp((void*)alloc); printl(" scheme:");
	if (alloc->data == null) {
		printl("\tNULL");
		output.color = c;
		return;
	}
	const table_ptr* ptr = alloc->data->ptr;
	for (size_t i = 0; i < TABLE_HEAP_TABLE_COUNT; i++) {
		if (ptr[i].used) {
			tab(); printu(i); print(":\tcount:\t"); printu(ptr->count); endl();
			print("\t\t"); print("address:\t"); printp(&alloc->data->table[i]); endl();
			i += ptr->count;
		}
	}
	output.color = c;
}