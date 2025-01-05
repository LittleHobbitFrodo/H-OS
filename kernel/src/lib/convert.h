//
//	convert.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

static const char *hex = "0123456789ABCDEF";

void to_string(char *str, size_t val);

void to_stringi(char *str, ssize_t val);

void to_hex(char *str, void *ptr);
void to_hexs(char* str, size_t h);

__attribute__((nonnull(1))) size_t to_int(const char* str);
ssize_t to_inti(const char* str);

#define HEXLEN_PTR (sizeof(void*) * 2) + 3
#define HEXLEN ((sizeof(size_t) * 2) + 1)

#define INTLEN_I8 4
#define INTLEN_U8 3

#define INTLEN_I16 6
#define INTLEN_U16 5

#define INTLEN_I32 11
#define INTLEN_U32 10

#define INTLEN_I64 20
#define INTLEN_U64 19
