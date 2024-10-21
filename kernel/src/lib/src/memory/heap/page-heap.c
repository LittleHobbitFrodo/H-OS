//
//	memory/heap/page-heap.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../memory/heap/page-heap.h"

void page_heap_init() {
	//	allocate page tables into regular heap
	//	add them into paging structure (test it)
	//	reserve memory for page heap
	//	cover page heap in virtual addresses
	//	allocate tables (in page heap) and add them into paging structure
	//	make the map the page heap
	//	delete allocated page tables (regular heap)
	//	initialize page heap


	memnull(&page_heap, sizeof(page_heap_t));
	vecs(&page_heap.segments, sizeof(page_heap_segment_t));
	page_heap_segment_t* seg = vec_push(&page_heap.segments, 1);
	seg->entries = page_heap.base.virtual_;
	seg->table_count = 1;
	seg->used = false;
}

memmap_entry* page_heap_reserve_memory() {
	//	page_heap.size must be set before calling this function

	page_heap.physical.start = page_heap.base.physical;
	page_heap.physical.end = (void*)((size_t)page_heap.base.physical + (page_heap.size * sizeof(page_table_t)));

	memmap_entry *ent, *last = null;
	memmap_display();
	memmap_entry* new = null;
	for (size_t i = 0; i < memmap.len; i++) {
		//	find usable memmap entry big enough
		if (i > 0) {
			last = ent;
		}
		ent = vec_at(&memmap, i);
		if ((ent->type == memmap_usable) && ((last != null) && (last->type != memmap_heap))) {
			if (ent->len >= (page_heap.size * PAGE_SIZE)) {
				//	pre-initialize page heap
				new = vec_insert(&memmap, i, 1);
				ent = vec_at(&memmap, i+1);
				if (new == null) {
					return null;
				}
				new->base = ent->base;
				new->len = page_heap.size * PAGE_SIZE;
				new->type = memmap_heap;

				ent->len -= new->len;
				ent->base += new->len;
				break;
			}
		}
	}
	return new;
}

