//
//	vector/aligned-vector.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

typedef struct aligned_vector {
	//NOTE:		destruction of each popped/freed element must be done manually
	aligned_ptr data;
	size_t bsize;
	size_t len;
} aligned_vector;


__attribute__((always_inline, nonnull(1)))
inline void avec(aligned_vector *this) {
	//	initializes empty avector
	aptr(&this->data);
	this->len = 0;
	this->bsize = 0;
}

__attribute__((always_inline, nonnull(1)))
static inline void avecsa(aligned_vector *this, size_t len, size_t bsize, size_t align) {
	//	initializes vector with allocation
	this->len = len;
	this->bsize = bsize;
	aptrse(&this->data, this->len * this->bsize, align);
}

__attribute__((nonnull(1)))
void avec_resize(aligned_vector *this, size_t len);

//	resize vector
__attribute__((nonnull(1, 3)))
void avec_resizef(aligned_vector *this, size_t len, void (*onrealloc)(void *));

//	resize vector and call function if data is reallocated
__attribute__((nonnull(1)))
void avec_resizecd(aligned_vector *this, size_t len, void (*construct)(void *), void (*destruct)(void *));

//	resize vector and calls function to destruct all popped elements
__attribute__((nonnull(1)))
void avec_resizecdf(aligned_vector *this, size_t len, void (*construct)(void *), void (*destruct)(void *), void (*onrealloc)(void *));

//	resize vector, destruct popped elements, call function on reallocation

__attribute__((always_inline, nonnull(1)))
inline void *avec_last(const aligned_vector *this) {
	void *r = null;
	if (this->data.ptr != null) {
		r = (void *) ((size_t) this->data.ptr + (this->bsize * (this->len - 1)));
	}
	return r;
}

__attribute__((always_inline))
inline void *avec_at(aligned_vector *this, size_t i) {
	void *r = null;
	if (this->data.ptr != null) {
		r = (void *) ((size_t) this->data.ptr + (i * this->bsize));
	}
	return r;
}

__attribute__((nonnull(1), returns_nonnull))
void *avec_push(aligned_vector *this, size_t count);

__attribute__((nonnull(1, 3), returns_nonnull))
void *avec_pushc(aligned_vector *this, size_t count, void (*construct)(void *));

__attribute__((nonnull(1, 3), returns_nonnull))
void *avec_pushf(aligned_vector *this, size_t count, void (*onrealloc)(void *));

__attribute__((nonnull(1, 3, 4), returns_nonnull))
void *avec_pushcf(aligned_vector *this, size_t count, void (*construct)(void *), void (*onrealloc)(void *));


__attribute__((nonnull(1)))
void avec_pop(aligned_vector *this, size_t count);

__attribute__((nonnull(1, 3)))
void avec_popd(aligned_vector *this, size_t count, void (*destruct)(void *));

__attribute__((nonnull(1, 3)))
void avec_popf(aligned_vector *this, size_t count, void (*onrealloc)(void *));

__attribute__((nonnull(1, 3, 4)))
void avec_popdf(aligned_vector *this, size_t count, void (*destruct)(void *), void (*onrealloc)(void *));

__attribute__((always_inline, nonnull(1)))
inline void avec_free(aligned_vector *this) {
	if (this->data.ptr != null) {
		aptr_free(&this->data);
		this->len = 0;
	}
}

__attribute__((always_inline, nonnull(1)))
inline void avec_clear(aligned_vector *this, void (*destruct)(void *)) {
	if (this->data.ptr != null) {
		for (size_t i = 0; i < this->len; i++) {
			destruct((void *) ((size_t) this->data.ptr + i * this->bsize));
		}
		aptr_free(&this->data);
		this->len = 0;
	}
}