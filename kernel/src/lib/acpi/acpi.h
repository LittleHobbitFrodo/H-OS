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
	u32 rsdt_address;	//	pointer to rsdt table

} __attribute__((packed)) acpi_rsdp_t;

typedef struct acpi_rsdp_extended_t {
	//	ACPI version 2.0 +

	char signature[8];
	u8 checksum;
	char oemid[6];
	u8 revision;
	u32 rsdt_address;

	//	ACPI version 2.0+ ONLY
	u32 length;
	u64 rsdt_address_x;
	u8 checksum_extended;
	u8 reserved[3];

} __attribute__((packed)) acpi_rsdp_extended_t;

typedef struct acpi_t {
	char signature[8];
	char manufacturer[6];
	u32 version;

	//	tables
	acpi_rsdp_extended_t* rsdp;
	acpi_rsdt_t* rsdt;
	acpi_fadt_t* fadt;
	acpi_sdt_header* dsdt;
	acpi_sdt_header* facs;

	//	vector of all tables (acpi_table_t)
	vector tables;

} acpi_t;

static acpi_t acpi;
	//	structure for simplified access


//	if those functions fail the system initialization will be halted
static void acpi_init();
static void acpi_query();
static void acpi_check();

