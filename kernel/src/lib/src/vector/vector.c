//
//	vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../vector/vector.h"

void vec_resize(multipurpose_vector *self, u32 len) {
	//	destruction of each popped element must be done manually
	if (self->data == null) {
		self->len = len;
		self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
		return;
	} else if (self->len == len) {
		return;
	}
	self->data = self->alloc->realloc(self->alloc, self->data, len * self->bsize);
	self->len = len;
}

void vec_resizecd(multipurpose_vector *self, u32 len, void (*construct)(void *), void (*destruct)(void *)) {
	if (self->data == null) {
		self->len = len;
		self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
		if (construct != null) {
			for (size_t i = 0; i < self->len; i++) {
				construct((void*)((size_t)self->data + (i * self->bsize)));
			}
		}
		return;
	}

	if (self->len == len) {
		return;
	}

	if (self->len < len) {
		if (destruct != null) {
			//	destruct elements
			for (size_t i = self->len - 1; i >= len; i--) {
				destruct((void *) ((size_t) self->data + (i * self->bsize)));
			}
		}
		self->data = self->alloc->realloc(self->alloc, self->data, len * self->bsize);
	} else {
		self->data = self->alloc->realloc(self->alloc, self->data, len * self->bsize);
		if (construct != null) {
			//	construct elements
			for (size_t i = self->len; i < len; i++) {
				construct((void *) ((size_t) self->data + (i * self->bsize)));
			}
		}
	}
	self->len = len;
}

void *vec_push(multipurpose_vector *self, u32 count) {
	if (self->data == null) {
		self->len = count;
		self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
		return self->data;
	}
	size_t olen = self->len;
	self->len += count;
	self->data = self->alloc->realloca(self->alloc, self->data, self->len * self->bsize, VECTOR_REALLOC_ADD);
	return (void *) ((size_t) self->data + (self->bsize * olen));
}

void *vec_pushc(multipurpose_vector *self, u32 count, void (*construct)(void *)) {
	if (self->data == null) {
		self->len = count;
		self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
		for (size_t i = 0; i < self->len; i++) {
			construct((void *) ((size_t) self->data + (i * self->bsize)));
		}
		return self->data;
	}
	size_t olen = self->len;
	self->len += count;
	self->data = self->alloc->realloca(self->alloc, self->data, self->len * self->bsize, VECTOR_REALLOC_ADD);
	for (size_t i = olen; i < self->len; i++) {
		construct((void *) ((size_t) self->data + (i * self->bsize)));
	}
	return (void *) ((size_t) self->data + (self->bsize * olen));
}

void vec_pop(multipurpose_vector *self, u32 count) {
	if (self->data != null) {
		if (self->len <= count) {
			self->alloc->free(self->alloc, self->data);
			self->data = null;
			self->len = 0;
		} else {
			self->len -= count;
			self->data = self->alloc->realloc(self->alloc, self->data, self->len * self->bsize);
		}
	}
}

void vec_popd(multipurpose_vector *self, u32 count, void (*destruct)(void *)) {
	if (self->data != null) {
		if (self->len <= count) {
			for (size_t i = 0; i < self->len; i++) {
				destruct((void *) ((size_t) self->data + (i * self->bsize)));
			}
			self->alloc->free(self->alloc, self->data);
			self->data = null;
			self->len = 0;
		} else {
			for (size_t i = self->len - count; i < self->len; i++) {
				destruct((void *) ((size_t) self->data + (i * self->bsize)));
			}
			self->len -= count;
			self->data = self->alloc->realloc(self->alloc, self->data, self->len * self->bsize);
		}
	}
}

void vec_take_over(multipurpose_vector* self, multipurpose_vector* other) {
	self->data = other->data;
	self->bsize = other->bsize;
	self->len = other->len;

	other->data = null;
	other->bsize = 0;
	other->len = 0;
}


void* vec_insert(multipurpose_vector* self, u32 index, u32 count) {
	//	returns pointer to the inserted entry
	if (self->data != null) {
		vec_push(self, count);
		for (ssize_t i = self->len - 1; (i+1 > (ssize_t)index) && (i > 0); i--) {
			memcpy(vec_at(self, i-1), vec_at(self, i), self->bsize);
		}
		return vec_at(self, ++index);
	}
	return null;
}
