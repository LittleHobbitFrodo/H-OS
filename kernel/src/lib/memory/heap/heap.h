//
//	memory/heap.h
//		part of the CORE kernel belonging to the H-OS project
//

//	simple and fast multipurpose heap implementation
//	for aligned allocations use the aligned_ptr structure

#pragma once

#include "../../integers.h"


#ifndef HEAP_INITIAL_BLOCK_SIZE
	#define HEAP_INITIAL_BLOCK_SIZE 32
#endif

#define HEAP_MINIMAL_ENTRY_SIZE 512
		//	minimum memmap entry size (in kb)

typedef struct heap_segment_t{
	struct heap_segment_t* next;
	size_t size;
	bool used;
} heap_segment_t;

struct page_table_t;

bool heap_reserve_memory(bool include_reclaimable_entries);

typedef struct heap_t {

	heap_segment_t* used_until;

	//[[maybe_unused]] void* start_physical;
	heap_segment_t* start;		//	first block
	heap_segment_t* end;		//	last block

	struct {
		size_t start;
		size_t end;
	} physical;

	void* virtual_start;
	void* virtual_end;

	void* table;		//	page_table_t

} heap_t;

heap_t heap;

static void heap_init();

void heap_debug();

bool heap_map(size_t physical, void** virt, size_t size, bool* table_created);

__attribute__((always_inline, nonnull))
inline void free(void* ptr) {
	heap_segment_t* seg = (heap_segment_t*)((size_t)ptr - sizeof(heap_segment_t));
	if ((size_t)heap.used_until > (size_t)seg) {
		heap.used_until = seg;
	}
	seg->used = false;
}

__attribute__((always_inline, nonnull(1))) inline void align_free(void* ptr, size_t offset) {
	free((void*)((size_t)ptr - offset));
}

[[maybe_unused]] static bool heap_uses_virtual = false;



[[nodiscard]] void* alloc(size_t bytes) __attribute__((returns_nonnull));

[[nodiscard]] void* realloc(void* ptr, size_t bytes) __attribute__((nonnull, returns_nonnull));
[[nodiscard]] void* realloca(void* ptr, size_t bytes, size_t add) __attribute__((nonnull, returns_nonnull));
//	reserve more/less space if reallocation is needed

__attribute__((always_inline, nonnull)) inline size_t heap_bsize(void* ptr) {
	return ((heap_segment_t*)((size_t)ptr - sizeof(heap_segment_t)))->size;
}

[[nodiscard]] void* heap_expand(size_t bytes) __attribute__((returns_nonnull));

void heap_connect(heap_segment_t* beg, heap_segment_t* end) __attribute__((nonnull(1, 2)));

heap_segment_t* heap_divide(heap_segment_t* seg, size_t size);

[[nodiscard]] void* heap_enlarge(size_t bytes) __attribute__((returns_nonnull));


[[nodiscard]] void* align_alloc(size_t bytes, size_t* align) __attribute__((nonnull(2), returns_nonnull));
//	allocates aligned memory and create new block at the empty space

[[nodiscard]] void* align_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) __attribute__((nonnull(1, 2), returns_nonnull));
[[nodiscard]] void* align_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
//	if data is reallocated -> calls on_realloc and pass pointer to it

[[nodiscard]] void* align_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) __attribute__((nonnull(1, 2), returns_nonnull));
[[nodiscard]] void* align_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
//	if data is reallocated -> calls on_realloc and pass pointer to it

[[nodiscard]] void* heap_align_enlarge(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));

[[nodiscard]] void* heap_align_expand(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));