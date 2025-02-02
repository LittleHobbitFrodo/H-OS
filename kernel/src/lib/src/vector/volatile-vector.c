//
//	memory/heap/page-vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../vector/volatile-vector.h"

#include "../../utils.h"

/*void vvec_resize(volatile_vector* this, vec_len_t len) {
	//	will not unlock the vector
	vvec_wait_and_lock(this);
	if (this->data == null) {
		this->len = len;
		this->data = alloc(len * this->bsize);
		return;
	}
	if (this->len == len) {
		return;
	}
	this->data = realloc((void*)this->data, len * this->bsize);
	this->len = len;
}

void vvec_resizecd(volatile_vector* this, vec_len_t len, void (*construct)(void*), void (*destruct)(void*)) {
	//	will not unlock the vector
	vvec_wait_and_lock(this);
	if (this->data == null) {
		this->len = len;
		this->data = alloc(len * this->bsize);
		if (construct != null) {
			for (size_t i = 0; i < len; i++) {
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
			void* data = (void*)this->data;
			for (size_t i = this->len - 1; i >= len; i++) {
				destruct((void*)((size_t)data + (i * this->bsize)));
			}
		}
		this->data = realloc((void*)this->data, len * this->bsize);
	} else {
		this->data = realloc((void*)this->data, len * this->bsize);
		if (construct != null) {
			void* data = (void*)this->data;
			for (size_t i = 0; i < len; i++) {
				construct((void*)((size_t)data + (i * this->bsize)));
			}
		}
	}
	this->len = len;
}

void* vvec_push(volatile_vector* this, vec_len_t count) {
	//	will not lock the vector
	vvec_wait_and_lock(this);
	if (this->data == null) {
		this->len = count;
		this->data = alloc((this->len * this->bsize) + VVEC_ALLOC_ADD);
		return this->data;
	}
	size_t olen = this->len;
	this->len += count;
	this->data = realloca(this->data, this->len * this->bsize, VVEC_REALLOC_ADD);
	return (void*)((size_t)this->data + (olen * this->bsize));
}

void* __vvec_push_locked(volatile_vector* this, vec_len_t count) {
	//	will not lock the vector
	if (this->data == null) {
		this->len = count;
		this->data = alloc((this->len * this->bsize) + VVEC_ALLOC_ADD);
		return this->data;
	}
	size_t olen = this->len;
	this->len += count;
	this->data = realloca(this->data, this->len * this->bsize, VVEC_REALLOC_ADD);
	return (void*)((size_t)this->data + (olen * this->bsize));
}

void* vvec_pushc(volatile_vector* this, vec_len_t count, void (*construct)(void*)) {
	vvec_wait_and_lock(this);
	if (this->data == null) {
		this->len = count;
		this->data = alloc((this->len * this->bsize) + VVEC_ALLOC_ADD);
		for (size_t i = 0; i < this->len; i++) {
			construct((void*)((size_t)this->data + (i * this->bsize)));
		}
		return this->data;
	}
	size_t olen = this->len;
	this->len += count;
	this->data = realloca(this->data, this->len * this->bsize, VVEC_REALLOC_ADD);
	for (size_t i = olen; i < this->len; i++) {
		construct((void*)((size_t)this->data + (i * this->bsize)));
	}
	return (void*)((size_t)this->data + (olen * this->bsize));
}

void vvec_pop(volatile_vector* this, vec_len_t count) {
	vvec_wait_and_lock(this);
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
	vvec_unlock(this);
}

void vvec_popd(volatile_vector* this, vec_len_t count, void (*destruct)(void*)) {
	vvec_wait_and_lock(this);
	if (this->data != null) {
		if (this->len <= count) {
			for (size_t i = 0; i < count; i++) {
				destruct((void*)((size_t)this->data + (i * this->bsize)));
			}
			free(this->data);
			this->len = 0;
			this->data = null;
		} else {
			for (size_t i = this->len - count; i < this->len; i++) {
				destruct((void*)((size_t)this->data + (i * this->bsize)));
			}
			this->len -= count;
			this->data = realloc(this->data, this->len * this->bsize);
		}
	}
	vvec_unlock(this);
}

void vvec_take_over(volatile_vector* this, volatile_vector* other) {
	vvec_wait_and_lock(this);
	vvec_wait_and_lock(other);
	this->data = other->data;
	this->len = other->len;
	this->bsize = other->bsize;

	other->data = 0;
	other->len = 0;
	other->bsize = 0;
	vvec_unlock(this);
	vvec_unlock(other);
}

void* vvec_insert(volatile_vector* this, vec_len_t index, vec_len_t count) {
	//	will not unlock the vector
	vvec_wait_and_lock(this);
	if (this->data != 0) {
		vvec_push(this, count);
		*//*for (ssize_t i = this->len - 1; (i+1 > (ssize_t)index) && (i > 0); i--) {
			memcpy(vvec_at(this, i-1), vvec_at(this, i), this->bsize);
		}*//*
		page_heap_segment_t* segs = &((page_heap_segment_t*)this->data)[index];
		memcpy_reverse(segs, (void*)((size_t)segs + (count * this->bsize)), (this->len - index) * this->bsize);
		return vvec_at_unlock(this, index);
	}
	return null;
}

void* __vvec_insert_locked(volatile_vector* this, vec_len_t index, vec_len_t count) {
	//	will not unlock the vector
	if (this->data != 0) {
		vec_push((multipurpose_vector*)this, count);
		void* source = (void*)((size_t)this->data + (index * this->bsize));
		for (size_t i = this->len - 1; i > index+1; i--) {
			print("\ttransferring seg["); printu(i-1); print("] to seg["); printu(i); printl("]");
			memcpy(source + ((i-1) * this->bsize),  (void*)((size_t)source + (i * this->bsize)), this->bsize);
		}
		//page_heap_segment_t* segs = &((page_heap_segment_t*)this->data)[index];
		//memcpy_reverse(segs, (void*)((size_t)segs + (count * this->bsize)), (this->len - index) * this->bsize);
		return vvec_at(this, index+1);
	}
	return null;
}

*/