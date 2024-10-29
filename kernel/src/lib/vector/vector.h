//
//	vector.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../integers.h"
#include "../memory/heap/heap.h"

#define VECTOR_REALLOC_ADD 4

typedef struct vector {
	//NOTE:		destruction of each popped/freed elements must be done manually

	void* data;
	size_t len;
	size_t bsize; //	size of each element
} vector;


__attribute__((always_inline, nonnull(1)))
inline void vec(vector *this) {
	//	initializes empty vector
	this->data = null;
	this->len = 0;
	this->bsize = 0;
}

__attribute__((always_inline, nonnull(1)))
inline void vecs(vector *this, size_t bsize) {
	//	initializes empty vector with byte size
	this->len = 0;
	this->bsize = bsize;
	this->data = null;
}

__attribute__((always_inline, nonnull(1)))
static inline void *vecsa(vector *this, size_t len, size_t bsize) {
	//	initializes vector and allocates memory for it
	this->len = len;
	this->bsize = bsize;
	if (this->len > 0) {
		this->data = alloc(this->len * this->bsize);
	} else {
		this->data = null;
	}
	return this->data;
}

__attribute__((nonnull(1, 4)))
static inline void vecsac(vector *this, size_t len, size_t bsize, void (*construct)(void *)) {
	//	initializes vector, allocates memory and constructs each element
	this->len = len;
	this->bsize = bsize;
	this->data = alloc(this->len * this->bsize);
	for (size_t i = 0; i < this->len; i++) {
		construct((void *) ((size_t) this->data + (i * this->bsize)));
	}
}

__attribute__((nonnull(1)))
void vec_resize(vector *this, size_t len);

//	resize vector
__attribute__((nonnull(1)))
void vec_resizecd(vector *this, size_t len, void (*construct)(void *), void (*destruct)(void *));
	//	resize vector, constructs pushed elements or destructs popped elements

__attribute__((always_inline, nonnull(1)))
inline void *vec_last(vector *this) {
	//	returns pointer to last element
	return (void *) ((size_t) this->data + (this->bsize * (this->len - 1)));
}

__attribute__((always_inline))
inline void *vec_at(vector *this, size_t at) {
	//	returns target element
	return (void *) (((size_t) this->data + (at * this->bsize)));
}

__attribute__((nonnull(1), returns_nonnull))
void *vec_push(vector *this, size_t count);

//	pushes to vector and returns pointer to new elements
__attribute__((returns_nonnull))
void *vec_pushc(vector *this, size_t count, void (*construct)(void *));

//	pushes to vector, constructs new elements and returns pointer to them

__attribute__((nonnull(1)))
void vec_pop(vector *this, size_t count);

//	pops vector
__attribute__((nonnull(1, 3)))
void vec_popd(vector *this, size_t count, void (*destruct)(void *));

//	pops vector and destructs popped elements

__attribute__((always_inline, nonnull(1)))
inline void vec_free(vector *this) {
	//	frees vector data (destruction of each object must be done manually)
	if (this->data != null) {
		free(this->data);
	}
	this->data = null;
	this->len = (this->bsize = 0);
}

__attribute__((always_inline, nonnull(1, 2)))
inline void vec_clear(vector *this, void (*destruct)(void *)) {
	//	frees vector data and destruct each element
	if (this->data != null) {
		for (size_t i = 0; i < this->len; i++) {
			destruct((void *) ((size_t) this->data + (i * this->bsize)));
		}
		free(this->data);
	}
}

__attribute__((nonnull(1, 2)))
void vec_take_over(vector* this, vector* other);
	//	will not delete free the "this" vector

__attribute__((nonnull(1)))
void* vec_insert(vector* this, size_t index, size_t count);


static vector memmap;
