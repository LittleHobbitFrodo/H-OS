//
//	string.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

typedef struct string {
	char *data;
	size_t size;
} string;

static size_t string_realloc_add = 4;

//	constructors
__attribute__((always_inline, nonnull(1))) inline void str(string *this) {
	this->data = null;
	this->size = 0;
}

__attribute__((always_inline, nonnull(1, 2))) inline void stre(string *this, const char *eq) {
	this->size = strlen(eq) - 1;
	this->data = (char *) palloc(this->size);
	memcpy((void *) eq, this->data, this->size);
}

__attribute__((always_inline, nonnull(1, 2))) inline void stres(string *this, const string *other) {
	this->size = other->size;
	this->data = palloc(this->size);
	memcpy(other->data, this->data, this->size);
}

__attribute__((nonnull(1, 2))) void str_take_over(string *this, string *other);

__attribute__((nonnull(1, 2))) void str_push(string *this, const char *other);

void str_pushs(string *this, const string *other);

__attribute__((nonnull(1))) void str_pushc(string* this, const char c);

void str_pop(string *this, size_t count);

void str_set(string *this, const char *str);

void str_sets(string *this, const string *other);

void str_setss(string *this, const char *str, size_t size);

void str_resize(string *this, size_t size);

i8 str_cmp(const string *s1, const char *s2);

i8 str_cmps(const string *s1, const string *s2);

bool str_cmpb(const string *s1, const char *s2);

bool str_cmpbs(const string *s1, const string *s2);

__attribute__((always_inline)) inline void str_clear(string *this) {
	if (this->data != null) {
		free(this->data);
	}
	this->size = 0;
}