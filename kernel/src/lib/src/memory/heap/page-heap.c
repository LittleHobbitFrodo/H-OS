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

	page_heap_segment_t* seg = vvec_push(&page_heap.segments, 1);
	seg->entries = page_heap.base.virtual_;
	seg->table_count = 1;
	seg->used = false;
}

bool page_heap_reserve_memory() {
	//	updates the page_heap.physical.start and .end variable

	memnull(&page_heap, sizeof(page_heap_t));
	vvecs(&page_heap.segments, sizeof(page_heap_segment_t));
		//	initializes volatile vector

	struct limine_memmap_entry* ent;
	struct {
		struct limine_memmap_entry **entries;
		size_t size;
	} mmp;
	mmp.entries = req_memmap.response->entries;
	mmp.size = req_memmap.response->entry_count;


	//	calculate size for page heap
	page_heap.size = (size_t)heap.physical.end - (size_t)heap.physical.start;
		//	get heap size
	page_heap.size = (page_heap.size / PAGE_SIZE) * (page_heap.size % PAGE_SIZE != 0);
		//	get page count
	page_heap.size = ((page_heap.size / PAGE_COUNT) + 1) * PAGE_COUNT;
		//	round up to 512


	//	find good spot for it
	for (size_t i = 0; i < mmp.size; i++) {
		//	find usable memmap entry big enough
		ent = mmp.entries[i];
		if ((ent->type == LIMINE_MEMMAP_USABLE) && (ent->base != (u64)heap.physical.start)) {
			if (ent->length >= (page_heap.size * PAGE_SIZE)) {
				//	pre-initialize page heap
				page_heap.physical.start = (void*)ent->base;
				page_heap.physical.end = (void*)((size_t)page_heap.physical.start + (page_heap.size * PAGE_SIZE));
				return true;
			}
		}
	}
	return false;


	/*page_heap.physical.start = page_heap.base.physical;
	page_heap.physical.end = (void*)((size_t)page_heap.base.physical + (page_heap.size * sizeof(page_table_t)));

	memmap_entry *ent, *last = null;
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
	return new;*/
}

void page_heap_debug() {
	u32 c = output.color;
	output.color = col.blue;
	printl("page heap scheme:");
	page_heap_segment_t* seg;
	for (size_t i = 0; i < page_heap.segments.len; i++) {
		seg = vvec_at_unlocked(&page_heap.segments, i);
		printu(i); print(":\t"); printp(seg->entries); print(" : "); printp((void*)(((size_t)seg->entries - (size_t)page_heap.base.virtual_) + (size_t)page_heap.base.physical)); endl();
		printc('\t'); printu(seg->table_count); print("\t"); printl((seg->used)? "used" : "unused");
	}
	output.color = c;
}

void page_heap_map() {

}
