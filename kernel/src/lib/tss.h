//
//	tss.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

typedef struct tss_base_t {
	u32 reserved;
	u64 rsp[3];
	u64 reserved1;
	u64 ist[7];
	u64 reserved2;
	u16 reserved3;
	u16 perms_offset;
} __attribute__((packed)) tss_base_t;

typedef u8 iomap[9192];

typedef struct tss_t {
	tss_base_t tss;
	iomap iomap;
} __attribute__((packed)) tss_t;

static void tss_init();

[[maybe_unused]] static void tss_update(segment_t seg) {
	asm volatile("ltr %0" :: "r"(seg));
}

static tss_t tss;
