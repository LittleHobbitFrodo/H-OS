//
//	smbios.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//	smbios -> system management BIOS | hardware info directly from firmware

#include "../integers.h"
#include "../vector/vector.h"

typedef struct smbios_entry_point_t {
	//	64bit version

	char anchor[5];
	u8 checksum;
	u8 length;

	u8 version_major;
	u8 version_minor;

	u8 revision;
	u8 reserved;
	u32 max_size;
	u64 table_base;

} __attribute__((packed)) smbios_entry_point_t;

enum smbios_header_types {
	smbios_header_bios = 0,
	smbios_header_system = 1,
	smbios_header_mainboard = 2,
	smbios_header_chasis = 3,
	smbios_header_cpu = 4,
	smbios_header_cache = 7,
	smbios_header_slots = 9,
	smbios_header_phys_mem_array = 16,
	smbios_header_mem_device = 17,
	smbios_header_mem_array_mapped_addrs = 19,
		//	or 20
	smbios_header_system_boot = 32,
	smbios_header_end = 127
} smbios_header_types;

typedef struct smbios_header_t {
	u8 type;
	u8 len;
	u16 handle;
} __attribute__((packed)) smbios_header_t;

typedef struct smbios_tableptr_t {
	smbios_header_t* header;
	enum smbios_header_types type;
}  smbios_tableptr_t;


static void smbios_init();

typedef struct smbios_t {
	bool supported;

	smbios_entry_point_t* entry_point;
	smbios_header_t* structure_table;
	vector tables;

} smbios_t;

static smbios_t smbios;


size_t smbios_table_length(smbios_header_t* header);