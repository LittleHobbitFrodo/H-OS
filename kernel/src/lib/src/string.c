//
//	string.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../string.h"

void str_take_over(string *self, string *other) {
	if (self->data != null) {
		self->alloc->free(self->alloc, self->data);
	}
	self->data = other->data;
	self->size = other->size;

	other->data = null;
	other->size = 0;
}

void str_push(string *self, const char *str) {
	if (self->data == null) {
		self->size = strlen(str) - 1;
		self->data = (char *) self->alloc->alloc(self->alloc, self->size + STRING_PUSH_ADD);
		memcpy((void *) str, self->data, self->size);
		return;
	}
	size_t osize = self->size;
	self->size += strlen(str) - 1;
	self->data = self->alloc->realloca(self->alloc, self->data, self->size, string_realloc_add);
	memcpy((void *) str, (void *) ((size_t) self->data + osize), self->size - osize);
}

void str_pushs(string *self, const string *other) {
	if (self->data == null) {
		self->size = other->size;
		self->data = (char *) self->alloc->alloc(self->alloc, self->size);
		memcpy(other->data, self->data, self->size);
		return;
	}
	self->data = self->alloc->realloca(self->alloc, self->data, self->size + other->size, string_realloc_add);
	memcpy(other->data, (void *) ((size_t) self->data + self->size), self->size + other->size);
	self->size += other->size;
}

void str_pushc(string* self, const char c) {
	if (self->data == null) {
		self->size = 1;
		self->data = (char*)self->alloc->alloc(self->alloc, 4);
		self->data[0] = c;
		return;
	}
	self->data = self->alloc->realloca(self->alloc, self->data, ++self->size, string_realloc_add);
	self->data[self->size - 1] = c;
}

void str_pop(string *self, size_t count) {
	if (self->data == null) {
		return;
	}
	if (self->size <= count) {
		self->alloc->free(self->alloc, self->data);
		self->data = null;
		self->size = 0;
	} else {
		self->size -= count;
	}
}

void str_set(string *self, const char *str) {
	if (self->data == null) {
		self->size = strlen(str) - 1;
		self->data = (char *) self->alloc->alloc(self->alloc, self->size);
		memcpy((void *) str, self->data, self->size);
		return;
	}
	self->size = strlen(str) - 1;
	self->data = (char *) self->alloc->realloc(self->alloc, self->data, self->size);
	memcpy((void *) str, self->data, self->size);
}

void str_sets(string *self, const string *other) {
	if (self->data == null) {
		self->size = other->size;
		self->data = (char *) self->alloc->alloc(self->alloc, self->size);
		memcpy(other->data, self->data, self->size);
		return;
	}
	self->size = other->size;
	self->data = (char *) self->alloc->realloc(self->alloc, self->data, self->size);
	memcpy(other->data, self->data, self->size);
}

void str_setss(string *self, const char *str, size_t size) {
	self->size = size;
	if (self->data == null) {
		self->data = (char *) self->alloc->alloc(self->alloc, self->size);
		memcpy((void *) str, self->data, self->size);
		return;
	}
	self->data = (char *) self->alloc->realloc(self->alloc, self->data, self->size);
	memcpy((void *) str, self->data, self->size);
}

void str_resize(string *self, size_t size) {
	self->size = size;
	if (self->data != null) {
		self->data = (char *) self->alloc->realloc(self->alloc, self->data, size);
	} else {
		self->data = (char *) self->alloc->alloc(self->alloc, size);
	}
}

i8 str_cmp(const string *s1, const char *s2) {
	u32 ln = strlen(s2) - 1;
	if (s1->size < ln) {
		return -1;
	} else if (s1->size > ln) {
		return 1;
	}
	for (u32 i = 0; i < s1->size; i++) {
		if (s1->data[i] != s2[i]) {
			return (i8) (((s1->data[i] > s2[i]) * 2) - 1);
			//  dt[i] > -> 2 - 1 = 1
			//  dt[i] < -> 0 - 1 = -1
		}
	}
	return 0;
}

i8 str_cmps(const string *s1, const string *s2) {
	if (s1->size < s2->size) {
		return -1;
	} else if (s1->size > s2->size) {
		return 1;
	}
	for (u32 i = 0; i < s1->size; i++) {
		if (s1->data[i] != s2->data[i]) {
			return (i8) (((s1->data[i] > s2->data[i]) * 2) - 1);
			//  dt[i] > -> 2 - 1 = 1
			//  dt[i] < -> 0 - 1 = -1
		}
	}
	return 0;
}

bool str_cmpb(const string *s1, const char *s2) {
	if (s1->size != strlen(s2) - 1) {
		return false;
	}
	for (size_t i = 0; i < s1->size; i++) {
		if (s1->data[i] != s2[i]) {
			return false;
		}
	}
	return true;
}

bool str_cmpbs(const string *s1, const string *s2) {
	if (s1->size != s2->size) {
		return false;
	}
	for (size_t i = 0; i < s1->size; i++) {
		if (s1->data[i] != s2->data[i]) {
			return false;
		}
	}
	return true;
}


void str_tokenize(const char* input, strvec_t* output, allocator_t* allocator) {
	string token;
	str(&token, allocator);
	strvec_construct(output, 0, allocator);

	size_t cmdlen = strlen(input);
	char* tstart = null;
	size_t tlen = 0;

	for (size_t i = 0; i < cmdlen; ++i) {
		switch (input[i]) {
			case ' ':
			case '\t':
			case '\n': {
				//	push
				if (likely(tlen > 0)) {
					str_setss(&token, tstart, tlen); //	copy data from cmdline
					string *nstr = strvec_push(output, 1); //	push to strings
					str(nstr, allocator); //	initialize string
					str_take_over(nstr, &token); //	take over token string
				}
				tlen = 0;
				tstart = null;
				break;
			}
			case '\0': {
				//	important
				break;
			}
			default: {
				if (tstart == null) {
					tstart = (char *) input + i;
				}
				tlen++;
				break;
			}
		}
	}
	//	push if last word is not pushed
	if (likely(tlen > 0)) {
		str_setss(&token, tstart, tlen); //	copy data from cmdline
		string *nstr = strvec_push(output, 1); //	push to strings
		str(nstr, allocator); //	initialize string
		str_take_over(nstr, &token); //	take over token string
		nstr->size = tlen;
	}
}