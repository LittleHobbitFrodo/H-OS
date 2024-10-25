//
//	memory/heap/page-heap.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//	heap implementation dedicated for page table management
//	works with aligned data with fixed sizes
//	needs regular heap to work

#include "../../integers.h"
#include "../paging.h"
#include "../../memory.h"
#include "../../vector/volatile-vector.h"
#include "../../memory/aligned_ptr.h"

typedef struct page_heap_segment_t {
	page_table_t* entries;
		//	virtual address to each entry
	volatile u8 used;
	u32 table_count;	//	entry_count * 512
} page_heap_segment_t;

typedef struct page_alloc_t {
	page_table_t* entries;
	u32 table_count;
} page_alloc_t;

typedef struct page_heap_t {
	volatile_vector segments;

	struct {
		void* virtual_;
		void* physical;
	} base;

	size_t used_until;
		//	index in vector

	struct physical {
		void* start;
		void* end;
	} physical;

	size_t size;	//	size in pages (4KB)

	struct tmp {
		aligned_ptr pdpt;
	} tmp;

	struct static_tables {
		page_alloc_t pdpt;
	} static_tables;

} page_heap_t;

#define PAGE_TABLE_SIZE (sizeof(page_entry) * PAGE_COUNT)

static page_heap_t page_heap;

/*static */void page_heap_init();

static bool page_heap_reserve_memory();

void page_heap_map();

page_table_t* page_alloc(page_alloc_t* ptr, u32 tables);
//	return physical address (virtual one is stored in ptr)

/*__attribute__((always_inline, nonnull(1)))
static inline void page_free(page_alloc_t* table) {
	((page_heap_segment_t*)vec_at(&page_heap.segments, table->seg_entry))->used = false;
}*/

__attribute__((nonnull(1)))
page_table_t* page_realloc(page_alloc_t* ptr, size_t tables, bool* reallocated);

void page_heap_divide_block(size_t segment, size_t tables);

__attribute__((nonnull(1), returns_nonnull))
page_table_t* page_heap_enlarge(page_alloc_t* ptr, size_t tables);

__attribute__((nonnull(1), returns_nonnull))
page_table_t* page_heap_expand(page_alloc_t* ptr, size_t tables);
	//	these two functions returns physical addresses

void page_heap_debug();