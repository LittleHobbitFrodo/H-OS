//
//	vector.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_VECTOR_H
	#define H_OS_LIB_VECTOR_H

	static size_t vector_realloc_add = 4;

	typedef struct vector {

		//NOTE:		destruction of each popped/freed entry must be done manually

		void* data;
		size_t len;
		size_t bsize;		//	size of each element
	} vector;

	__attribute__((always_inline)) inline void vec(vector* this) {
		this->data = null;
		this->len = 0;
		this->bsize = 0;
	}

	__attribute__((always_inline)) inline void vecs(vector* this, size_t bsize) {
		this->len = 0;
		this->bsize = bsize;
		this->data = null;
	}

	__attribute__((always_inline)) inline void* vecsa(vector* this, size_t len, size_t bsize) {
		this->len = len;
		this->bsize = bsize;
		if (this->len > 0) {
			this->data = alloc(this->len * this->bsize);
		} else {
			this->data = null;
		}
		return this->data;
	}

	void vec_resize(vector* this, size_t len);

	__attribute__((always_inline)) inline void* vec_last(vector* this) {
		void* r = null;
		if (this->data != null) {
			r = (void*)((size_t)this->data + (this->bsize * (this->len - 1)));
		}
		return r;
	}

	__attribute__((always_inline)) inline void* vec_at(vector* this, size_t at) {
		void* r = null;
		if (this->data != null) {
			r = (void*)(((size_t)this->data + (at * this->bsize)));
		}
		return r;
	}

	[[nodiscard]] void* vec_push(vector* this, size_t count);

	void vec_pop(vector* this, size_t count);

	__attribute__((always_inline)) inline void vec_free(vector* this) {
		//	destruction of each object must be done manually
		if (this->data != null) {
			free(this->data);
		}
		this->data = null;
		this->len = (this->bsize = 0);
	}


	/*typedef struct aligned_vector {
		aligned_ptr ptr;
		size_t size;
	} aligned_vector;*/




	static vector memmap;



#endif
//	#warning vector.h already included
//#endif