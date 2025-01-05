//
//	aligned_ptr.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../memory/aligned_ptr.h"

void *aptr_alloc(aligned_ptr *this, size_t bytes) {
	if (this->ptr == null) {
		this->offset = this->align;
		this->ptr = align_alloc(bytes, &this->offset);
		return this->ptr;
	}
	return null;
}

void *aptr_realloc(aligned_ptr *this, size_t bytes) {
	if (this->ptr == null) {
		return aptr_alloc(this, bytes);
	}
	return (this->ptr = align_realloc(this->ptr, &this->offset, this->align, bytes));
}

void *aptr_reallocf(aligned_ptr *this, size_t bytes, void (*on_realloc)(void *)) {
	if (this->ptr == null) {
		return aptr_alloc(this, bytes);
	}
	return (this->ptr = align_reallocf(this->ptr, &this->offset, this->align, bytes, on_realloc));
}

void *aptr_realloca(aligned_ptr *this, size_t bytes, size_t add) {
	if (this->ptr == null) {
		return aptr_alloc(this, bytes);
	}
	return (this->ptr = align_realloca(this->ptr, &this->offset, this->align, bytes, add));
}

void *aptr_reallocaf(aligned_ptr *this, size_t bytes, size_t add, void (*on_realloc)(void *)) {
	if (this->ptr == null) {
		return aptr_alloc(this, bytes);
	}
	return (this->ptr = align_reallocaf(this->ptr, &this->offset, this->align, bytes, add, on_realloc));
}