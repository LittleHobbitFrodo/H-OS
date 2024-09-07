//
//	memory.h
//		part of the CORE kernel belonging to the H-OS project
//

#include "memory/heap.h"

#pragma once

#ifndef H_OS_LIB_MEMORY_H
	#define H_OS_LIB_MEMORY_H

	#ifndef KERNEL_STACK_SIZE
		#define KERNEL_STACK_SIZE 64
	#endif

	#ifndef INTERRUPT_STACK_SIZE
		#define INTERRUPT_STACK_SIZE 8
	#endif

	#define STACK_MINIMUM_INITIAL_SIZE KERNEL_STACK_SIZE + INTERRUPT_STACK_SIZE

	#define KB 1024
	#define MB (1024 * 1024)
	#define GB (1024 * 1024 * 1024)

	extern void* kernel_stack_address;
	extern void* interrupt_stack_address;
	/*extern void* pml4_address;
	extern void* kernel_stack_base;*/
		//	these variables are initialized in page_init() (memory/paging.c)

	enum panic_codes memory_init();

	__attribute__((always_inline)) inline void* align(void* ptr, size_t align) {
		return (void*)(((u64)ptr + align - 1) & ~(align - 1));
	}

	static struct meminfo {
		size_t total;
		size_t usable;
		size_t used;

		size_t reserved;		//	reserved memory
		size_t ring0;			//	ring 0 memory (kheap, stacks, acpi, fb, ...)

		size_t unmapped;		//	bad memory

	} meminfo;

	static volatile struct limine_stack_size_request k_stack_size_req = {
		.id = LIMINE_STACK_SIZE_REQUEST,
		.revision = 0,
		.stack_size = KERNEL_STACK_SIZE * KB
	};

	static volatile struct limine_kernel_address_request k_address = {
		.id = LIMINE_KERNEL_ADDRESS_REQUEST,
		.revision = 0
	};

	static volatile struct limine_memmap_request memmap_req = {
		.id = LIMINE_MEMMAP_REQUEST,
		.revision = 0
	};

	enum memmap_types {
		usable,
		kernel_,
		heap,
		reserved,
		reclaimable,
		acpi,
		bad,
		stack,
		other,
		undefined
	} memmap_types;

	typedef struct memmap_entry {
		size_t base;
		size_t len;
		enum memmap_types type;
	} memmap_entry;

	enum memmap_types memmap_entry_type(u64 constant);

	void memmap_parse();

	void memmap_reclaim();

	void memmap_display(bool original);


	typedef struct aligned_ptr {
		void* ptr;
		size_t offset;		//	offset from start of the block
		size_t align;
	} aligned_ptr;

	__attribute__((always_inline)) inline void aptr(aligned_ptr* this) {
		this->ptr = null;
		this->offset = 0;
		this->align = 0;
	}
	__attribute__((always_inline)) inline void aptrs(aligned_ptr* this, size_t align) {
		this->ptr = null;
		this->offset = align;
		this->align = align;
	}

	__attribute__((always_inline)) inline void aptrse(aligned_ptr* this, size_t bytes, size_t align) {
		this->align = (this->offset = align);
		this->ptr = align_alloc(bytes, &this->offset);
	}

	void* aptr_alloc(aligned_ptr* this, size_t bytes);
	void* aptr_realloc(aligned_ptr* this, size_t bytes);
	void* aptr_reallocf(aligned_ptr* this, size_t bytes, void (*on_realloc)(void*));
	void* aptr_realloca(aligned_ptr* this, size_t bytes, size_t add);
	void* aptr_reallocaf(aligned_ptr* this, size_t bytes, size_t add, void (*on_realloc)(void*));
	__attribute__((always_inline)) inline void aptr_free(aligned_ptr* this) {
		align_free(this->ptr, this->offset);
		this->ptr = null;
		this->align = (this->offset = 0);
	}


	//	memmap vector is declared in vector.h

#endif
//	#warning memory.h already included
//#endif