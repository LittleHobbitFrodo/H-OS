//
//	memory/heap.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_MEMORY_HEAP_H
	#define H_OS_LIB_MEMORY_HEAP_H

	#ifndef HEAP_INITIAL_BLOCK_SIZE
		#define HEAP_INITIAL_BLOCK_SIZE 32
	#endif

	#define HEAP_MINIMAL_ENTRY_SIZE 512
		//	minimum memmap entry size (in kb)

	typedef struct heap_segment{
		struct heap_segment* next;
		size_t size;
		bool used;
	} heap_segment;

	bool heap_reserve_memory(bool include_reclaimable_entries);

	void heap_init();

	static heap_segment* heap_start = null;
	static void* heap_end_physical = null;

	static heap_segment* heap_used_until = null;
	static heap_segment* heap_end = null;

	void heap_debug();


	__attribute__((always_inline, nonnull)) inline void free(void* ptr) {
		((heap_segment*)((size_t)ptr - sizeof(heap_segment)))->used = false;
	}

	__attribute__((always_inline, nonnull(1))) inline void align_free(void* ptr, size_t offset) {
		free((void*)((size_t)ptr - offset));
	}

	#include "./heap/physical.h"
	#include "./heap/virtual.h"


#endif
//	#warning memory/heap.h already included
//#endif