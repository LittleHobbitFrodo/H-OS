//
//	memory/heap/page-vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../integers.h"
#include "../memory/heap/heap.h"
#include "./vector.h"
#include <stdatomic.h>

#define VVEC_REALLOC_ADD 8
#define VVEC_ALLOC_ADD 4

typedef struct volatile_vector {
	//	implemented in page heap

	void* data;
	vec_len_t len;
	vec_bsize_t bsize;
	volatile bool lock;

	//	if the lock is engaged (and it is planned to be changed)
		//	use classic vector functions (vec_push, ...)
		//	packed attribute is important

} __attribute__((packed)) volatile_vector;

#define vvec_wait_and_lock(instance_ptr) while (__atomic_test_and_set(&instance_ptr->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define vvec_wait_lock(instance_ptr) while (__atomic_load_n(&instance_ptr->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define vvec_lock(instance_ptr) __atomic_test_and_set(&instance_ptr->lock, __ATOMIC_ACQUIRE)
#define vvec_unlock(instance_ptr) __atomic_clear(&instance_ptr->lock, __ATOMIC_RELEASE)

__attribute__((always_inline, nonnull(1)))
inline void vvec(volatile_vector* this) {
	this->data = null;
	this->len = 0;
	this->lock = false;
	this->bsize = 0;
}

__attribute__((always_inline, nonnull(1)))
inline void vvecs(volatile_vector* this, vec_bsize_t bsize) {
	this->data = null;
	this->len = 0;
	this->lock = false;
	this->bsize = bsize;
}

__attribute__((always_inline, nonnull(1)))
static inline void* vvecsa(volatile_vector* this, vec_len_t len, vec_bsize_t bsize) {
	//	will leave the vector locked
	vvec_lock(this);
	this->len = len;
	this->bsize = bsize;
	this->data = alloc(this->len * this->bsize);
	return this->data;
}

__attribute__((always_inline, nonnull(1, 4)))
static inline void vvecsac(volatile_vector* this, vec_len_t len, vec_bsize_t bsize, void (*construct)(void*)) {
	//	will unlock the vector
	vvec_lock(this);
	this->len = len;
	this->bsize = bsize;
	this->data = alloc(this->len * this->bsize);
	for (size_t i = 0; i < this->len; i++) {
		construct((void*)((size_t)this->data + i * this->bsize));
	}
	vvec_unlock(this);
}

__attribute__((always_inline, nonnull))
inline void* vvec_last(volatile_vector* this) {
	return (void*)((size_t)this->data + ((this->len - 1) * this->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_at(volatile_vector* this, vec_len_t at) {
	return (void*)((size_t)this->data + (at * this->bsize));
}

__attribute__((always_inline, nonnull))
inline void* vvec_last_lock(volatile_vector* this) {
	vvec_wait_and_lock(this);
	return (void*)((size_t)this->data + ((this->len - 1) * this->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_at_lock(volatile_vector* this, vec_len_t at) {
	vvec_wait_and_lock(this);
	return (void*)((size_t)this->data + (at * this->bsize));
}

__attribute__((always_inline, nonnull))
inline void* vvec_last_unlock(volatile_vector* this) {
	vvec_unlock(this);
	return (void*)((size_t)this->data + ((this->len - 1) * this->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_at_unlock(volatile_vector* this, vec_len_t at) {
	vvec_unlock(this);
	return (void*)((size_t)this->data + (at * this->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_last_wait(volatile_vector* this) {
	vvec_wait_and_lock(this);
	vvec_unlock(this);
	return (void*)(((size_t)this->data + ((this->len - 1) * this->bsize)));
}

__attribute__((always_inline, nonnull(1)))
static inline void vvec_free(volatile_vector* this) {
	vvec_wait_and_lock(this);
	if (this->data != null) {
		free((void*)this->data);
		this->data = null;
	}
	this->len = 0;
	vvec_unlock(this);
}

__attribute__((nonnull(1)))
void vvec_resize(volatile_vector* this, vec_len_t len);
__attribute__((nonnull(1)))
void vvec_resizecd(volatile_vector* this, vec_len_t len, void (*construct)(void*), void (*destruct)(void*));
//	will not unlock the vector


__attribute__((nonnull(1), returns_nonnull))
void* vvec_push(volatile_vector* this, vec_len_t count);
__attribute__((nonnull(1), returns_nonnull))
void* __vvec_push_locked(volatile_vector* this, vec_len_t count);
__attribute__((nonnull(1), returns_nonnull))
void* vvec_pushc(volatile_vector* this, vec_len_t count, void (*construct)(void*));

__attribute__((nonnull(1)))
void vvec_pop(volatile_vector* this, vec_len_t count);
__attribute__((nonnull(1)))
void vvec_popd(volatile_vector* this, vec_len_t count, void (*destruct)(void*));

__attribute__((nonnull(1)))
void vvec_clear(volatile_vector* this, void (*destruct)(void*));

__attribute__((nonnull(1, 2)))
void vvec_take_over(volatile_vector* this, volatile_vector* other);

__attribute__((nonnull(1)))
void* vvec_insert(volatile_vector* this, vec_len_t index, vec_len_t count);
__attribute__((nonnull(1)))
void* __vvec_insert_locked(volatile_vector* this, vec_len_t index, vec_len_t count);

