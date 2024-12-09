//
//	utils.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#pragma once
#include "./integers.h"


i8 strcmp(const char *s1, const char *s2);

[[nodiscard]] __attribute__((always_inline))
inline size_t strlen(const char *s) {
	size_t i = 0;
	for (; s[i] != '\0'; i++);
	++i;
	return i;
}

static inline bool strcmpb(const char *s1, const char *s2) {
	size_t l = strlen(s1);
	bool r = true;
	if (l == strlen(s2)) {
		for (size_t i = 0; i < l; i++) {
			if (s1[i] != s2[i]) {
				r = false;
				break;
			}
		}
	} else {
		r = false;
	}
	return r;
}

__attribute__((nonnull(1, 2)))
i8 strncmp(const char* s1, const char* s2, size_t n);

__attribute__((nonnull(1, 2)))
bool strncmpb(const char* s1, const char* s2, size_t n);


void memcpy(void *src, void *dest, size_t size);

__attribute__((always_inline))
inline void memset(void *ptr, size_t size, u8 val) {
	for (size_t i = 0; i < size; i++) {
		((u8 *) ptr)[i] = val;
	}
}

void strrev(char *str, size_t len);

extern void hang();

extern void halt();

[[maybe_unused]] static void waitaminute(size_t max, const char *msg);

#define enable_interrupts asm volatile("sti");
#define disable_interrupts asm volatile("cli");

__attribute__((always_inline))
inline byte inb(u16 port) {
	byte ret;
	asm volatile("inb %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

__attribute__((always_inline))
inline void outb(u16 port, u8 data) {
	asm volatile("outb %1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline void outw(u16 port, u16 data) {
	asm volatile("outw %1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline u16 inw(u16 port) {
	u16 ret;
	asm volatile("inw %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

__attribute__((always_inline))
inline void outd(u16 port, u32 data) {
	asm volatile("outd %1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline u32 ind(u16 port) {
	u32 ret;
	asm volatile("ind %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

__attribute__((always_inline))
inline void outq(u16 port, u64 data) {
	asm volatile("outq %1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline u64 inq(u16 port) {
	u64 ret;
	asm volatile("inq %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}





__attribute__((always_inline))
inline void iowait() {
	outb(0x80, 0);
}

static void memnull(void* ptr, size_t size);

#define max(x, max_) ((x > max_)? max_ : x)
#define min(x, min_) ((x < min_)? min_ : x)

void countdown(const char* msg, u8 seconds);

void wait(size_t milli);


void memcpy_reverse(void* src, void* dest, size_t size);



__attribute__((always_inline))
static inline void to_be_optimized([[maybe_unused]] void* a) {}
	//	empty function that will be optimized-out by compiler
		//	yes it is actually used
