//
//	memory/heap/page-vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../integers.h"
#include "../memory/heap/multipurpose/heap.h"
#include "./vector.h"
//#include <stdatomic.h>
#include "../atomic.h"

#define VVEC_REALLOC_ADD 8
#define VVEC_ALLOC_ADD 4

#define vvec_wait_and_lock(instance_ptr) while (__atomic_test_and_set(&instance_ptr->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define vvec_wait_lock(instance_ptr) while (__atomic_load_n(&instance_ptr->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define vvec_lock(instance_ptr) __atomic_test_and_set(&instance_ptr->lock, __ATOMIC_ACQUIRE)
#define vvec_unlock(instance_ptr) __atomic_clear(&instance_ptr->lock, __ATOMIC_RELEASE)

#define volatile_vector_type(name, type, type_name, construct, destruct)\
typedef struct type_name {\
	type* data;\
	size_t capacity;\
	u32 len;\
	volatile u8 lock;\
	allocator_t* alloc;\
} type_name;\
__attribute__((always_inline, nonnull(1))) static inline void name##_construct(type_name* self, size_t len, allocator_t* alloc) { \
	self->alloc = alloc;\
	if (len == 0) {\
		self->data = null;\
		self->len = 0;\
		self->capacity = 0;\
	} else {\
		self->len = len;\
		self->data = self->alloc->alloc(self->alloc, (len * sizeof(type)) + (sizeof(type) * VVEC_ALLOC_ADD));\
		for (size_t i = 0; i < len; i++) {\
			construct(&self->data[i]);\
		}\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
	}\
}\
__attribute__((always_inline, nonnull(1))) static inline void name##_destruct(type_name* self) {\
	if (self->data != 0) {\
		for (size_t i = 0; i < self->len; i++) {\
			destruct(&self->data[i]);\
		}\
		self->alloc->free(self->data);\
		self->data = null;\
		self->len = 0;\
		self->capacity = 0;\
		self->lock = false;\
	}\
}\
__attribute__((nonnull(1))) void name##_resize(type_name* self, size_t len) {\
	vvec_wait_and_lock(self);\
	if (self->data == null) {\
		self->data = self->alloc->alloc(self->alloc, len * sizeof(type));\
		for (size_t i = 0; i < len; i++) {\
			construct(&self->data[i]);\
		}\
		self->len = len;\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
		vvec_unlock(self);\
		return;\
	} else if (self->len == len) {\
		vvec_unlock(self);\
		return;\
	}\
	if (self->len < len) {\
		self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
		for (size_t i = self->len; i < len; i++) {\
			construct(&self->data[i]);\
		}\
	} else {\
		for (size_t i = self->len-1; i >= len; i--) {\
			destruct(&self->data[i]);\
		}\
		self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
	}\
	self->len = len;\
	vvec_unlock(self);\
}\
__attribute__((nonnull(1))) type* name##_push(type_name* self, size_t count) {                               \
	vvec_wait_and_lock(self);\
	if (self->data == null) {                                              \
		self->data = self->alloc->alloc(self->alloc, (count * sizeof(type)) + (sizeof(type) * VVEC_ALLOC_ADD));\
		self->len = count;\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
		for (size_t i = 0; i < count; i++) {\
			construct(&self->data[i]);\
		}\
		return self->data;\
	}                                                                      \
	;size_t olen = self->len;\
	self->len += count;\
	if (self->len <= self->capacity) {\
		for (size_t i = olen; i < self->len; i++) {\
			construct(&self->data[i]);\
		}\
		return &self->data[self->len - count];\
	}\
	self->data = self->alloc->realloca(self->alloc, self->data, self->len * sizeof(type), VVEC_REALLOC_ADD * sizeof(type));\
	self->capacity = heap_bsize(self->data) / sizeof(type);\
	for (size_t i = olen; i < self->len; i++) {\
		construct(&self->data[i]);\
	}\
	return &self->data[olen];\
}\
__attribute__((nonnull(1))) void name##_pop(type_name* self, size_t count) {\
	if (self->data == null) {\
		return;\
	}\
	vvec_wait_and_lock(self);\
	if (self->len <= count) {\
		for (size_t i = 0; i < self->len; i++) {\
			destruct(&self->data[i]);\
		}\
		self->alloc->free(self->data);\
		self->data = null;\
		self->len = (self->capacity = 0);\
		vvec_unlock(self);\
		return;\
	}\
	for (size_t i = self->len - count; i < self->len; i++) {\
		destruct(&self->data[i]);\
	}\
	self->len -= count;\
	self->data = self->alloc->realloc(self->alloc, self->data, self->len * sizeof(type));\
	vvec_unlock(self);\
}\
__attribute__((nonnull(1))) type* name##_insert(type_name* self, u32 index, u32 count) {\
	vvec_wait_and_lock(self);\
	if (self->data == null) {\
		self->data = self->alloc->alloc(self->alloc, (count * sizeof(type)) + (sizeof(type) * VVEC_ALLOC_ADD));\
		self->len = count;\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
		for (size_t i = 0; i < count; i++) {\
			construct(&self->data[i]);\
		}\
		return self->data;\
	}\
	if (self->len + count > self->capacity) {\
		self->data = self->alloc->realloca(self->alloc, self->data, self->len * sizeof(type), VVEC_REALLOC_ADD * sizeof(type));\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
	}\
	self->len += count;\
	for (ssize_t i = self->len; (i+1 > (ssize_t)index) && (i > 0); i--) {\
        self->data[i] = self->data[i-1];\
    }\
    construct(&self->data[index]);\
	return &self->data[index];\
}

/*typedef struct volatile_vector {
	//	implemented in page heap

	void* data;
	vec_len_t len;
	vec_bsize_t bsize;
	volatile bool lock;

	//	if the lock is engaged (and it is planned to be changed)
		//	use classic vector functions (vec_push, ...)
		//	packed attribute is important

} __attribute__((packed)) volatile_vector;

__attribute__((always_inline, nonnull(1)))
inline void vvec(volatile_vector* self) {
	self->data = null;
	self->len = 0;
	self->lock = false;
	self->bsize = 0;
}

__attribute__((always_inline, nonnull(1)))
inline void vvecs(volatile_vector* self, vec_bsize_t bsize) {
	self->data = null;
	self->len = 0;
	self->lock = false;
	self->bsize = bsize;
}

__attribute__((always_inline, nonnull(1)))
static inline void* vvecsa(volatile_vector* self, vec_len_t len, vec_bsize_t bsize) {
	//	will leave the vector locked
	vvec_lock(self);
	self->len = len;
	self->bsize = bsize;
	self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
	return self->data;
}

__attribute__((always_inline, nonnull(1, 4)))
static inline void vvecsac(volatile_vector* self, vec_len_t len, vec_bsize_t bsize, void (*construct)(void*)) {
	//	will unlock the vector
	vvec_lock(self);
	self->len = len;
	self->bsize = bsize;
	self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
	for (size_t i = 0; i < self->len; i++) {
		construct((void*)((size_t)self->data + i * self->bsize));
	}
	vvec_unlock(self);
}

__attribute__((always_inline, nonnull))
inline void* vvec_last(volatile_vector* self) {
	return (void*)((size_t)self->data + ((self->len - 1) * self->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_at(volatile_vector* self, vec_len_t at) {
	return (void*)((size_t)self->data + (at * self->bsize));
}

__attribute__((always_inline, nonnull))
inline void* vvec_last_lock(volatile_vector* self) {
	vvec_wait_and_lock(self);
	return (void*)((size_t)self->data + ((self->len - 1) * self->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_at_lock(volatile_vector* self, vec_len_t at) {
	vvec_wait_and_lock(self);
	return (void*)((size_t)self->data + (at * self->bsize));
}

__attribute__((always_inline, nonnull))
inline void* vvec_last_unlock(volatile_vector* self) {
	vvec_unlock(self);
	return (void*)((size_t)self->data + ((self->len - 1) * self->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_at_unlock(volatile_vector* self, vec_len_t at) {
	vvec_unlock(self);
	return (void*)((size_t)self->data + (at * self->bsize));
}

__attribute__((always_inline, nonnull(1)))
inline void* vvec_last_wait(volatile_vector* self) {
	vvec_wait_and_lock(self);
	vvec_unlock(self);
	return (void*)(((size_t)self->data + ((self->len - 1) * self->bsize)));
}

__attribute__((always_inline, nonnull(1)))
static inline void vvec_free(volatile_vector* self) {
	vvec_wait_and_lock(self);
	if (self->data != null) {
		free((void*)self->data);
		self->data = null;
	}
	self->len = 0;
	vvec_unlock(self);
}

__attribute__((nonnull(1)))
void vvec_resize(volatile_vector* self, vec_len_t len);
__attribute__((nonnull(1)))
void vvec_resizecd(volatile_vector* self, vec_len_t len, void (*construct)(void*), void (*destruct)(void*));
//	will not unlock the vector


__attribute__((nonnull(1), returns_nonnull))
void* vvec_push(volatile_vector* self, vec_len_t count);
__attribute__((nonnull(1), returns_nonnull))
void* __vvec_push_locked(volatile_vector* self, vec_len_t count);
__attribute__((nonnull(1), returns_nonnull))
void* vvec_pushc(volatile_vector* self, vec_len_t count, void (*construct)(void*));

__attribute__((nonnull(1)))
void vvec_pop(volatile_vector* self, vec_len_t count);
__attribute__((nonnull(1)))
void vvec_popd(volatile_vector* self, vec_len_t count, void (*destruct)(void*));

__attribute__((nonnull(1)))
void vvec_clear(volatile_vector* self, void (*destruct)(void*));

__attribute__((nonnull(1, 2)))
void vvec_take_over(volatile_vector* self, volatile_vector* other);

__attribute__((nonnull(1)))
void* vvec_insert(volatile_vector* self, vec_len_t index, vec_len_t count);
__attribute__((nonnull(1)))
void* __vvec_insert_locked(volatile_vector* self, vec_len_t index, vec_len_t count);
*/
