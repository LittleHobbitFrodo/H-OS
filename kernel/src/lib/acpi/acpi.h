//
//	acpi/acpi.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../integers.h"
#include "./structures.h"

typedef struct acpi_rsdp_t {
	//	ACPI version 1.0
	char signature[8];
	u8 checksum;
	char oemid[6];	//	system manufacturer
	u8 revision;
	u32 rsdt_address;	//	provide pointers to acpi tables
} __attribute__((packed)) acpi_rsdp_t;

typedef struct acpi_rsdp_extended_t {
	//	ACPI version 2.0 +

	char signature[8];
	u8 checksum;
	char oemid[6];
	u8 revision;
	u32 rsdt_address;

	u32 length;
	u64 xsdt_address;
	u8 checksum_extended;
	u8 reserved[3];

} __attribute__((packed)) acpi_rsdp_extended_t;


typedef struct acpi_t {
	acpi_rsdp_extended_t *rsdp;
	u32 version;

	//	maybe unused
	acpi_fadt *fadt;

} acpi_t;

static acpi_t acpi;
	//	structure for simplified access

static void acpi_init();
static u32 acpi_detect_version();