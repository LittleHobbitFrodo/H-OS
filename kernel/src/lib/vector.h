//
//	vector.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_VECTOR_H
	#define H_OS_LIB_VECTOR_H

	static size_t vector_realloc_add = 4;

	typedef struct vector {

		//NOTE:		destruction of each popped/freed elements must be done manually

		void* data;
		size_t len;
		size_t bsize;		//	size of each element
	} vector;

	__attribute__((always_inline, nonnull(1)))
	inline void vec(vector* this) {
		//	initializes empty vector
		this->data = null;
		this->len = 0;
		this->bsize = 0;
	}

	__attribute__((always_inline, nonnull(1)))
	inline void vecs(vector* this, size_t bsize) {
		//	initializes empty vector with byte size
		this->len = 0;
		this->bsize = bsize;
		this->data = null;
	}

	__attribute__((always_inline, nonnull(1)))
	inline void* vecsa(vector* this, size_t len, size_t bsize) {
		//	initializes vector and allocates memory for it
		this->len = len;
		this->bsize = bsize;
		if (this->len > 0) {
			this->data = palloc(this->len * this->bsize);
		} else {
			this->data = null;
		}
		return this->data;
	}

	__attribute__((nonnull(1, 4)))
	inline void vecsac(vector* this, size_t len, size_t bsize, void (*construct)(void*)) {
		//	initializes vector, allocates memory and constructs each element
		this->len = len;
		this->bsize = bsize;
		if (this->len > 0) {
			this->data = palloc(this->len * this->bsize);
			for (size_t i = 0; i < this->len; i++) {
				construct((void*)((size_t)this->data + (i * this->bsize)));
			}
		} else {
			this->data = null;
		}
	}

	__attribute__((nonnull(1))) void vec_resize(vector* this, size_t len);
		//	resize vector
	__attribute__((nonnull(1)))
	void vec_resizecd(vector* this, size_t len, void (*construct)(void*), void (*destruct)(void*));
		//	resize vector, constructs pushed elements or destructs popped elements

	__attribute__((always_inline, nonnull(1)))
	inline void* vec_last(vector* this) {
		//	returns pointer to last element (or NULL)
		void* r = null;
		if (this->data != null) {
			r = (void*)((size_t)this->data + (this->bsize * (this->len - 1)));
		}
		return r;
	}

	__attribute__((always_inline)) inline void* vec_at(vector* this, size_t at) {
		//	returns target element (or NULL)
		void* r = null;
		if (this->data != null) {
			r = (void*)(((size_t)this->data + (at * this->bsize)));
		}
		return r;
	}

	__attribute__((nonnull(1), returns_nonnull))
	void* vec_push(vector* this, size_t count);
		//	pushes to vector and returns pointer to new elements
	__attribute__((returns_nonnull))
	void* vec_pushc(vector* this, size_t count, void (*construct)(void*));
		//	pushes to vector, constructs new elements and returns pointer to them

	__attribute__((nonnull(1))) void vec_pop(vector* this, size_t count);
		//	pops vector
	__attribute__((nonnull(1, 3)))
	void vec_popd(vector* this, size_t count, void (*destruct)(void*));
		//	pops vector and destructs popped elements

	__attribute__((always_inline, nonnull(1))) inline void vec_free(vector* this) {
		//	frees vector data (destruction of each object must be done manually)
		if (this->data != null) {
			free(this->data);
		}
		this->data = null;
		this->len = (this->bsize = 0);
	}

	__attribute__((always_inline, nonnull(1, 2)))
	inline void vec_clear(vector* this, void (*destruct)(void*)) {
		//	frees vector data and destruct each element
		if (this->data != null) {
			for (size_t i = 0; i < this->len; i++) {
				destruct((void*)((size_t)this->data + (i * this->bsize)));
			}
			free(this->data);
		}
	}






	typedef struct aligned_vector {

		//NOTE:		destruction of each popped/freed element must be done manually

		aligned_ptr data;
		size_t bsize;
		size_t len;
	} aligned_vector;

	__attribute__((always_inline, nonnull(1))) inline void avec(aligned_vector* this) {
		//	initializes empty avector
		aptr(&this->data);
		this->len = 0;
		this->bsize = 0;
	}

	__attribute__((always_inline, nonnull(1))) inline void avecsa(aligned_vector* this, size_t len, size_t bsize, size_t align) {
		//	initializes vector with allocation
		this->len = len;
		this->bsize = bsize;
		aptrse(&this->data, this->len * this->bsize, align);
	}

	__attribute__((nonnull(1))) void avec_resize(aligned_vector* this, size_t len);
		//	resize vector
	__attribute__((nonnull(1, 3)))
	void avec_resizef(aligned_vector* this, size_t len, void (*onrealloc)(void*));
		//	resize vector and call function if data is reallocated
	__attribute__((nonnull(1)))
	void avec_resizecd(aligned_vector* this, size_t len, void (*construct)(void*), void (*destruct)(void*));
		//	resize vector and calls function to destruct all popped elements
	__attribute__((nonnull(1)))
	void avec_resizecdf(aligned_vector* this, size_t len, void (*construct)(void*), void (*destruct)(void*), void (*onrealloc)(void*));
		//	resize vector, destruct popped elements, call function on reallocation

	__attribute__((always_inline, nonnull(1)))
	inline void* avec_last(const aligned_vector* this) {
		void* r = null;
		if (this->data.ptr != null) {
			r = (void*)((size_t)this->data.ptr + (this->bsize * (this->len - 1)));
		}
		return r;
	}

	__attribute__((always_inline))
	inline void* avec_at(aligned_vector* this, size_t i) {
		void* r = null;
		if (this->data.ptr != null) {
			r = (void*)((size_t)this->data.ptr + (i * this->bsize));
		}
		return r;
	}

	__attribute__((nonnull(1), returns_nonnull))
	void* avec_push(aligned_vector* this, size_t count);

	__attribute__((nonnull(1, 3), returns_nonnull))
	void* avec_pushc(aligned_vector* this, size_t count, void (*construct)(void*));

	__attribute__((nonnull(1, 3), returns_nonnull))
	void* avec_pushf(aligned_vector* this, size_t count, void (*onrealloc)(void*));

	__attribute__((nonnull(1, 3, 4), returns_nonnull))
	void* avec_pushcf(aligned_vector* this, size_t count, void (*construct)(void*), void (*onrealloc)(void*));


	__attribute__((nonnull(1)))
	void avec_pop(aligned_vector* this, size_t count);
	__attribute__((nonnull(1, 3)))
	void avec_popd(aligned_vector* this, size_t count, void (*destruct)(void*));
	__attribute__((nonnull(1, 3)))
	void avec_popf(aligned_vector* this, size_t count, void (*onrealloc)(void*));
	__attribute__((nonnull(1, 3, 4)))
	void avec_popdf(aligned_vector* this, size_t count, void (*destruct)(void*), void (*onrealloc)(void*));

	__attribute__((always_inline, nonnull(1))) inline void avec_free(aligned_vector* this) {
		if (this->data.ptr != null) {
			aptr_free(&this->data);
			this->len = 0;
		}
	}

	__attribute__((always_inline, nonnull(1))) inline void avec_clear(aligned_vector* this, void (*destruct)(void*)) {
		if (this->data.ptr != null) {
			for (size_t i = 0; i < this->len; i++) {
				destruct((void*)((size_t)this->data.ptr + i * this->bsize));
			}
			aptr_free(&this->data);
			this->len = 0;
		}
	}



	static vector memmap;



#endif
//	#warning vector.h already included
//#endif