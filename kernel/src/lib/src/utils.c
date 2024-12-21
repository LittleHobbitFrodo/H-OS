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

void memcpy(const void* src, void* dest, size_t size) {
	for (size_t i = 0; i < size; i++) {
		((u8 *) dest)[i] = ((u8 *) src)[i];
	}
}

void countdown(const char* msg, u8 seconds) {
	print(msg); print(" in ");
	seconds = max(seconds, 9);
	for (seconds++; seconds > 0; seconds--) {
		printc((char)'0' + (char)seconds);
		for (size_t i = 0; i < 1000; i++) {
			asm volatile("hlt");
		}
		screen_flush_at(output.line, --output.column);
	}
}

void wait(size_t mili) {
	for (size_t i = 0; i < mili; i++) {
		asm volatile("hlt");
	}
}

void strrev(char* str, size_t len) {
	size_t start = 0;
	size_t end = len - 1;
	char tmp;
	for (; start < end; start++) {
		tmp = str[start];
		str[start] = str[end];
		str[end] = tmp;
		end--;
	}
}

void memcpy_reverse(void* src, void* dest, size_t size) {
	for (size_t i = size; i > 0; i--) {
		((u8*)dest)[i-1] = ((u8*)src)[i-1];
	}
}
