//
//	vector.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../integers.h"
#include "../memory/heap/heap.h"

#define VECTOR_REALLOC_ADD 4
#define VECTOR_MAX_CAPACITY_DIFFERENCE 6

//	NOTE: all vector types are defined with packed attribute
	//	that ensures use of custom vectors in void_vector functions

//	vector type with no automatic constructing and destructing of elements
#define vector_type(name, type, type_name)\
	typedef struct type_name {\
		type* data;\
		size_t len;\
		size_t capacity;\
	} __attribute__((packed)) type_name;\
	__attribute__((always_inline, nonnull(1))) /*[[maybe_unused]]*/ static inline void name##_construct(type_name* this, size_t len) {\
		if (len == 0) {\
			this->data = null;\
			this->len = 0;\
			this->capacity = 0;\
			return;\
		}\
		this->len = len;\
		this->data = alloc(len * sizeof(type));\
		this->capacity = heap_bsize(this->data) / sizeof(type);\
	}\
	__attribute__((always_inline, nonnull(1))) /*[[maybe_unused]]*/ static inline void name##_destruct(type_name* this) {\
		if (this->data != 0) {\
			free(this->data);\
		}\
		this->data = null;\
		this->len = 0;\
		this->capacity = 0;\
	}\
	__attribute__((nonnull(1))) void name##_resize(type_name* this, size_t len) {\
		if (this->data == null) {\
			this->data = alloc(len * sizeof(type));\
			this->len = len;\
			this->capacity = heap_bsize(this->data) / sizeof(type);\
		} else if (this->len == len) {\
			return;\
		}\
		this->len = len;\
		this->data = realloc(this->data, len * sizeof(type));\
		this->capacity = heap_bsize(this->data) / sizeof(type);\
	}\
	__attribute__((nonnull(1))) type* name##_push(type_name* this, size_t count) {\
		if (this->data == null) {\
			this->data = alloc(count * sizeof(type));\
			this->len = count;\
			this->capacity = heap_bsize(this->data) / sizeof(type);\
			return this->data;\
		}\
		this->len += count;\
		if (this->len <= this->capacity) {\
			return &this->data[this->len - count];\
		}\
		this->data = realloca(this->data, this->len * sizeof(type), VECTOR_REALLOC_ADD);\
		this->capacity = heap_bsize(this->data) / sizeof(type);\
		return &this->data[this->len - count];\
	}\
	__attribute__((nonnull(1))) void name##_pop(type_name* this, size_t count) {\
		if (this->data != null) {\
			if (this->len <= count) {\
				this->len = 0;\
				free(this->data);\
				this->data = null;\
				this->capacity = 0;\
			} else {\
				this->len -= count;\
				this->data = realloc(this->data, this->len * sizeof(type));\
				this->capacity = heap_bsize(this->data) / sizeof(type);\
			}\
		}\
	}\
	__attribute__((always_inline, nonnull(1))) /*[[maybe_unused]]*/ static inline type* name##_at(type_name* this, size_t at) {\
		return &this->data[at];\
	}\
	__attribute__((always_inline, nonnull(1))) /*[[maybe_unused]]*/ static inline type* name##_last(type_name* this) {\
		return &this->data[this->len-1];\
	}\
	__attribute__((always_inline, nonnull(1))) /*[[maybe_unused]]*/ static inline bool name##_empty(type_name* this) {\
		return this->data == null;\
	}\

#define vector_instance(name, type, type_name)\
	typedef struct type_name {\
		type* data;\
		size_t len;\
		size_t capacity;\
	} __attribute__((packed)) type_name;\
	static type_name name;\
	__attribute__((always_inline)) static inline void name##_construct(size_t len) {\
		if (len == 0) {\
			name.data = null;\
			name.len = 0;\
			return;\
		}\
		name.len = len;\
		name.data = alloc(len * sizeof(type));\
		name.capacity = heap_bsize(name.data) / sizeof(type);\
	}\
	__attribute__((always_inline)) static inline void name##_destruct() {\
		if (name.data != 0) {\
			free(name.data);\
		}\
		name.data = null;\
		name.len = 0;\
		name.capacity = 0;\
	}\
	void name##_resize(size_t len) {\
		if (name.data == null) {\
			name.data = alloc(len * sizeof(type));\
			name.len = len;\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
		} else if (name.len == len) {\
			return;\
		}\
		name.len = len;\
		name.data = realloc(name.data, len * sizeof(type));\
		name.capacity = heap_bsize(name.data) / sizeof(type);\
	}\
	__attribute__((returns_nonnull)) type* name##_push(size_t count) {\
		if (name.data == null) {\
			name.data = alloc(count * sizeof(type));\
			name.len = count;\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
			return name.data;\
		}\
		name.len += count;\
		if (name.len <= name.capacity) {\
			return &name.data[name.len - count];\
		}\
		name.data = realloca(name.data, name.len * sizeof(type), VECTOR_REALLOC_ADD);\
		name.capacity = heap_bsize(name.data) / sizeof(type);\
		return &name.data[name.len - count];\
	}\
	void name##_pop(size_t count) {\
		if (name.data != null) {\
			if (name.len <= count) {\
				free(name.data);\
				name.data = null;\
				name.len = 0;\
				name.capacity = 0;\
			} else {\
				name.len -= count;\
				name.data = realloc(name.data, name.len * sizeof(type));\
				name.capacity = heap_bsize(name.data) / sizeof(type);\
			}\
		}\
	}\
	__attribute__((always_inline, returns_nonnull)) static inline type* name##_at(size_t at) {\
		return &name.data[at];\
	}\
	__attribute__((always_inline, returns_nonnull)) static inline type* name##_last() {\
		return &name.data[name.len-1];\
	}\
	__attribute__((always_inline)) static inline bool name##_empty() {\
		return name.data == null;\
	}\


#define vector_type_cd(name, type, type_name, construct, destruct)\
	typedef struct type_name {\
		type* data;\
		size_t len;\
		size_t capacity;\
	} __attribute__((packed)) type_name;\
	__attribute__((always_inline, nonnull(1))) static inline void name##_construct(type_name* this, size_t len) {\
		if (len == 0) {\
			this->data = null;\
			this->len = 0;\
			this->capacity = 0;\
			return;\
		}\
		this->len = len;\
		this->data = alloc(len * sizeof(type));\
		for (size_t i = 0; i < len; i++) {\
			construct(&this->data[i]);\
		}\
		this->capacity = heap_bsize(this->data) / sizeof(type);\
	}\
	__attribute__((always_inline, nonnull(1))) static inline void name##_destruct(type_name* this) {\
		if (this->data != 0) {\
			for (size_t i = 0; i < this->len; i++) {\
				destruct(&this->data[i]);\
			}\
			free(this->data);\
		}\
		this->data = null;\
		this->len = 0;\
		this->capacity = 0;\
	}\
	__attribute__((nonnull(1))) void name##_resize(type_name* this, size_t len) {\
		if (this->data == null) {\
			this->data = alloc(len * sizeof(type));\
			for (size_t i = 0; i < len; i++) {\
				construct(&this->data[i]);\
			}\
			this->len = len;\
			this->capacity = heap_bsize(this->data) / sizeof(type);\
			return;\
		} else if (this->len == len) {\
			return;\
		}\
		if (this->len < len) {\
			for (size_t i = this->len-1; i >= len; i--) {\
				destruct(&this->data[i]);\
			}\
			this->data = realloc(this->data, len * sizeof(type));\
			this->capacity = heap_bsize(this->data) / sizeof(type);\
		} else {\
			this->data = realloc(this->data, len * sizeof(type));\
			for (size_t i = this->len; i < len; i++) {\
				construct(&this->data[i]);\
			}\
		}\
		this->len = len;\
 	}\
	__attribute__((nonnull(1))) type* name##_push(type_name* this, size_t count) {\
		if (this->data == null) {\
			this->data = alloc(count * sizeof(type));\
			this->len = count;\
			this->capacity = heap_bsize(this->data) / sizeof(type);\
			for (size_t i = 0; i < count; i++) {\
				construct(&this->data[i]);\
			}\
			return this->data;\
		}\
		size_t olen = this->len;\
		this->len += count;\
		if (this->len <= this->capacity) {\
			return &this->data[this->len - count];\
		}\
		this->data = realloca(this->data, this->len * sizeof(type), VECTOR_REALLOC_ADD);\
		this->capacity = heap_bsize(this->data) / sizeof(type);\
		for (size_t i = olen; i < this->len; i++) {\
			construct(&this->data[i]);\
		}\
		return &this->data[this->len - count];\
	}\
	__attribute__((nonnull(1))) void name##_pop(type_name* this, size_t count) {\
		if (this->data == null) {\
			return;\
		}\
		if (this->len <= count) {\
			free(this->data);\
			this->data = null;\
			this->len = 0;\
			return;\
		}\
		this->len -= count;\
		if (count < VECTOR_MAX_CAPACITY_DIFFERENCE) {\
			return;\
		} else {\
			this->data = realloc(this->data, this->len * sizeof(type));\
			this->capacity = heap_bsize(this->data) / sizeof(type);\
		}\
	}\
	__attribute__((always_inline, nonnull(1))) static inline type* name##_at(type_name* this, size_t at) {\
		return &this->data[at];\
	}\
	__attribute__((always_inline, nonnull(1))) static inline type* name##_last(type_name* this) {\
		return &this->data[this->len-1];\
	}\
	__attribute__((always_inline, nonnull(1))) static inline bool name##_empty(type_name* this) {\
		return this->data == null;\
	}\


#define vector_instance_cd(name, type, type_name, construct, destruct)\
	typedef struct type_name {\
		type* data;\
		size_t len;\
		size_t capacity;\
	} __attribute__((packed)) type_name;\
	static type_name name;\
	__attribute__((always_inline)) static inline void name##_construct(size_t len) {\
		if (len == 0) {\
			name.data = null;\
			name.len = 0;\
			return;\
		}\
		name.len = len;\
		name.data = alloc(len * sizeof(type));\
		name.capacity = heap_bsize(name.data) / sizeof(type);\
		for (size_t i = 0; i < name.len; i++) {\
			construct(&name.data[i]);\
		}\
	}\
	__attribute__((always_inline)) static inline void name##_destruct() {\
		if (name.data != 0) {\
			for (size_t i = 0; i < name.len; i++) {\
				destruct(&name.data[i]);\
			}\
			free(name.data);\
		}\
		name.data = null;\
		name.len = 0;\
		name.capacity = 0;\
	}\
	void name##_resize(size_t len) {\
		if (name.data == null) {\
			name.data = alloc(len * sizeof(type));\
			name.len = len;\
			for (size_t i = 0; i < len; i++) {\
				construct(&name.data[i]);\
			}\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
		} else if (name.len == len) {\
			return;\
		}\
		if (name.len < len) {\
			for (size_t i = name.len-1; i >= len; i--) {\
				destruct(&name.data[i]);\
			}\
			name.data = realloc(name.data, len * sizeof(type));\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
		} else {\
			name.data = realloc(name.data, len * sizeof(type));\
			for (size_t i = name.len; i < len; i++) {\
				construct(&name.data[i]);\
			}\
		}\
		name.len = len;\
	}\
	__attribute__((returns_nonnull)) type* name##_push(size_t count) {\
		if (name.data == null) {\
			name.data = alloc(count * sizeof(type));\
			name.len = count;\
			name.capacity = heap_bsize(name.data) / sizeof(type);\
			return name.data;\
		}\
		size_t olen = name.len;\
		name.len += count;\
		if (name.len <= name.capacity) {\
			return &name.data[name.len - count];\
		}\
		name.data = realloca(name.data, name.len * sizeof(type), VECTOR_REALLOC_ADD);\
		for (size_t i = olen; i < name.len; i++) {\
			construct(&name.data[i]);\
		}\
		name.capacity = heap_bsize(name.data) / sizeof(type);\
		return &name.data[name.len - count];\
	}\
	void name##_pop(size_t count) {\
		if (name.data != null) {\
			if (name.len <= count) {\
				for (size_t i = 0; i < name.len; i++) {\
					destruct(&name.data[i]);\
				}\
				free(name.data);\
				name.data = null;\
				name.len = 0;\
				name.capacity = 0;\
			} else {\
				size_t olen = name.len;\
				name.len -= count;\
				for (size_t i = olen - 1; i >= name.len; i--) {\
					destruct(&name.data[i]);\
				}\
				name.data = realloc(name.data, name.len * sizeof(type));\
				name.capacity = heap_bsize(name.data) / sizeof(type);\
			}\
		}\
	}\
	__attribute__((always_inline, returns_nonnull)) static inline type* name##_at(size_t at) {\
		return &name.data[at];\
	}\
	__attribute__((always_inline, returns_nonnull)) static inline type* name##_last() {\
		return &name.data[name.len-1];\
	}\
	__attribute__((always_inline)) static inline bool name##_empty() {\
		return name.data == null;\
	}\


typedef u32 vec_bsize_t;
typedef size_t vec_len_t;
typedef size_t vec_size_t;

typedef struct multipurpose_vector {
	//NOTE:		destruction of each popped/freed elements must be done manually

	void* data;
	vec_len_t len;
	vec_bsize_t bsize; //	size of each element

	//	packed attribute is important (volatile_vector)
} __attribute__((packed)) multipurpose_vector;


__attribute__((always_inline, nonnull(1)))
inline void vec(multipurpose_vector *this) {
	//	initializes empty vector
	this->data = null;
	this->len = 0;
	this->bsize = 0;
}

__attribute__((always_inline, nonnull(1)))
inline void vecs(multipurpose_vector *this, vec_len_t bsize) {
	//	initializes empty vector with byte size
	this->len = 0;
	this->bsize = bsize;
	this->data = null;
}

__attribute__((always_inline, nonnull(1)))
static inline void *vecsa(multipurpose_vector *this, vec_len_t len, vec_bsize_t bsize) {
	//	initializes vector and allocates memory for it
	this->len = len;
	this->bsize = bsize;
	if (this->len > 0) {
		this->data = alloc(this->len * this->bsize);
	} else {
		this->data = null;
	}
	return this->data;
}

__attribute__((always_inline, nonnull(1, 4)))
static inline void vecsac(multipurpose_vector *this, vec_len_t len, vec_bsize_t bsize, void (*construct)(void *)) {
	//	initializes vector, allocates memory and constructs each element
	this->len = len;
	this->bsize = bsize;
	this->data = alloc(this->len * this->bsize);
	for (size_t i = 0; i < this->len; i++) {
		construct((void *) ((size_t) this->data + (i * this->bsize)));
	}
}

__attribute__((nonnull(1)))
void vec_resize(multipurpose_vector *this, vec_len_t len);

//	resize vector
__attribute__((nonnull(1)))
void vec_resizecd(multipurpose_vector *this, vec_len_t len, void (*construct)(void *), void (*destruct)(void *));
	//	resize vector, constructs pushed elements or destructs popped elements

__attribute__((always_inline, nonnull(1)))
inline void *vec_last(multipurpose_vector *this) {
	//	returns pointer to last element
	return (void *) ((size_t) this->data + (this->bsize * (this->len - 1)));
}

__attribute__((always_inline))
inline void *vec_at(multipurpose_vector *this, vec_len_t at) {
	//	returns target element
	return (void *) (((size_t) this->data + (at * this->bsize)));
}

__attribute__((nonnull(1), returns_nonnull))
void *vec_push(multipurpose_vector *this, vec_len_t count);

//	pushes to vector and returns pointer to new elements
__attribute__((returns_nonnull))
void *vec_pushc(multipurpose_vector *this, vec_len_t count, void (*construct)(void *));

//	pushes to vector, constructs new elements and returns pointer to them

__attribute__((nonnull(1)))
void vec_pop(multipurpose_vector *this, vec_len_t count);

//	pops vector
__attribute__((nonnull(1, 3)))
void vec_popd(multipurpose_vector *this, vec_len_t count, void (*destruct)(void *));

//	pops vector and destructs popped elements

__attribute__((always_inline, nonnull(1)))
inline void vec_free(multipurpose_vector *this) {
	//	frees vector data (destruction of each object must be done manually)
	if (this->data != null) {
		free(this->data);
	}
	this->data = null;
	this->len = (this->bsize = 0);
}

__attribute__((always_inline, nonnull(1, 2)))
inline void vec_clear(multipurpose_vector *this, void (*destruct)(void *)) {
	//	frees vector data and destruct each element
	if (this->data != null) {
		for (size_t i = 0; i < this->len; i++) {
			destruct((void *) ((size_t) this->data + (i * this->bsize)));
		}
		free(this->data);
	}
}

__attribute__((nonnull(1, 2)))
void vec_take_over(multipurpose_vector* this, multipurpose_vector* other);
	//	will not delete free the "this" vector

__attribute__((nonnull(1)))
void* vec_insert(multipurpose_vector* this, vec_len_t index, vec_len_t count);


vector_instance(memmap, memmap_entry, memmap_vector);