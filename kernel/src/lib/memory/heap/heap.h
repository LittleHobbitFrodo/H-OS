//
//	memory/heap.h
//		part of the CORE kernel belonging to the H-OS project
//

//	simple and fast multipurpose heap implementation
//	for aligned allocations use the aligned_ptr structure

#pragma once

#include "../../integers.h"
#include "../paging.h"


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

bool heap_reserve_memory(bool include_reclaimable_entries);

typedef struct heap_t {

	heap_segment_t* used_until;

	//[[maybe_unused]] void* start_physical;
	heap_segment_t* start;
	heap_segment_t* end;

	struct {
		void* start;
		void* end;
	} physical;

	void* base_virtual;

} heap_t;

static heap_t heap;

static void heap_init();

void heap_debug();

__attribute__((always_inline, nonnull)) inline void free(void* ptr) {
	((heap_segment_t*)((size_t)ptr - sizeof(heap_segment_t)))->used = false;
}

__attribute__((always_inline, nonnull(1))) inline void align_free(void* ptr, size_t offset) {
	free((void*)((size_t)ptr - offset));
}

[[maybe_unused]] static bool heap_uses_virtual = false;



void* alloc(size_t bytes) __attribute__((returns_nonnull));

void* realloc(void* ptr, size_t bytes) __attribute__((nonnull, returns_nonnull));
void* realloca(void* ptr, size_t bytes, size_t add) __attribute__((nonnull, returns_nonnull));
//	reserve more/less space if reallocation is needed

__attribute__((always_inline, nonnull)) inline size_t heap_bsize(void* ptr) {
	return ((heap_segment_t*)((size_t)ptr - sizeof(heap_segment_t)))->size;
}

[[nodiscard]] void* heap_expand(size_t bytes) __attribute__((returns_nonnull));

void heap_connect(heap_segment_t* beg, heap_segment_t* end) __attribute__((nonnull(1, 2)));

heap_segment_t* heap_divide(heap_segment_t* seg, size_t size);

[[nodiscard]] void* heap_enlarge(size_t bytes) __attribute__((returns_nonnull));


void* align_alloc(size_t bytes, size_t* align) __attribute__((nonnull(2), returns_nonnull));
//	allocates aligned memory and create new block at the empty space

void* align_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) __attribute__((nonnull(1, 2), returns_nonnull));
void* align_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
//	if data is reallocated -> calls on_realloc and pass pointer to it

void* align_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) __attribute__((nonnull(1, 2), returns_nonnull));
void* align_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
//	if data is reallocated -> calls on_realloc and pass pointer to it

void* heap_align_enlarge(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));

void* heap_align_expand(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));