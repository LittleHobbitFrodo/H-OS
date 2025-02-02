//
//	string.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"
#include "./vector/vector.h"

#define STRING_PUSH_ADD 6

typedef struct string {
	char *data;
	size_t size;
	allocator_t* alloc;
} string;


static size_t string_realloc_add = 4;

//	constructors
__attribute__((always_inline, nonnull(1, 2)))
inline void str(string *self, allocator_t* alloc) {
	self->data = null;
	self->size = 0;
	self->alloc = alloc;
}

__attribute__((always_inline, nonnull(1, 2, 3)))
inline void stre(string *self, const char *eq, allocator_t* alloc) {
	self->alloc = alloc;
	self->size = strlen(eq) - 1;
	self->data = alloc->alloc(alloc, self->size);
	memcpy((void *) eq, self->data, self->size+1);
}

__attribute__((always_inline, nonnull(1, 2)))
inline void stres(string *self, const string *other, allocator_t* alloc) {
	self->alloc = alloc;
	self->size = other->size;
	self->data = alloc->alloc(alloc, self->size);
	memcpy(other->data, self->data, self->size);
}

__attribute__((nonnull(1, 2)))
void str_take_over(string *self, string *other);

__attribute__((nonnull(1, 2)))
void str_push(string *self, const char *other);

__attribute__((nonnull(1, 2)))
void str_pushs(string *self, const string *other);

__attribute__((nonnull(1)))
void str_pushc(string* self, const char c);

void str_pop(string *self, size_t count);

void str_set(string *self, const char *str);

void str_sets(string *self, const string *other);

void str_setss(string *self, const char *str, size_t size);

void str_resize(string *self, size_t size);

i8 str_cmp(const string *s1, const char *s2);

i8 str_cmps(const string *s1, const string *s2);

bool str_cmpb(const string *s1, const char *s2);

bool str_cmpbs(const string *s1, const string *s2);

__attribute__((always_inline)) inline
void str_clear(string *self) {
	if (self->data != null) {
		self->alloc->free(self->alloc, self->data);
		self->data = null;
	}
	self->size = 0;
}

//	vector_type is defined in vector/vector.h
vector_with_allocator_type(strvec, string, strvec_t, str, str_clear);

static void str_tokenize(const char* input, strvec_t* output, allocator_t* alloc);