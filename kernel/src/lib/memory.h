//
//	memory.h
//		part of the CORE kernel belonging to the H-OS project
//

#include "memory/heap.h"

#pragma once

#ifndef H_OS_LIB_MEMORY_H
	#define H_OS_LIB_MEMORY_H

	#define KB 1024
	#define MB (1024 * 1024)
	#define GB (1024 * 1024 * 1024)



	//	kernel stack (initialized in init.asm)
	char KERNEL_STACK[65536] __attribute__((aligned(4096)));
	extern void* KERNEL_STACK_END;

	char INTERRUPT_STACKS[7][INTERRUPT_STACK_SIZE * KB] __attribute__((aligned(4096)));


	static void memory_init();

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

	enum memmap_types {
		memmap_usable,
		memmap_kernel,
		memmap_heap,
		memmap_reserved,
		memmap_reclaimable,
		memmap_acpi,
		memmap_bad,
		memmap_stack,
		memmap_other,
		memmap_paging,
		memmap_undefined
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

	__attribute__((always_inline, nonnull)) inline void aptr(aligned_ptr* this) {
		this->ptr = null;
		this->offset = 0;
		this->align = 0;
	}
	__attribute__((always_inline, nonnull)) inline void aptrs(aligned_ptr* this, size_t align) {
		this->ptr = null;
		this->offset = align;
		this->align = align;
	}

	__attribute__((always_inline, nonnull)) inline void aptrse(aligned_ptr* this, size_t bytes, size_t align) {
		this->align = (this->offset = align);
		this->ptr = palign_alloc(bytes, &this->offset);
	}

	__attribute__((nonnull(1))) void* aptr_alloc(aligned_ptr* this, size_t bytes);

	__attribute__((nonnull(1), returns_nonnull)) void* aptr_realloc(aligned_ptr* this, size_t bytes);
	__attribute__((nonnull(1), returns_nonnull)) void* aptr_reallocf(aligned_ptr* this, size_t bytes, void (*on_realloc)(void*));
	__attribute__((nonnull(1), returns_nonnull)) void* aptr_realloca(aligned_ptr* this, size_t bytes, size_t add);
	__attribute__((nonnull(1), returns_nonnull)) void* aptr_reallocaf(aligned_ptr* this, size_t bytes, size_t add, void (*on_realloc)(void*));
	__attribute__((always_inline, nonnull(1))) inline void aptr_free(aligned_ptr* this) {
		align_free(this->ptr, this->offset);
		this->ptr = null;
		this->align = (this->offset = 0);
	}

	void va_info(void* addr);

	typedef struct stack_frame {
		//	get -> read rbp
		struct stack_frame* rbp;
		u64 rip;
	} __attribute__((packed)) stack_frame;


	//	memmap vector is declared in vector.h

#endif
//	#warning memory.h already included
//#endif