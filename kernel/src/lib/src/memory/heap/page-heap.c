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
	seg->entries = page_heap.base.virtual;
	seg->table_count = 1;
	seg->used = false;
	vvec_unlock((&page_heap.segments));
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
				page_heap.base.virtual = (void*)((size_t)pages.hhdm + (size_t)page_heap.physical.start);
				return true;
			}
		}
	}
	return false;
}

void page_heap_debug() {
	vvec_wait_and_lock((&page_heap.segments));
	u32 c = output.color;
	output.color = col.blue;
	printl("page heap scheme:");
	page_heap_segment_t* seg;
	for (size_t i = 0; i < page_heap.segments.len; i++) {
		seg = vvec_at(&page_heap.segments, i);
		printu(i); print(":\t"); printp(seg->entries); print(" : "); printu(seg->table_count); print("\t:\t"); printl((seg->used)? "used" : "unused");
	}
	output.color = c;
	vvec_unlock((&page_heap.segments));
}

page_table_t* page_alloc(u32 tables) {
	vvec_wait_and_lock((&page_heap.segments));
	page_table_t* ret = __page_alloc_locked(tables);
	vvec_unlock((&page_heap.segments));
	return ret;
}

page_table_t* __page_alloc_locked(u32 tables) {
	page_heap_segment_t* segs = page_heap.segments.data;
	for (size_t i = page_heap.used_until; i < page_heap.segments.len; i++) {
		if ((!segs[i].used) && (segs[i].table_count >= tables)) {
			if ((segs[i].table_count > tables) && (segs[i].table_count > 1)) {
				__page_heap_divide_block(i, tables);
			}
			segs[i].used = true;
			if (page_heap.used_until < i) {
				page_heap.used_until = i;
			}
			return segs[i].entries;
		}
	}

	//	enlarges page heap
	page_heap_segment_t* last = vvec_last(&page_heap.segments);
	if (!last->used) {
		last->used = true;
		last->table_count = tables;
		return last->entries;
	}

	return page_heap_expand(tables, false);
}

page_table_t* page_realloc(page_table_t* ptr, u32 tables, bool* reallocated) {
	vvec_wait_and_lock((&page_heap.segments));
	ssize_t index = page_find_index(ptr);
	if (index == -1) {
		page_table_t* ret = __page_alloc_locked(tables);
		vvec_unlock((&page_heap.segments));
		return ret;
	}
	page_heap_segment_t* seg = vvec_at(&page_heap.segments, index);
	if (seg->table_count >= tables) {
		if (reallocated != null) {
			*reallocated = false;
		}
		if ((seg->table_count > tables) && (seg->table_count > 1)) {
			__page_heap_divide_block(index, tables);
		}
		vvec_unlock((&page_heap.segments));
		return ptr;
	}
	page_table_t* new = page_alloc(tables);
	page_cpy(ptr, new, PAGE_TABLE_SIZE * ((seg->table_count > tables)? tables : seg->table_count));
	seg->used = false;
	vvec_unlock((&page_heap.segments));
	return new;
}





page_table_t* page_heap_expand(size_t tables, bool unlock) {
	//	takes the vector as if it is already locked
	//	will unlock the vector

	page_heap_segment_t* last = vvec_last(&page_heap.segments);
	page_heap_segment_t* new = vec_push((vector*)&page_heap.segments, 1);
	new->used = true;
	new->entries = last->entries + last->table_count;
	new->table_count = tables;
	if (unlock) {
		vvec_unlock((&page_heap.segments));
	}
	return new->entries;
}

void __page_heap_divide_block(size_t segment, [[maybe_unused]] size_t tables) {
	//	divides page heap block -> resize page_heap.segments[segment] to tables
	//	function assumes:
		//	vector is already locked and not accessed
		//	tables < segments[segment].table_count

	vec_push((vector*)&page_heap.segments, 1);
	page_heap_segment_t* segs = page_heap.segments.data;
	for (size_t i = page_heap.segments.len - 1; i > segment + 1; i--) {
		segs[i] = segs[i-1];
	}
	page_heap_segment_t* old = &segs[segment];
	page_heap_segment_t* new = &segs[segment+1];
	new->entries = old->entries + tables;
	new->table_count = old->table_count - tables;
	new->used = false;
	old->table_count = tables;
}


void page_free(page_table_t* ptr) {
	vvec_wait_and_lock((&page_heap.segments));
	page_heap_segment_t* seg = page_find(ptr);
	if (seg != null) {
		seg->used = false;
	}
	vvec_unlock((&page_heap.segments));
}

page_heap_segment_t* page_find(page_table_t* ptr) {
	//	uses the vector as already locked
	//	binary search implementation
	page_heap_segment_t *segs = page_heap.segments.data;
	size_t low = 0, high = page_heap.segments.len - 1, mid;

	do {
		mid = low + (high - low) / 2;

		if (segs[mid].entries == ptr) {
			return &segs[mid];
		}

		if (segs[mid].entries < ptr) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}

	} while (low <= high);

	return null;
}

ssize_t page_find_index(page_table_t* ptr) {
	//	uses the vector as already locked
	//	binary search implementation
	page_heap_segment_t *segs = page_heap.segments.data;
	size_t low = 0, high = page_heap.segments.len - 1, mid;

	do {
		mid = low + (high - low) / 2;

		if (segs[mid].entries == ptr) {
			return mid;
		}

		if (segs[mid].entries < ptr) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}

	} while (low <= high);

	return -1;
}