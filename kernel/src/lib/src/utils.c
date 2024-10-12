//
//	utils.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../utils.h"

i8 strcmp(const char *s1, const char *s2) {
	size_t l1 = strlen(s1);
	size_t l2 = strlen(s2);
	if (l1 < l2) {
		return -1;
	} else if (l1 > l2) {
		return 1;
	}
	for (size_t i = 0; i < l1; i++) {
		if (s1[i] != s2[i]) {
			return (i8) (((s1[i] > s2[i]) * 2) - 1);
			//  dt[i] > -> 2 - 1 = 1
			//  dt[i] < -> 0 - 1 = -1
		}
	}
	return 0;
}

void waitaminute(size_t max, const char *msg) {
	printl(msg);
	for (size_t i = 0; i < MAX_I32 / max; i++);
}

void memnull(void* ptr, size_t size) {
	//	sets chunk of memory to 0
	for (size_t i = 0; i < size; i++) {
		((u8*)ptr)[i] = 0;
	}
}

i8 strncmp(const char *s1, const char *s2, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			return (i8)(((s1[i] > s2[i]) * 2) - 1);
		}
	}
	return 0;
}

bool strncmpb(const char* s1, const char* s2, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			return false;
		}
	}
	return true;
}