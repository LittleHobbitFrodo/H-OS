//
//	memory/heap/virtual.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_MEMORY_HEAP_VIRTUAL_H
	#define H_OS_LIB_MEMORY_HEAP_VIRTUAL_H


	//	virtual allocation is yet not functional
	//	these functions returns physical addresses

	__attribute__((always_inline, returns_nonnull))
	 static inline void* alloc(size_t bytes) {
		return (void*)((size_t)palloc(bytes) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(1)))
	 static inline void* realloc(void* ptr, size_t bytes) {
		return (void*)((size_t)prealloc(ptr, bytes) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(1)))
	 static inline void* realloca(void* ptr, size_t bytes, size_t add) {
		return (void*)((size_t)prealloca(ptr, bytes,add) - (size_t)heap.start + (size_t)heap.base_virtual);
	}


	__attribute__((always_inline, returns_nonnull))
	 static inline void* heap_expand(size_t bytes) {
		return (void*)((size_t)pheap_expand(bytes) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull))
	 static inline void* heap_enlarge(size_t bytes) {
		return (void*)((size_t)pheap_enlarge(bytes) - (size_t)heap.start + (size_t)heap.base_virtual);
	}


	__attribute__((always_inline, returns_nonnull, nonnull(2)))
	 static inline void* align_alloc(size_t bytes, size_t* align) {
		return (void*)((size_t)palign_alloc(bytes, align) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(1, 2)))
	 static inline void* align_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) {
		return (void*)((size_t)palign_realloc(ptr, offset, align, bytes) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(1, 2)))
	 static inline void* align_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*onrealloc)(void*)) {
		return (void*)((size_t)palign_reallocf(ptr, offset, align, bytes, onrealloc) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(1, 2)))
	 static inline void* align_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) {
		return (void*)((size_t)palign_realloca(ptr, offset, align, bytes, add) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(1, 2)))
	 static inline void* align_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*onrealloc)(void*)) {
		return (void*)((size_t)palign_reallocaf(ptr, offset, align, bytes, add, onrealloc) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(2)))
	 static inline void* heap_align_enlarge(size_t bytes, size_t* align) {
		return (void*)((size_t)pheap_align_enlarge(bytes, align) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

	__attribute__((always_inline, returns_nonnull, nonnull(2)))
	 static inline void* heap_align_expand(size_t bytes, size_t* align) {
		return (void*)((size_t)pheap_align_expand(bytes, align) - (size_t)heap.start + (size_t)heap.base_virtual);
	}

#endif