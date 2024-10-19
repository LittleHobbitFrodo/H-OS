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

	typedef struct heap_segment_t{
		struct heap_segment_t* next;
		size_t size;
		bool used;
	} heap_segment_t;

	bool heap_reserve_memory(bool include_reclaimable_entries);

	typedef struct heap_t {

		heap_segment_t* used_until;

		heap_segment_t* start;
		void* end_physical;
		heap_segment_t* end;

		void* base_virtual;

		[[maybe_unused]] bool uses_virtual;

	} heap_t;

	static heap_t heap;

	void heap_init();

	//heap_segment_t* heap_start = null;
	//static void* heap_end_physical = null;

	//static heap_segment_t* heap_used_until = null;
	//static heap_segment_t* heap_end = null;

	//void* heap_virtual_base = null;
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