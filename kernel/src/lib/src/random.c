//
//	random.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../random.h"

#include "../k_management.h"

void random_init() {
	seed = *((size_t*)kerninfo.build.guid) ^ *((size_t*)kerninfo.build.time);
	seed += *((size_t*)kerninfo.version);
}

void randomize() {
	seed &= 0b100111;
	seed = *((size_t*)((size_t)kerninfo.build.guid + seed));
	seed ^= tick << 33;
	seed ^= tick >> 17;
	seed ^= timespec.time.seconds >> 5;
}