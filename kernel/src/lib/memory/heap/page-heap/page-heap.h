//
//	memory/heap/page-heap.h
//		part of the CORE kernel belonging to the H-OS project
//


//	NOTE: do NOT include in include.h

//	heap implementation dedicated for page table management
//	works with aligned data with fixed sizes
//	needs regular heap to work

#pragma once

#include "../../../integers.h"
#include "../heap.h"


static void page_heap_init();

static bool page_heap_reserve_memory();

void page_heap_debug();


[[nodiscard]] __attribute__((returns_nonnull))
page_table_t* page_alloc(u32 count);

[[nodiscard]] __attribute__((returns_nonnull))
page_table_t* page_realloc(page_table_t* table, u32 count);

void page_free(page_table_t* table);

void page_heap_debug();



/*void page_free(page_table_t* ptr);
void __page_free_locked(page_table_t* ptr);

[[nodiscard]] __attribute__((nonnull(1), returns_nonnull))
page_segment* page_find(page_table_t* ptr);
[[nodiscard]] __attribute__((nonnull(1)))
ssize_t page_find_index(page_table_t* ptr);

[[nodiscard]] __attribute__((always_inline))
inline page_table_t* page_alloc([[maybe_unused]] u32 tables) {
	return null;
}

[[nodiscard]] __attribute__((returns_nonnull))
page_table_t* __page_alloc_locked(u32 tables);

[[nodiscard]] __attribute__((nonnull(1), returns_nonnull))
page_table_t* page_realloc(page_table_t* ptr, u32 tables, bool* reallocated);


//	functions below assumes that the vector is already locked by one of functions above

void __page_heap_divide_block(size_t segment, size_t tables);

[[nodiscard]] __attribute__((always_inline))
inline page_table_t* page_heap_expand([[maybe_unused]]size_t tables, [[maybe_unused]]bool unlock) {
	return null;
}*/



/*

typedef struct page_heap_segment_t {
	page_table_t* entries;
		//	virtual address to each entry
	volatile u8 used;
	u32 table_count;	//	entry_count * 512
} page_heap_segment_t;

typedef struct page_heap_t {
	volatile_vector segments;

	struct {
		void* virtual;
		void* physical;
	} base;

	size_t used_until;
		//	index in vector

	struct physical {
		void* start;
		void* end;
	} physical;

	size_t size;	//	size in pages (4KB)

} page_heap_t;

#define PAGE_TABLE_SIZE (sizeof(page_entry) * PAGE_COUNT)

static page_heap_t page_heap;

static void page_heap_init();

static bool page_heap_reserve_memory();

void page_heap_debug();

void page_free(page_table_t* ptr);
void __page_free_locked(page_table_t* ptr);

[[nodiscard]] __attribute__((nonnull(1), returns_nonnull))
page_heap_segment_t* page_find(page_table_t* ptr);
[[nodiscard]] __attribute__((nonnull(1)))
ssize_t page_find_index(page_table_t* ptr);

[[nodiscard]] __attribute__((always_inline))
inline page_table_t* page_alloc(u32 tables);

[[nodiscard]] __attribute__((returns_nonnull))
page_table_t* __page_alloc_locked(u32 tables);

[[nodiscard]] __attribute__((nonnull(1), returns_nonnull))
page_table_t* page_realloc(page_table_t* ptr, u32 tables, bool* reallocated);


//	functions below assumes that the vector is already locked by one of functions above

void __page_heap_divide_block(size_t segment, size_t tables);

[[nodiscard]] __attribute__((always_inline))
inline page_table_t* page_heap_expand(size_t tables, bool unlock);
*/