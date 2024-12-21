//
//	memory/heap/multipurpose/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

//	multipurpose heap implementation with use of allocators
	//	atomicity -> each core has its own heap

#include "../../../integers.h"

#pragma once

typedef u32 heap_size_t;

typedef struct heap_metadata {
	//	required by page heap

	struct {
		size_t start;
		size_t end;
	} physical;

	size_t size;

	struct {
		void* start;
		void* end;
	} virtual;

	void* table;

	void* allocator;

} heap_metadata;

typedef struct heap_block {
	struct heap_block* next;
	heap_size_t size;
	_Atomic u8 lock;
	volatile u8 used;
} __attribute__((aligned(8))) heap_block;

typedef struct allocator_t {
	heap_block* first;
	heap_block* last;
	heap_block* first_free;
	heap_metadata meta;
	volatile u8 lock;		//	foreign allocations locks the alloc
	volatile u8 pending;		//	foreign allocations must wait till this is clear

	void* (*alloc)(struct allocator_t* self, heap_size_t bytes);
	void* (*realloc)(struct allocator_t* self, void* ptr, heap_size_t bytes);
	void* (*realloca)(struct allocator_t* self, void* ptr, heap_size_t bytes, heap_size_t add);
	void (*free)(struct allocator_t* alloc, void* ptr);

} allocator_t;


typedef struct heap_t {
	allocator_t global;
	allocator_t* allocators;
	size_t count;
} heap_t;

heap_t heap = {0};

