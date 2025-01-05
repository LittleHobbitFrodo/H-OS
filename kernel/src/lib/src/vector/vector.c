//
//	vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../vector/vector.h"

void vec_resize(multipurpose_vector *this, vec_len_t len) {
	//	destruction of each popped element must be done manually
	if (this->data == null) {
		this->len = len;
		this->data = alloc(this->len * this->bsize);
		return;
	} else if (this->len == len) {
		return;
	}
	this->data = realloc(this->data, len * this->bsize);
	this->len = len;
}

void vec_resizecd(multipurpose_vector *this, vec_len_t len, void (*construct)(void *), void (*destruct)(void *)) {
	if (this->data == null) {
		this->len = len;
		this->data = alloc(this->len * this->bsize);
		if (construct != null) {
			for (size_t i = 0; i < this->len; i++) {
				construct((void*)((size_t)this->data + (i * this->bsize)));
			}
		}
		return;
	}

	if (this->len == len) {
		return;
	}

	if (this->len < len) {
		if (destruct != null) {
			//	destruct elements
			for (size_t i = this->len - 1; i >= len; i--) {
				destruct((void *) ((size_t) this->data + (i * this->bsize)));
			}
		}
		this->data = realloc(this->data, len * this->bsize);
	} else {
		this->data = realloc(this->data, len * this->bsize);
		if (construct != null) {
			//	construct elements
			for (size_t i = this->len; i < len; i++) {
				construct((void *) ((size_t) this->data + (i * this->bsize)));
			}
		}
	}
	this->len = len;
}

void *vec_push(multipurpose_vector *this, vec_len_t count) {
	if (this->data == null) {
		this->len = count;
		this->data = alloc(this->len * this->bsize);
		return this->data;
	}
	size_t olen = this->len;
	this->len += count;
	this->data = realloca(this->data, this->len * this->bsize, VECTOR_REALLOC_ADD);
	return (void *) ((size_t) this->data + (this->bsize * olen));
}

void *vec_pushc(multipurpose_vector *this, vec_len_t count, void (*construct)(void *)) {
	if (this->data == null) {
		this->len = count;
		this->data = alloc(this->len * this->bsize);
		for (size_t i = 0; i < this->len; i++) {
			construct((void *) ((size_t) this->data + (i * this->bsize)));
		}
		return this->data;
	}
	size_t olen = this->len;
	this->len += count;
	this->data = realloca(this->data, this->len * this->bsize, VECTOR_REALLOC_ADD);
	for (size_t i = olen; i < this->len; i++) {
		construct((void *) ((size_t) this->data + (i * this->bsize)));
	}
	return (void *) ((size_t) this->data + (this->bsize * olen));
}

void vec_pop(multipurpose_vector *this, vec_len_t count) {
	if (this->data != null) {
		if (this->len <= count) {
			free(this->data);
			this->data = null;
			this->len = 0;
		} else {
			this->len -= count;
			this->data = realloc(this->data, this->len * this->bsize);
		}
	}
}

void vec_popd(multipurpose_vector *this, vec_len_t count, void (*destruct)(void *)) {
	if (this->data != null) {
		if (this->len <= count) {
			for (size_t i = 0; i < this->len; i++) {
				destruct((void *) ((size_t) this->data + (i * this->bsize)));
			}
			free(this->data);
			this->data = null;
			this->len = 0;
		} else {
			for (size_t i = this->len - count; i < this->len; i++) {
				destruct((void *) ((size_t) this->data + (i * this->bsize)));
			}
			this->len -= count;
			this->data = realloc(this->data, this->len * this->bsize);
		}
	}
}

void vec_take_over(multipurpose_vector* this, multipurpose_vector* other) {
	this->data = other->data;
	this->bsize = other->bsize;
	this->len = other->len;

	other->data = null;
	other->bsize = 0;
	other->len = 0;
}


void* vec_insert(multipurpose_vector* this, vec_len_t index, vec_len_t count) {
	//	returns pointer to the inserted entry
	if (this->data != null) {
		vec_push(this, count);
		for (ssize_t i = this->len - 1; (i+1 > (ssize_t)index) && (i > 0); i--) {
			memcpy(vec_at(this, i-1), vec_at(this, i), this->bsize);
		}
		return vec_at(this, ++index);
	}
	return null;
}
