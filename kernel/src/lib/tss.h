//
//	tss.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_TSS_H
	#define H_OS_LIB_TSS_H

	typedef struct tss_base_t {
		u64 rsp[3];

		u64 reserved;

		u64 ist[7];
		u64 reserved1;
		u32 reserved2;
		u16 perms_offset;
		//	offset (from base) to io perms bitmap
		u16 reserved3;
	} __attribute__((packed)) tss_base_t;

	typedef u8 iomap[9192];

	typedef struct tss_t {
		tss_base_t tss;
		iomap iomap;
	} __attribute__((packed)) tss_t;

	static void tss_init();

	static void tss_update(segment_t seg) {
		asm volatile("ltr %0" :: "r"(seg));
	}

	static tss_t tss;

#endif