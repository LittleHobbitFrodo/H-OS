//
//	memory/heap/page-vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../integers.h"
#include "../memory/heap/heap.h"
#include <stdatomic.h>

#define VVEC_REALLOC_ADD 8
#define VVEC_ALLOC_ADD 4

typedef struct volatile_vector {
	//	implemented in page heap

	void* data;
	size_t len;
	volatile bool lock;
	u32 bsize;

} volatile_vector;

#define vvec_wait_and_lock(instance) while (__atomic_test_and_set(&instance->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define vvec_wait_lock(instance) while (__atomic_load_n(&instance->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define vvec_lock(instance) __atomic_test_and_set(&instance->lock, __ATOMIC_ACQUIRE)
#define vvec_unlock(instance) __atomic_clear(&instance->lock, __ATOMIC_RELEASE)

__attribute__((always_inline, nonnull(1)))
inline void vvec(volatile_vector* this) {
	this->data = null;
	this->len = 0;
	this->lock = false;
	this->bsize = 0;
}

__attribute__((always_inline, nonnull(1)))
inline void vvecs(volatile_vector* this, u32 bsize) {
	this->data = null;
	this->len = 0;
	this->lock = false;
	this->bsize = bsize;
}

__attribute__((always_inline, nonnull(1), returns_nonnull))
static inline void* vvecsa(volatile_vector* this, size_t len, u32 bsize) {
	//	will leave the vector locked
	vvec_lock(this);
	this->len = len;
	this->bsize = bsize;
	this->data = alloc(this->len * this->bsize);
	return this->data;
}

__attribute__((always_inline, nonnull(1, 4)))
static inline void vvecsac(volatile_vector* this, size_t len, u32 bsize, void (*construct)(void*)) {
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

__attribute__((nonnull(1), returns_nonnull))
void* vvec_last_locked(volatile_vector* this);

__attribute__((nonnull(1), returns_nonnull))
void* vvec_at_locked(volatile_vector* this, size_t at);

__attribute__((always_inline, nonnull(1), returns_nonnull))
inline void* vvec_last_unlocked(volatile_vector* this) {
	return (void*)((size_t)this->data + ((this->len - 1) * this->bsize));
}

__attribute__((always_inline, nonnull(1), returns_nonnull))
inline void* vvec_at_unlocked(volatile_vector* this, size_t at) {
	return (void*)((size_t)this->data + (at * this->bsize));
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
void vvec_resize(volatile_vector* this, size_t len);

__attribute__((nonnull(1)))
void vvec_resizecd(volatile_vector* this, size_t len, void (*construct)(void*), void (*destruct)(void*));


__attribute__((nonnull(1), returns_nonnull))
void* vvec_push(volatile_vector* this, size_t count);

__attribute__((nonnull(1), returns_nonnull))
void* vvec_pushc(volatile_vector* this, size_t count, void (*construct)(void*));

__attribute__((nonnull(1)))
void vvec_pop(volatile_vector* this, size_t count);

__attribute__((nonnull(1)))
void vvec_popd(volatile_vector* this, size_t count, void (*destruct)(void*));

__attribute__((nonnull(1)))
void vvec_clear(volatile_vector* this, void (*destruct)(void*));

__attribute__((nonnull(1, 2)))
void vvec_take_over(volatile_vector* this, volatile_vector* other);

__attribute__((nonnull(1)))
void* vvec_insert(volatile_vector* this, size_t index, size_t count);

