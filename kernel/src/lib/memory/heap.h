//
//	memory/heap.h
//		part of the CORE kernel belonging to the H-OS project
//

//	simple and fast multipurpose heap implementation
//	for aligned allocations use the aligned_ptr structure

#pragma once

#include "../integers.h"
#include "./paging.h"

#ifndef H_OS_LIB_MEMORY_HEAP_H
	#define H_OS_LIB_MEMORY_HEAP_H

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

	/*static*/ void heap_init();
	/*static*/ void heap_map();

	void heap_debug();

	__attribute__((always_inline, nonnull)) inline void free(void* ptr) {
		((heap_segment_t*)((size_t)ptr - sizeof(heap_segment_t)))->used = false;
	}

	__attribute__((always_inline, nonnull(1))) inline void align_free(void* ptr, size_t offset) {
		free((void*)((size_t)ptr - offset));
	}

	[[maybe_unused]] static bool heap_uses_virtual = false;


	#include "./heap/physical.h"
	#include "./heap/virtual.h"

#endif
//	#warning memory/heap.h already included
//#endif