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


void memcpy(const void *src, void *dest, size_t size);

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
	asm volatile("in %b0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

__attribute__((always_inline))
inline void outb(u16 port, u8 data) {
	asm volatile("out %b1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline void outw(u16 port, u16 data) {
	asm volatile("out %w1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline u16 inw(u16 port) {
	u16 ret;
	asm volatile("in %w0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

__attribute__((always_inline))
inline void outd(u16 port, u32 data) {
	asm volatile("out %d1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline u32 ind(u16 port) {
	u32 ret;
	asm volatile("in %d0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

__attribute__((always_inline))
inline void outq(u16 port, u64 data) {
	asm volatile("out %q1, %0" :: "a"(data), "Nd"(port));
}

__attribute__((always_inline))
inline u64 inq(u16 port) {
	u64 ret;
	asm volatile("in %q0, %1" : "=a"(ret) : "Nd"(port));
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

#define PRE_INTERRUPT_READ_DELAY for (size_t y = 0; y < MAX_I32/32; y++) {iowait();}
	//	short delay (in seconds) for user to read from display
	//	DEBUG/TEST ONLY

__attribute__((always_inline))
static inline void to_be_optimized([[maybe_unused]] void* a) {}
	//	empty function that will be optimized-out by compiler
		//	yes it is actually used

#define comptime_known(var) __builtin_constant_p(var)

inline u64 set_bits(u8 count) {
	u64 ret = 0;
	for (u8 i = 0; i < count; i++) {
		ret |= 1 << i;
	}
	return ret;
}

inline u64 set_bits_at(u8 offset, u8 bits) {
	u64 ret = 0;
	bits += offset;
	for (; offset < bits; offset++) {
		ret |= 1 << offset;
	}
	return ret;
}

inline void bitmap_clear(u64* map, size_t start, size_t bits) {
	const size_t index = start/64;
	const size_t offset = start - (index*64);
	const size_t tmp = ((start + bits) % 64);

	if (tmp < bits) {
		//	two qwords
		u64 mask = ~set_bits_at(offset, bits);
		map[index] &= mask;

		mask = ~set_bits(tmp);
		map[index] &= mask;
	} else {
		//	one qword case
		map[index] &= ~set_bits_at(offset, bits);
	}
}

inline void bitmap_set(u64* map, size_t start, size_t bits) {
	const size_t index = start/64;
	const size_t offset = start - (index*64);
	const size_t tmp = ((start + bits) % 64);

	if (tmp < bits) {
		//	two qwords
		u64 mask = set_bits_at(offset, bits);
		map[index] |= mask;

		mask = set_bits(tmp);
		map[index] |= mask;
	} else {
		//	one qword case
		map[index] |= set_bits_at(offset, bits);
	}
}

ssize_t bitmap_find_cleared(const u64* bitmap, size_t size, size_t n);

#define is_in_range(min, val, max) ((val >= min) && (val < max))
