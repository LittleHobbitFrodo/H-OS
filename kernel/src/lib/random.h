//
//	random.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

//	simple random number generator

static void random_init();

static size_t seed;

__attribute__((always_inline)) static inline size_t random() {
	//	xorshift algorithm
	seed ^= seed << 33;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	return seed;
}

static inline size_t random_range(size_t min, size_t max) {
	return (random() % (max - min)) + min;
}

[[maybe_unused]] static void randomize();