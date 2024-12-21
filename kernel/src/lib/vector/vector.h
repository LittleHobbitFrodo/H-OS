//
//	vector.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../integers.h"
#include "../memory/heap/multipurpose/heap.h"
#include "../output.h"

#define VECTOR_REALLOC_ADD 4
#define VECTOR_MAX_CAPACITY_DIFFERENCE 6


#define readonly_vector(name, type, type_name, destruct)\
	typedef struct type_name {\
		type *data;\
		allocator_t *alloc;\
		u32 len;\
	} type_name;\
	type* name##_build(type_name* self, allocator_t* alloc, u32 count) {\
		if (count == 0) {\
			self->data = null;\
			self->alloc = alloc;\
			self->len = 0;\
			return null;\
		}\
		self->alloc = alloc;\
		self->data = self->alloc->alloc(self->alloc, count * sizeof(type));\
		memnull(self->data, count * sizeof(type));\
		self->len = count;\
		return self->data;\
	}\
	void name##_destruct(type_name* self) {\
		if (self->data != null) {\
			for (u32 i = 0; i < self->len; i++) {\
				destruct(&self->data[i]);\
			}\
			self->alloc->free(self->alloc, self->data);\
			self->data = null;\
			self->len = 0;\
		}\
	}\
	__attribute__((always_inline)) inline type* name##_at(type_name* self, u32 at) {\
		return &self->data[at];\
	}



//	NOTE:	all vector types are defined with packed attribute
	//	that ensures use of custom vectors in void_vector functions

//	NOTE:	allocators are instance-dependent

#define vector_type(name, type, type_name, construct, destruct)\
	typedef struct type_name {\
		type* data;\
		u32 len;\
		u32 capacity;\
		allocator_t* alloc;\
	} type_name;\
	__attribute__((always_inline)) static inline void name##_construct(type_name* self, u32 len, allocator_t* alloc) {\
		self->alloc = alloc;\
		if (len == 0) {\
			self->data = null;\
			self->len = 0;\
			self->capacity = 0;\
			return;\
		} else {\
            self->len = len;\
            self->data = self->alloc->alloc(self->alloc, len * sizeof(type));\
            for (size_t i = 0; i < len; i++) {\
				construct(&self->data[i]);\
            }\
        	self->capacity = heap_bsize(self->data) / sizeof(type);\
    	}\
	}\
	__attribute__((always_inline)) static inline void name##_destruct(type_name* self) {\
		if (self->data != 0) {\
            for (size_t i = 0; i < self->len; i++) {\
               	destruct(&self->data[i]);\
            }\
			self->alloc->free(self->alloc, self->data);\
		}\
		self->data = null;\
		self->len = 0;\
		self->capacity = 0;\
	}\
	__attribute__((nonnull(1))) void name##_resize(type_name* self, u32 len) {\
		if (self->data == null) {\
			self->data = self->alloc->alloc(self->alloc, len * sizeof(type));\
			for (size_t i = 0; i < len; i++) {\
				construct(&self->data[i]);\
            }\
			self->len = len;\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
			return;\
		} else if (self->len == len) {\
			return;\
		}\
		if (self->len < len) {\
			for (size_t i = self->len-1; i >= len; i--) {\
				destruct(&self->data[i]);\
			}\
			self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
		} else {\
			self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
			for (size_t i = self->len; i < len; i++) {\
				construct(&self->data[i]);\
            }\
		}\
		self->len = len;\
 	}\
	__attribute__((nonnull(1))) type* name##_push(type_name* self, u32 count) {\
		if (self->data == null) {\
			self->data = self->alloc->alloc(self->alloc, count * sizeof(type));\
			self->len = count;\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
			for (size_t i = 0; i < count; i++) {\
				construct(&self->data[i]);\
            }\
			return self->data;\
		}\
		size_t olen = self->len;\
		self->len += count;\
		if (self->len <= self->capacity) {\
			return &self->data[self->len - count];\
		}\
		self->data = self->alloc->realloca(self->alloc, self->data, self->len * sizeof(type), VECTOR_REALLOC_ADD);\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
        for (size_t i = olen; i < self->len; i++) {\
           	construct(&self->data[i]);\
        }\
		return &self->data[olen];\
	}\
	__attribute__((nonnull(1))) void name##_pop(type_name* self, u32 count) {\
		if (self->data == null) {\
			return;\
		}\
		if (self->len <= count) {\
			self->alloc->free(self->alloc, self->data);\
			self->data = null;\
			self->len = 0;\
			return;\
		}\
		self->len -= count;\
		if (count < VECTOR_MAX_CAPACITY_DIFFERENCE) {\
			return;\
		} else {\
			self->data = self->alloc->realloc(self->alloc, self->data, self->len * sizeof(type));\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
		}\
	}\
	__attribute__((always_inline)) static inline type* name##_at(type_name* self, u32 at) {\
		return &self->data[at];\
	}\
	__attribute__((always_inline)) static inline type* name##_last(type_name* self) {\
		return &self->data[self->len-1];\
	}\
	__attribute__((always_inline)) static inline bool name##_empty(type_name* self) {\
		return self->data == null;\
	}                                                                            \

#define vector_with_allocator_type(name, type, type_name, construct, destruct)\
	typedef struct type_name {\
		type* data;\
		u32 len;\
		u32 capacity;                                                                                                     \
		allocator_t* alloc;\
	} type_name;\
	__attribute__((always_inline)) static inline void name##_construct(type_name* self, u32 len, allocator_t* alloc) { \
        self->alloc = alloc;\
		if (len == 0) {\
			self->data = null;\
			self->len = 0;\
			self->capacity = 0;\
			return;\
		} else {\
            self->len = len;\
            self->data = self->alloc->alloc(self->alloc, len * sizeof(type));\
            for (size_t i = 0; i < len; i++) {\
				construct(&self->data[i], self->alloc);\
            }\
        	self->capacity = heap_bsize(self->data) / sizeof(type);\
    	}\
	}\
	__attribute__((always_inline)) static inline void name##_destruct(type_name* self) {\
		if (self->data != 0) {\
            for (size_t i = 0; i < self->len; i++) {\
               	destruct(&self->data[i]);\
            }\
			self->alloc->free(self->alloc, self->data);\
		}\
		self->data = null;\
		self->len = 0;\
		self->capacity = 0;\
	}\
	__attribute__((nonnull(1))) void name##_resize(type_name* self, u32 len) {\
		if (self->data == null) {\
			self->data = self->alloc->alloc(self->alloc, len * sizeof(type));\
			for (size_t i = 0; i < len; i++) {\
				construct(&self->data[i], self->alloc);\
            }\
			self->len = len;\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
			return;\
		} else if (self->len == len) {\
			return;\
		}\
		if (self->len < len) {\
			for (size_t i = self->len-1; i >= len; i--) {\
				destruct(&self->data[i]);\
			}\
			self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
		} else {\
			self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
			for (size_t i = self->len; i < len; i++) {\
				construct(&self->data[i], self->alloc);\
            }\
		}\
		self->len = len;\
 	}\
	__attribute__((nonnull(1))) type* name##_push(type_name* self, u32 count) {\
		if (self->data == null) {\
			self->data = self->alloc->alloc(self->alloc, count * sizeof(type));\
			self->len = count;\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
			for (size_t i = 0; i < count; i++) {\
				construct(&self->data[i], self->alloc);\
            }\
			return self->data;\
		}\
		size_t olen = self->len;\
		self->len += count;\
		if (self->len <= self->capacity) {\
			return &self->data[self->len - count];\
		}\
		self->data = self->alloc->realloca(self->alloc, self->data, self->len * sizeof(type), VECTOR_REALLOC_ADD);\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
        for (size_t i = olen; i < self->len; i++) {\
           	construct(&self->data[i], self->alloc);\
        }\
		return &self->data[olen];\
	}\
	__attribute__((nonnull(1))) void name##_pop(type_name* self, u32 count) {\
		if (self->data == null) {\
			return;\
		}\
		if (self->len <= count) {\
			self->alloc->free(self->alloc, self->data);\
			self->data = null;\
			self->len = 0;\
			return;\
		}\
		self->len -= count;\
		if (count < VECTOR_MAX_CAPACITY_DIFFERENCE) {\
			return;\
		} else {\
			self->data = self->alloc->realloc(self->alloc, self->data, self->len * sizeof(type));\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
		}\
	}\
	__attribute__((always_inline)) static inline type* name##_at(type_name* self, u32 at) {\
		return &self->data[at];\
	}\
	__attribute__((always_inline)) static inline type* name##_last(type_name* self) {\
		return &self->data[self->len-1];\
	}\
	__attribute__((always_inline)) static inline bool name##_empty(type_name* self) {\
        return self->data == null;\
    }\


#define vector_instance(name, type, type_name, construct, destruct)\
	typedef struct type_name {\
		type* data;\
		u32 len;\
		u32 capacity;\
		allocator_t* alloc;\
	} type_name;\
    static type_name name;\
	static inline void name##_construct(u32 len, allocator_t* alloc) {\
        name.alloc = alloc;\
		if (len == 0) {\
			name.data = null;\
			name.len = 0;\
			name.capacity = 0;\
			return;\
		} else {\
            name.len = len;\
            name.data = name.alloc->alloc(name.alloc, len * sizeof(type));\
            for (size_t i = 0; i < len; i++) {\
				construct(&name.data[i]);\
            }\
        	name.capacity = heap_bsize(name.data) / sizeof(type);\
    	}\
	}\
	__attribute__((always_inline)) static inline void name##_destruct() {\
		if (name.data != 0) {\
            for (size_t i = 0; i < name.len; i++) {\
               	destruct(&name.data[i]);\
            }\
			name.alloc->free(name.alloc, name.data);\
		}\
		name.data = null;\
		name.len = 0;\
		name.capacity = 0;\
	}\
	void name##_resize(u32 len) {\
		if (name.data == null) {\
			name.data = name.alloc->alloc(name.alloc, len * sizeof(type));\
			for (size_t i = 0; i < len; i++) {\
				construct(&name.data[i]);\
            }\
			name.len = len;\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
			return;\
		} else if (name.len == len) {\
			return;\
		}\
		if (name.len < len) {\
			for (size_t i = name.len-1; i >= len; i--) {\
				destruct(&name.data[i]);\
			}\
			name.data = name.alloc->realloc(name.alloc, name.data, len * sizeof(type));\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
		} else {\
			name.data = name.alloc->realloc(name.alloc, name.data, len * sizeof(type));\
			for (size_t i = name.len; i < len; i++) {\
				construct(&name.data[i]);\
            }\
		}\
		name.len = len;\
 	}\
	type* name##_push(u32 count) {\
		if (name.data == null) {\
			name.data = name.alloc->alloc(name.alloc, count * sizeof(type));\
			name.len = count;\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
			for (u32 i = 0; i < count; i++) {\
				construct(&name.data[i]);\
			}\
			return name.data;\
		}\
		size_t olen = name.len;\
		name.len += count;\
		if (name.len >= name.capacity) {\
			name.data = name.alloc->realloca(name.alloc, name.data, name.len * sizeof(type), VECTOR_REALLOC_ADD * sizeof(type));\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
		}\
		for (u32 i = olen; i < name.len; i++) {\
			construct(&name.data[i]);\
		}\
		return &name.data[olen];\
	}\
	void name##_pop(u32 count) {\
		if (name.data == null) {\
            return;\
        }\
		if (name.len <= count) {\
			for (u32 i = 0; i < name.len; i++) {\
				destruct(&name.data[i]);\
			}\
			name.alloc->free(name.alloc, name.data);\
			name.data = null;\
			name.len = 0;\
			name.capacity = 0;\
			return;\
		}\
		u32 olen = name.len;\
		name.len -= count;\
		for (ssize_t i = olen-1; i >= name.len; i--) {\
			destruct(&name.data[i]);\
		}\
		if ((heap_bsize(name.data) >= VECTOR_MAX_CAPACITY_DIFFERENCE) && (VECTOR_MAX_CAPACITY_DIFFERENCE * sizeof(type) >= 8)) {\
			_heap_divide(name.alloc, ((heap_block*)name.data) - 1, name.len * sizeof(type));\
		}\
		name.capacity = heap_bsize((heap_block*)name.data - 1);\
	}\
	__attribute__((always_inline)) static inline type* name##_at(u32 at) {\
		return &name.data[at];\
	}\
	__attribute__((always_inline)) static inline type* name##_last() {\
		return &name.data[name.len-1];\
	}\
	__attribute__((always_inline)) static inline bool name##_empty() {\
		return name.data == null;\
	}

#define vector_with_allocator_instance(name, type, type_name, construct, destruct)\
	typedef struct type_name {\
		type* data;\
		u32 len;\
		u32 capacity;\
		allocator_t* alloc;\
	} type_name;\
    static type_name name;\
	__attribute__((always_inline)) static inline void name##_construct(u32 len, allocator_t* alloc) {\
		this->alloc = alloc;\
		if (len == 0) {\
			name.data = null;\
			name.len = 0;\
			name.capacity = 0;\
			return;\
		} else {\
            name.len = len;\
            name.data = name.alloc->alloc(name.alloc, len * sizeof(type));\
            for (size_t i = 0; i < len; i++) {\
				construct(&name.data[i], name.alloc);\
            }\
        	name.capacity = heap_bsize(name.data) / sizeof(type);\
    	}\
	}\
	__attribute__((always_inline)) static inline void name##_destruct(type_name* self) {\
		if (name.data != 0) {\
            for (size_t i = 0; i < name.len; i++) {\
               	destruct(&name.data[i]);\
            }\
			name.alloc->free(name.alloc, name.data);\
		}\
		name.data = null;\
		name.len = 0;\
		name.capacity = 0;\
	}\
	void name##_resize(u32 len) {\
		if (name.data == null) {\
			name.data = name.alloc->alloc(name.alloc, len * sizeof(type));\
			for (size_t i = 0; i < len; i++) {\
				construct(&name.data[i], name.alloc);\
            }\
			name.len = len;\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
			return;\
		} else if (name.len == len) {\
			return;\
		}\
		if (name.len < len) {\
			for (size_t i = name.len-1; i >= len; i--) {\
				destruct(&name.data[i]);\
			}\
			name.data = name.alloc->realloc(name.alloc, name.data, len * sizeof(type));\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
		} else {\
			name.data = name.alloc->realloc(name.alloc, name.data, len * sizeof(type));\
			for (size_t i = name.len; i < len; i++) {\
				construct(&name.data[i], name.alloc);\
            }\
		}\
		name.len = len;\
 	}\
	type* name##_push(u32 count) {\
		if (name.data == null) {\
			name.data = name.alloc->alloc(name.alloc, count * sizeof(type));\
			name.len = count;\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
			for (size_t i = 0; i < count; i++) {\
				construct(&name.data[i], name.alloc);\
            }\
			return name.data;\
		}\
		size_t olen = name.len;\
		name.len += count;\
		if (name.len <= name.capacity) {\
			return &name.data[name.len - count];\
		}\
		name.data = name.alloc->realloca(name.alloc, name.data, name.len * sizeof(type), VECTOR_REALLOC_ADD);\
		name.capacity = heap_bsize(name.data) / sizeof(type);\
        for (size_t i = olen; i < name.len; i++) {\
           	construct(&name.data[i], name.alloc);\
        }\
		return &name.data[olen];\
	}\
	void name##_pop(u32 count) {\
		if (name.data == null) {\
			return;\
		}\
		if (name.len <= count) {\
			name.alloc->free(name.alloc, name.data);\
			name.data = null;\
			name.len = 0;\
			return;\
		}\
		name.len -= count;\
		if (count < VECTOR_MAX_CAPACITY_DIFFERENCE) {\
			return;\
		} else {\
			name.data = name.alloc->realloc(name.alloc, name.data, name.len * sizeof(type));\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
		}\
	}\
	__attribute__((always_inline)) static inline type* name##_at(u32 at) {\
		return &name.data[at];\
	}\
	__attribute__((always_inline)) static inline type* name##_last(type_name* self) {\
		return &name.data[name.len-1];\
	}\
	__attribute__((always_inline)) static inline bool name##_empty(type_name* self) {\
		return name.data == null;\
	}

#define vector_with_different_allocator_type(name, type, type_name, construct, destruct, instance_alloc_t)\
	typedef struct type_name {\
		type* data;\
		u32 len;\
		u32 capacity;\
		allocator_t* alloc;\
		instance_alloc_t* inst_alloc;\
	} type_name;\
	__attribute__((always_inline)) static inline void name##_construct(type_name* self, u32 len, allocator_t* alloc, instance_alloc_t* inst_alloc) { \
        self->alloc = alloc;\
		self->inst_alloc = inst_alloc;\
		if (len == 0) {\
			self->data = null;\
			self->len = 0;\
			self->capacity = 0;\
			return;\
		} else {\
            self->len = len;\
            self->data = self->alloc->alloc(self->alloc, len * sizeof(type));\
            for (size_t i = 0; i < len; i++) {\
				construct(&self->data[i], self->inst_alloc);\
            }\
        	self->capacity = heap_bsize(self->data) / sizeof(type);\
    	}\
	}\
	__attribute__((always_inline)) static inline void name##_destruct(type_name* self) {\
		if (self->data != 0) {\
            for (size_t i = 0; i < self->len; i++) {\
               	destruct(&self->data[i]);\
            }\
			self->alloc->free(self->alloc, self->data);\
		}\
		self->data = null;\
		self->len = 0;\
		self->capacity = 0;\
	}\
	__attribute__((nonnull(1))) void name##_resize(type_name* self, u32 len) {\
		if (self->data == null) {\
			self->data = self->alloc->alloc(self->alloc, len * sizeof(type));\
			for (size_t i = 0; i < len; i++) {\
				construct(&self->data[i], self->inst_alloc);\
            }\
			self->len = len;\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
			return;\
		} else if (self->len == len) {\
			return;\
		}\
		if (self->len < len) {\
			for (size_t i = self->len-1; i >= len; i--) {\
				destruct(&self->data[i]);\
			}\
			self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
		} else {\
			self->data = self->alloc->realloc(self->alloc, self->data, len * sizeof(type));\
			for (size_t i = self->len; i < len; i++) {\
				construct(&self->data[i], self->inst_alloc);\
            }\
		}\
		self->len = len;\
 	}\
	__attribute__((nonnull(1))) type* name##_push(type_name* self, u32 count) {\
		if (self->data == null) {\
			self->data = self->alloc->alloc(self->alloc, count * sizeof(type));\
			self->len = count;\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
			for (size_t i = 0; i < count; i++) {\
				construct(&self->data[i], self->inst_alloc);\
            }\
			return self->data;\
		}\
		size_t olen = self->len;\
		self->len += count;\
		if (self->len <= self->capacity) {\
			return &self->data[self->len - count];\
		}\
		self->data = self->alloc->realloca(self->alloc, self->data, self->len * sizeof(type), VECTOR_REALLOC_ADD);\
		self->capacity = heap_bsize(self->data) / sizeof(type);\
        for (size_t i = olen; i < self->len; i++) {\
           	construct(&self->data[i], self->inst_alloc);\
        }\
		return &self->data[olen];\
	}\
	__attribute__((nonnull(1))) void name##_pop(type_name* self, u32 count) {\
		if (self->data == null) {\
			return;\
		}\
		if (self->len <= count) {\
			self->alloc->free(self->alloc, self->data);\
			self->data = null;\
			self->len = 0;\
			return;\
		}\
		self->len -= count;\
		if (count < VECTOR_MAX_CAPACITY_DIFFERENCE) {\
			return;\
		} else {\
			self->data = self->alloc->realloc(self->alloc, self->data, self->len * sizeof(type));\
			self->capacity = heap_bsize(self->data) / sizeof(type);\
		}\
	}\
	__attribute__((always_inline)) static inline type* name##_at(type_name* self, u32 at) {\
		return &self->data[at];\
	}\
	__attribute__((always_inline)) static inline type* name##_last(type_name* self) {\
		return &self->data[self->len-1];\
	}\
	__attribute__((always_inline)) static inline bool name##_empty(type_name* self) {\
        return self->data == null;\
    }\

typedef struct multipurpose_vector {\
	void* data;
	u32 len;
	u32 capacity;
	allocator_t* alloc;
	u32 bsize;
} multipurpose_vector;

__attribute__((always_inline))
inline void vec(multipurpose_vector *self, allocator_t* alloc) {
	//	initializes empty vector
	self->alloc = alloc;
	self->data = null;
	self->len = 0;
	self->bsize = 0;
}

__attribute__((always_inline))
inline void vecs(multipurpose_vector *self, u32 bsize, allocator_t* alloc) {
	//	initializes empty vector with byte size
	self->len = 0;
	self->bsize = bsize;
	self->data = null;
	self->alloc = alloc;
}

__attribute__((always_inline))
static inline void *vecsa(multipurpose_vector *self, u32 len, u32 bsize, allocator_t* alloc) {
	//	initializes vector and allocates memory for it
	self->alloc = alloc;
	self->len = len;
	self->bsize = bsize;
	if (self->len > 0) {
		self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
	} else {
		self->data = null;
	}
	return self->data;
}

__attribute__((always_inline, nonnull(1, 4)))
static inline void vecsac(multipurpose_vector *self, u32 len, u32 bsize, void (*construct)(void *), allocator_t* alloc) {
	//	initializes vector, allocates memory and constructs each element
	self->alloc = alloc;
	self->len = len;
	self->bsize = bsize;
	self->data = self->alloc->alloc(self->alloc, self->len * self->bsize);
	for (size_t i = 0; i < self->len; i++) {
		construct((void *) ((size_t) self->data + (i * self->bsize)));
	}
}

__attribute__((nonnull(1)))
void vec_resize(multipurpose_vector *self, u32 len);

//	resize vector
__attribute__((nonnull(1)))
void vec_resizecd(multipurpose_vector *self, u32 len, void (*construct)(void *), void (*destruct)(void *));
//	resize vector, constructs pushed elements or destructs popped elements

__attribute__((always_inline))
inline void *vec_last(multipurpose_vector *self) {
	//	returns pointer to last element
	return (void *) ((size_t) self->data + (self->bsize * (self->len - 1)));
}

__attribute__((always_inline))
inline void *vec_at(multipurpose_vector *self, u32 at) {
	//	returns target element
	return (void *) (((size_t) self->data + (at * self->bsize)));
}

__attribute__((nonnull(1), returns_nonnull))
void *vec_push(multipurpose_vector *self, u32 count);

//	pushes to vector and returns pointer to new elements
__attribute__((returns_nonnull))
void *vec_pushc(multipurpose_vector *self, u32 count, void (*construct)(void *));

//	pushes to vector, constructs new elements and returns pointer to them

__attribute__((nonnull(1)))
void vec_pop(multipurpose_vector *self, u32 count);

//	pops vector
__attribute__((nonnull(1, 3)))
void vec_popd(multipurpose_vector *self, u32 count, void (*destruct)(void *));

//	pops vector and destructs popped elements

__attribute__((always_inline))
inline void vec_free(multipurpose_vector *self) {
	//	frees vector data (destruction of each object must be done manually)
	if (self->data != null) {
		self->alloc->free(self->alloc, self->data);
	}
	self->data = null;
	self->len = (self->bsize = 0);
}

__attribute__((always_inline, nonnull(1, 2)))
inline void vec_clear(multipurpose_vector *self, void (*destruct)(void *)) {
	//	frees vector data and destruct each element
	if (self->data != null) {
		for (size_t i = 0; i < self->len; i++) {
			destruct((void *) ((size_t) self->data + (i * self->bsize)));
		}
		self->alloc->free(self->alloc, self->data);
	}
}

__attribute__((nonnull(1, 2)))
void vec_take_over(multipurpose_vector* self, multipurpose_vector* other);
//	will not delete free the "self" vector

__attribute__((nonnull(1)))
void* vec_insert(multipurpose_vector* self, u32 index, u32 count);


//vector_instance(memmap, memmap_entry, memmap_vector, to_be_optimized, to_be_optimized);
readonly_vector(memmap, memmap_entry, memmap_vec, to_be_optimized);

static memmap_vec memmap = {0};