//
//	acpi/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//	acpi is being deprecated (version 0.1.3.6 | this goofy implementation is no use anyway)

/*typedef struct acpi_sdt_header {
	//	standard header, used in other acpi structures
	char signature[4];
	u32 length;
	u8 revision;
	u8 checksum;
	u8 oemid[6];
	u64 oem_table_id;	//	additional info about oem
	u32 oem_revision;
	char acpi_compiler[4];		//	info about compiler used to build acpi
	u32 acpi_compiler_revision;

} __attribute__((packed)) acpi_sdt_header;

typedef acpi_sdt_header acpi_rsdt_t;

typedef struct acpi_addresses_t {
	u8 address_space;
	u8 bit_width;
	u8 bit_offset;
	u8 access_size;
	u64 address;

} __attribute__((packed)) acpi_addresses_t;

typedef struct acpi_fadt_t {
	//	fixed acpi descriptor table

	acpi_sdt_header header;
	u32 firmware_ctrl_ptr;
	u32 dsdt_ptr;

	//	used in ACPI v1.0, abandoned (used for compatibility)
	u8 reserved;
	u8 power_management_profile;
	u16 sci_interrupt;
	u8 acpi_enable;
	u8 acpi_disable;
	u8 s4BIOS_req;
	u8 p_state_control;
	u32 PM1_a_event_block;
	u32 PM1_b_event_block;
	u32 PM1_a_control_block;
	u32 PM1_b_control_block;
	u32 PM2_control_block;
	u32 PM_timer_block;
	u32 GPE_0_block;
	u32 GPE_1_block;
	u8 PM1_event_length;
	u8 PM1_control_length;
	u8 PM2_control_length;
	u8 PM_timer_length;
	u8 GPE_0_length;
	u8 GPE_1_length;
	u8 GPE_1_base;
	u8 c_state_control;
	u16 worst_C2_latency;
	u16 worst_C3_latency;
	u16 flush_size;
	u16 flush_stride;
	u8 duty_offset;
	u8 duty_width;
	u8 day_alarm;
	u8 month_alarm;
	u8 century;

	//	used in ACPI 2.0+
	u16 boot_architecture_flags;
	u8 reserved2;
	u32 flags;

	acpi_addresses_t reset_reg;

	u8 reset_value;
	u8 reserved3[3];

	//	64bit pointers (ACPI 2.0+ only)
	u64 x_firmware_ctrl_ptr;
	u64 x_dsdt_ptr;

	acpi_addresses_t PM1_a_event_block_X;
	acpi_addresses_t PM1_b_event_block_X;
	acpi_addresses_t PM1_a_control_block_X;
	acpi_addresses_t PM1_b_control_block_X;
	acpi_addresses_t PM2_control_block_X;
	acpi_addresses_t PM_timer_block_X;
	acpi_addresses_t GPE_0_block_X;
	acpi_addresses_t GPE_1_block_X;

} __attribute__((packed)) acpi_fadt_t;

enum power_management_profiles {
	unspecified = 0,
	desktop = 1,
	mobile = 2,
	workstation = 3,
	server_enterprise = 4,
	server_soho = 5,
	aplliance_pc = 6,
	server_performance = 7,
	//	7+ are reserved
} power_management_profiles;


enum acpi_addresses_address_space {
	//	for acpi_addresses_t
	aaas_system_memory = 0,
	aaas_system_io = 1,
	aaas_PCI_config_space = 2,
	aaas_embedded_controller = 3,
	aaas_system_management_bus = 4,
	aaas_system_CMOS = 5,
	aaas_PCI_device_bar_target = 6,
	//...
	aaas_OEM_defined = 0x80	//	and higher values
} acpi_addresses_address_space;

enum acpi_tables {
	acpi_table_bgrt,

	acpi_table_madt,
	acpi_table_bert,
	acpi_table_crep,
	acpi_table_dsdt,
	acpi_table_ecdt,
	acpi_table_einj,
	acpi_table_erst,
	acpi_table_fadt,
	acpi_table_facs,
	acpi_table_hest,
	acpi_table_msct,
	acpi_table_mpst,
	acpi_table_oem,
	acpi_table_pmtt,
	acpi_table_psdt,
	acpi_table_rasf,
	acpi_table_sbst,
	acpi_table_slit,
	acpi_table_srat,
	acpi_table_ssdt,
	acpi_table_xsdt,
	acpi_table_unsupported,
	acpi_table_unknown
} acpi_tables;

static enum acpi_tables acpi_resolve_table(const char* signature);

typedef struct acpi_table_t {
	acpi_sdt_header* ptr;
	enum acpi_tables type;
} acpi_table_t;

typedef struct acpi_bgrt_t {
	acpi_sdt_header header;
	u16 version;		//	must be 1
	u8 status;
	u8 image_type;
	u64 image_base;
	struct image_offset {
		u32 x;
		u32 y;
	} image_offset;
} __attribute__((packed))acpi_bgrt_t;

enum acpi_bgrt_status {
	bgrt_status_bits_displayed = 0b1,
	bgrt_status_bits_orientation = 0b110,	//	orientation is multiplied by 90 (0 = no offset, 1 = 90 degrees, ...)
	bgrt_status_bits_reserved = 0b11111,
} acpi_bgrt_status;*/



