//
//	acpi/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

typedef struct acpi_std_header {
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

} __attribute__((packed)) acpi_std_header;

typedef struct acpi_addresses_t {
	u8 address_space;
	u8 bit_width;
	u8 bit_offset;
	u8 access_size;
	u64 address;

} __attribute__((packed)) acpi_addresses_t;

typedef struct acpi_fadt {
	//	fixed acpi descriptor table

	acpi_std_header header;
	u32 firmware_ctrl;
	u32 dst;

	//	used in ACPI v1.0, abandoned (used for compatibility)
	u8 reserved;
	u8 preferred_power_management_profile;
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
	u64 firmware_control_ptr;
	u64 dst_ptr;

	acpi_addresses_t PM1_a_event_block_X;
	acpi_addresses_t PM1_b_event_block_X;
	acpi_addresses_t PM1_a_control_block_X;
	acpi_addresses_t PM1_b_control_block_X;
	acpi_addresses_t PM2_control_block_X;
	acpi_addresses_t PM_timer_block_X;
	acpi_addresses_t GPE_0_block_X;
	acpi_addresses_t GPE_1_block_X;

} __attribute__((packed)) acpi_fadt;




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