//
//	memory/heap/physical.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_MEMORY_HEAP_PHYSICAL_H
	#define H_OS_LIB_MEMORY_HEAP_PHYSICAL_H
	//	heap management with physical addresses

	void* palloc(size_t bytes) __attribute__((returns_nonnull));

	void* prealloc(void* ptr, size_t bytes) __attribute__((nonnull, returns_nonnull));
	void* prealloca(void* ptr, size_t bytes, size_t add) __attribute__((nonnull, returns_nonnull));
		//	reserve more/less space if reallocation is needed

	__attribute__((always_inline, nonnull)) inline size_t heap_bsize(void* ptr) {
		return ((heap_segment*)((size_t)ptr - sizeof(heap_segment)))->size;
	}

	[[nodiscard]] void* pheap_expand(size_t bytes) __attribute__((returns_nonnull));

	void heap_connect(heap_segment* beg, heap_segment* end) __attribute__((nonnull(1, 2)));

	heap_segment* heap_divide(heap_segment* seg, size_t size);

	[[nodiscard]] void* pheap_enlarge(size_t bytes) __attribute__((returns_nonnull));


	void* palign_alloc(size_t bytes, size_t* align) __attribute__((nonnull(2), returns_nonnull));
		//	allocates aligned memory and create new block at the empty space

	void* palign_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) __attribute__((nonnull(1, 2), returns_nonnull));
	void* palign_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
		//	if data is reallocated -> calls on_realloc and pass pointer to it

	void* palign_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) __attribute__((nonnull(1, 2), returns_nonnull));
	void* palign_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
		//	if data is reallocated -> calls on_realloc and pass pointer to it

	void* pheap_align_enlarge(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));

	void* pheap_align_expand(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));

	//void* heap_align_enlargeo_p(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));
	//void* heap_align_expando_p(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));

#endif