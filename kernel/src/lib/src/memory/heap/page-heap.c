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

}

void page_heap_reseve_memory() {
	//	inserts into memmap vector
	//	find valid space
	//	fills page_heap structure
}

void page_alloc(page_alloc_t* ptr, size_t tables) {
	page_heap_segment_t* seg;
	for (size_t i = page_heap.used_until; i < page_heap.segments.len; i++) {
		seg = (page_heap_segment_t*)vec_at(&page_heap.segments, i);
		if (!seg->used) {
			if (seg->table_count > tables) {
				//	divide block

			} else if (seg->table_count == tables) {
				seg->used = true;
				seg->seg_entry = i;
				ptr->entries = (page_table_t*)seg->entries;
				ptr->seg_entry = i;
				ptr->table_count = tables;
			}
		}
	}
	page_heap_enlarge(ptr, tables);
}

void page_realloc(page_alloc_t* ptr, size_t tables, bool* reallocated) {
	//	find corresponding segment

	page_heap_segment_t *segs = page_heap.segments.data, *cur = &segs[ptr->seg_entry];
	if (cur->table_count >= tables) {
		if (reallocated != null) {
			*reallocated = true;
		}
		return;
	}
	for (size_t i = 0; i < page_heap.segments.len; i++) {
		if (unlikely((!segs[i].used) && (segs[i].table_count >= tables))) {
			segs[i].used = true;
			memcpy(cur->entries, segs[i].entries, sizeof(page_entry) * PAGE_COUNT * ptr->table_count);
			ptr->entries = segs[i].entries;
			ptr->seg_entry = i;
			ptr->table_count = tables;
			cur->used = false;
			if (reallocated != null) {
				*reallocated = true;
			}
			return;
		}
	}
	page_heap_enlarge(ptr, tables);
}

void page_heap_divide_block(size_t segment, size_t tables) {
	page_heap_segment_t* segs = page_heap.segments.data;
	for (ssize_t i = page_heap.segments.len - 1; i > (ssize_t)segment+1; i--) {
		segs[i] = segs[i - 1];
	}
	page_heap_segment_t* new = vec_at(&page_heap.segments, segment+1);
	page_heap_segment_t* old = vec_at(&page_heap.segments, segment);
	old->table_count = new->table_count - tables;
	new->table_count = tables;
	new->entries = old->entries + tables;
	new->used = false;
	new->seg_entry = segment+1;
}

void page_heap_enlarge(page_alloc_t* ptr, size_t tables) {
	page_heap_segment_t* last = vec_last(&page_heap.segments);
	if (!last->used) {
		last->table_count = tables;
		last->used = true;
		ptr->entries = last->entries;
		ptr->seg_entry = last->seg_entry;
		ptr->table_count = tables;
		return;
	}
	page_heap_expand(ptr, tables);
}

void page_heap_expand(page_alloc_t* ptr, size_t tables) {
	page_heap_segment_t* last = vec_last(&page_heap.segments);
	page_heap_segment_t* new = vec_push(&page_heap.segments, 1);
	new->entries = last->entries + last->table_count;
	new->seg_entry = page_heap.segments.len - 1;
	new->used = true;
	ptr->entries = new->entries;
	ptr->seg_entry = new->seg_entry;
	ptr->table_count = tables;
}