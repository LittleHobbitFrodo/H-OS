//
//	aligned_ptr.h
//		part of the CORE kernel belonging to the H-OS project
//

//	structure used for aligned allocations

#pragma once

#include "../integers.h"
#include "./heap.h"



typedef struct aligned_ptr {
	void *ptr;
	size_t offset; //	offset from start of the block
	size_t align;
} aligned_ptr;

__attribute__((always_inline, nonnull)) inline void aptr(aligned_ptr *this) {
	this->ptr = null;
	this->offset = 0;
	this->align = 0;
}

__attribute__((always_inline, nonnull)) inline void aptrs(aligned_ptr *this, size_t align) {
	this->ptr = null;
	this->offset = align;
	this->align = align;
}

__attribute__((always_inline, nonnull)) inline void aptrse(aligned_ptr *this, size_t bytes, size_t align) {
	this->align = (this->offset = align);
	this->ptr = palign_alloc(bytes, &this->offset);
}

__attribute__((nonnull(1))) void *aptr_alloc(aligned_ptr *this, size_t bytes);

__attribute__((nonnull(1), returns_nonnull)) void *aptr_realloc(aligned_ptr *this, size_t bytes);

__attribute__((nonnull(1), returns_nonnull)) void *aptr_reallocf(aligned_ptr *this, size_t bytes, void (*on_realloc)(void *));

__attribute__((nonnull(1), returns_nonnull)) void *aptr_realloca(aligned_ptr *this, size_t bytes, size_t add);

__attribute__((nonnull(1), returns_nonnull)) void *aptr_reallocaf(aligned_ptr *this, size_t bytes, size_t add, void (*on_realloc)(void *));

__attribute__((always_inline, nonnull(1))) inline void aptr_free(aligned_ptr *this) {
	align_free(this->ptr, this->offset);
	this->ptr = null;
	this->align = (this->offset = 0);
}

void va_info(void *addr);

typedef struct stack_frame {
	//	get -> read rbp
	struct stack_frame *rbp;
	u64 rip;
} __attribute__((packed)) stack_frame;