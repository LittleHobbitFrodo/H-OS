//
//	memory/heap/page-heap.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//	heap implementation dedicated for page table management

#include "../../integers.h"
#include "../paging.h"
#include "../../memory.h"
#include "../../vector.h"

typedef struct page_heap_segment_t {
	page_table_t* entries;
		//	virtual address to each entry
	volatile u8 used;
	u16 table_count;	//	entry_count * 512
	u32 seg_entry;
} page_heap_segment_t;

typedef struct page_alloc_t {
	page_table_t* entries;
	u32 table_count;
	u32 seg_entry;
} page_alloc_t;

typedef struct page_heap_t {
	vector segments;

	struct {
		void* virtual_;
		void* physical;
	} base;

	size_t used_until;
		//	index in vector
	page_heap_segment_t* start;
	page_heap_segment_t* end;

	struct physical {
		void* start;
		void* end;
	} physical;

} page_heap_t;

static page_heap_t page_heap;

[[maybe_unused]] static void page_heap_init();

[[maybe_unused]] static void page_heap_reseve_memory();

void page_alloc(page_alloc_t* ptr, size_t tables);
//__attribute__((nonnull(1))) void page_free(page_table_t* table);
#ifndef KERNEL_DEBUG
__attribute__((always_inline, nonnull(1))) static inline void page_free(page_alloc_t* table)
#else
__attribute__((nonnull(1))) static void page_free(page_alloc_t* table)
#endif
{
	((page_heap_segment_t*)vec_at(&page_heap.segments, table->seg_entry))->used = false;
}

__attribute__((nonnull(1))) void page_realloc(page_alloc_t* ptr, size_t tables, bool* reallocated);
void page_heap_divide_block(size_t segment, size_t tables);

__attribute__((nonnull(1))) void page_heap_enlarge(page_alloc_t* ptr, size_t tables);
__attribute__((nonnull(1))) void page_heap_expand(page_alloc_t* ptr, size_t tables);
	//	these two functions returns physical addresses