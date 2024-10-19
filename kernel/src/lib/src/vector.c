//
//	vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../vector.h"

void vec_resize(vector *this, size_t len) {
	//	destruction of each popped element must be done manually
	if (this->data == null) {
		this->len = len;
		this->data = alloc(this->len * this->bsize);
		return;
	} else if (this->len == len) {
		return;
	}
	if (this->len < len) {
		this->data = realloc(this->data, len * this->bsize);
	} else {
		this->data = realloc(this->data, len * this->bsize);
	}
	this->len = len;
}

void vec_resizecd(vector *this, size_t len, void (*construct)(void *), void (*destruct)(void *)) {
	if (this->data == null) {
		this->len = len;
		this->data = alloc(this->len * this->bsize);
	} else if (this->len == len) {
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

void *vec_push(vector *this, size_t count) {
	if (this->data == null) {
		this->len = count;
		this->data = alloc(this->len * this->bsize);
		return this->data;
	}
	size_t olen = this->len;
	this->len += count;
	this->data = prealloca(this->data, this->len * this->bsize, vector_realloc_add);
	return (void *) ((size_t) this->data + (this->bsize * olen));
}

void *vec_pushc(vector *this, size_t count, void (*construct)(void *)) {
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
	this->data = prealloca(this->data, this->len * this->bsize, vector_realloc_add);
	for (size_t i = olen; i < this->len; i++) {
		construct((void *) ((size_t) this->data + (i * this->bsize)));
	}
	return (void *) ((size_t) this->data + (this->bsize * olen));
}

void vec_pop(vector *this, size_t count) {
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

void vec_popd(vector *this, size_t count, void (*destruct)(void *)) {
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

void vec_take_over(vector* this, vector* other) {
	this->data = other->data;
	this->bsize = other->bsize;
	this->len = other->len;

	other->data = null;
	other->bsize = 0;
	other->len = 0;
}


void avec_resize(aligned_vector *this, size_t len) {
	if (this->data.ptr == null) {
		this->len = len;
		aptr_alloc(&this->data, this->len * this->bsize);
		return;
	} else if (this->len == len) {
		return;
	}
	if (this->len < len) {
		aptr_realloc(&this->data, len * this->bsize);
	} else {
		aptr_realloc(&this->data, len * this->bsize);
	}
	this->len = len;
}

void avec_resizef(aligned_vector *this, size_t len, void (*onrealloc)(void *)) {
	if (this->data.ptr == null) {
		this->len = len;
		aptr_alloc(&this->data, this->len * this->bsize);
		return;
	} else if (this->len == len) {
		return;
	}
	if (this->len < len) {
		aptr_reallocf(&this->data, len * this->bsize, onrealloc);
	} else {
		aptr_reallocf(&this->data, len * this->bsize, onrealloc);
	}
	this->len = len;
}

void avec_resizecd(aligned_vector *this, size_t len, void (*construct)(void *), void (*destruct)(void *)) {
	if (this->data.ptr == null) {
		this->len = len;
		aptr_alloc(&this->data, this->len * this->bsize);
		if (construct != null) {
			for (size_t i = 0; i < len; i++) {
				construct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
		}
	} else if (this->len == len) {
		return;
	}
	if (this->len < len) {
		aptr_realloc(&this->data, len * this->bsize);
		if (construct != null) {
			for (size_t i = this->len; i < len; i++) {
				construct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
		}
	} else {
		if (destruct != null) {
			for (size_t i = this->len; i < len; i++) {
				destruct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
		}
		aptr_realloc(&this->data, len * this->bsize);
	}
	this->len = len;
}

void avec_resizecdf(aligned_vector *this, size_t len, void (*construct)(void *), void (*destruct)(void *), void (*onrealloc)(void *)) {
	if (this->data.ptr == null) {
		this->len = len;
		aptr_alloc(&this->data, this->len * this->bsize);
		if (construct != null) {
			for (size_t i = 0; i < len; i++) {
				construct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
		}
	} else if (this->len == len) {
		return;
	}
	if (this->len < len) {
		aptr_reallocf(&this->data, len * this->bsize, onrealloc);
		if (construct != null) {
			for (size_t i = this->len; i < len; i++) {
				construct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
		}
	} else {
		if (destruct != null) {
			for (size_t i = this->len; i < len; i++) {
				destruct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
		}
		aptr_reallocf(&this->data, len * this->bsize, onrealloc);
	}
	this->len = len;
}

void *avec_push(aligned_vector *this, size_t count) {
	if (this->data.ptr == null) {
		this->len = count;
		aptr_alloc(&this->data, this->len * this->bsize);
		return this->data.ptr;
	}
	size_t olen = this->len;
	this->len += count;
	aptr_realloca(&this->data, this->len * this->bsize, vector_realloc_add);
	return (void *) ((size_t) this->data.ptr + (olen * this->bsize));
}

void *avec_pushc(aligned_vector *this, size_t count, void (*construct)(void *)) {
	if (this->data.ptr == null) {
		this->len = count;
		aptr_alloc(&this->data, this->len * this->bsize);
		for (size_t i = 0; i < this->len; i++) {
			construct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
		}
		return this->data.ptr;
	}
	size_t olen = this->len;
	this->len += count;
	aptr_realloca(&this->data, this->len * this->bsize, vector_realloc_add);
	return (void *) ((size_t) this->data.ptr + (olen * this->bsize));
}

void *avec_pushf(aligned_vector *this, size_t count, void (*onrealloc)(void *)) {
	if (this->data.ptr == null) {
		this->len = count;
		aptr_alloc(&this->data, this->len * this->bsize);
		return this->data.ptr;
	}
	size_t olen = this->len;
	this->len += count;
	aptr_reallocaf(&this->data, this->len * this->bsize, vector_realloc_add, onrealloc);
	return (void *) ((size_t) this->data.ptr + (olen * this->bsize));
}

void *avec_pushcf(aligned_vector *this, size_t count, void (*construct)(void *), void (*onrealloc)(void *)) {
	if (this->data.ptr == null) {
		this->len = count;
		aptr_alloc(&this->data, this->len * this->bsize);
		for (size_t i = 0; i < this->len; i++) {
			construct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
		}
		return this->data.ptr;
	}
	size_t olen = this->len;
	this->len += count;
	aptr_reallocf(&this->data, this->len * this->bsize, onrealloc);
	for (size_t i = olen; i < this->len; i++) {
		construct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
	}
	return (void *) ((size_t) this->data.ptr + (olen * this->bsize));
}

void avec_pop(aligned_vector *this, size_t count) {
	if (this->data.ptr != null) {
		if (this->len <= count) {
			aptr_free(&this->data);
			this->len = 0;
		} else {
			this->len -= count;
			aptr_realloc(&this->data, this->len * this->bsize);
		}
	}
}

void avec_popd(aligned_vector *this, size_t count, void (*destruct)(void *)) {
	if (this->data.ptr != null) {
		if (this->len <= count) {
			for (size_t i = 0; i < this->len; i++) {
				destruct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
			aptr_free(&this->data);
			this->len = 0;
		} else {
			size_t olen = this->len;
			this->len -= count;
			for (size_t i = this->len; i < olen; i++) {
				destruct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
			aptr_realloc(&this->data, this->len * this->bsize);
		}
	}
}

void avec_popf(aligned_vector *this, size_t count, void (*onrealloc)(void *)) {
	if (this->data.ptr != null) {
		if (this->len <= count) {
			aptr_free(&this->data);
			this->len = 0;
		} else {
			this->len -= count;
			aptr_reallocf(&this->data, this->len * this->bsize, onrealloc);
		}
	}
}

void avec_popdf(aligned_vector *this, size_t count, void (*destruct)(void *), void (*onrealloc)(void *)) {
	if (this->data.ptr != null) {
		if (this->len <= count) {
			for (size_t i = 0; i < this->len; i++) {
				destruct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
			aptr_free(&this->data);
			this->len = 0;
		} else {
			size_t olen = this->len;
			this->len -= count;
			for (size_t i = this->len; i < olen; i++) {
				destruct((void *) ((size_t) this->data.ptr + (i * this->bsize)));
			}
			aptr_reallocf(&this->data, this->len * this->bsize, onrealloc);
		}
	}
}
