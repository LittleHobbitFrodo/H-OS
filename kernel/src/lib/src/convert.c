//
//	convert.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../convert.h"

void to_hex(char *str, void *ptr) {
	str[HEXLEN_PTR - 1] = '\0';
	str[0] = '0';
	str[1] = 'x';

	u8 *p = (u8 *) &ptr;
	for (size_t i = HEXLEN_PTR - 2; i > 1; p++) {
		str[i--] = hex[(*p & 0xf)];
		str[i--] = hex[((*p >> 4) & 0xf)];
	}
}

void to_hexs(char* str, size_t h) {
	if (h == 0) {
		str[0] = '0';
		str[1] = '\0';
		return;
	}
	for (ssize_t i = HEXLEN-2; i >= 0; i--) {
		str[i] = hex[(h >> (i*4)) & 0xf];
	}
	for (ssize_t i = HEXLEN - 2; i >= 0; i--) {
		if (str[i] != '0') {
			str[i+1] = '\0';
			break;
		}
	}
}



void to_string(char *str, size_t val) {
	if (val == 0) {
		str[0] = '0';
		str[1] = '\0';
	} else {
		size_t i = 0;
		while ((val > 0) && (i + 1 < INTLEN_U64)) {
			str[i++] = '0' + (val % 10);
			val /= 10;
		}
		str[i] = '\0';
		strrev(str, i);
	}
}

void to_stringi(char *str, ssize_t val) {
	size_t i = 0;
	bool negative = val < 0;
	if (negative) {
		val = -val;
	}

	while ((val > 0) && (i + 1 < INTLEN_I64)) {
		str[i++] = '0' + (val % 10);
		val /= 10;
	}

	if (negative) {
		str[i++] = '-';
	}
	str[i] = '\0';
	strrev(str, i);
}


size_t to_int(const char* str) {
	ssize_t len = strlen(str);
	ssize_t i = 0;
	size_t result = 0, mul = 1;
	for (; (i < len) && ((str[i] >= '0') && (str[i] <= '9')); i++);
	--i;

	for (; (i >= 0) && ((str[i] >= '0') && (str[i] <= '9')); i--) {
		result += (str[i] - '0') * mul;
		mul *= 10;
	}

	return result;
}

ssize_t to_inti(const char* str) {
	ssize_t len = strlen(str);
	ssize_t i = 0;
	ssize_t result = 0;
	size_t mul = 1;
	if (str[0] == '-') {
		i++;
	}
	for (; (i < len) && ((str[i] >= '0') && (str[i] <= '9')); i++);
	--i;

	for (; (i >= 0) && ((str[i] >= '0') && (str[i] <= '9')); i--) {
		result += (str[i] - '0') * mul;
		mul *= 10;
	}
	if (str[i] == '-') {
		result = -result;
	}

	return result;
}
