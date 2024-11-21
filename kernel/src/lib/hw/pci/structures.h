//
//	hw/pci/structures.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

enum pci_device_types {
	pci_general_device = 0x0,
	pci_pci_to_pci = 0x1,
	pci_pci_to_cardbus = 0x2
};

typedef struct pci_address {
	u32 zero:		2;
	u32 offset:		6;
	u32 function:	3;
	u32 slot:		5;
	u32 bus:		8;
	u32 reserved:	7;
	u32 enable:		1;
} __attribute__((packed)) pci_address;

union pci_address_u32 {
	pci_address address;
	u32 u32;
} pci_address_u32;

typedef struct pci_status_t {

	u16 reserved:			2;
	u16 interrupt_status:	1;			//	1 and interrupts enabled = signal will be sent
	u16 capabilities:		1;			//	1 = there is a linked list of capabilities on offset 0x34
	u16 capable_66mhz:		1;			//	1 = is capable of running on 66mhz (otherwise on 33mhz)
	u16 reserved2:			1;
	u16 capable_fast_btb:	1;			//	1 = able accept fast back-to-backs transactions from multiple agents (otherwise from only one)
	u16 master_data_parity_err: 1;		//
	u16 timing_devsel:		2;			//	slowest time that will take to assert DEVSEL# | 0x0 fast timing, 0x1 medium, 0x2 slow
	u16 target_abort_signaled:	1;
	u16 received_target_abort:	1;
	u16 received_master_abort:	1;
	u16 signaled_system_error:	1;
	u16 parity_error_detected:	1;

} __attribute__((packed)) pci_status_t;

typedef struct pci_command_t {

	u16 respond_io:		1;		//	1 = can respond to io space accesses
	u16 respond_mem:	1;		//	1 = can respond to memory space access
	u16 master:			1;		//	1 = can act like bus master
	u16 special_cycles:	1;		//	1 = can monitor special cycle operations (otherwise ignores them)
	u16 mem_write:		1;		//	1 = can generate memory write and invalidate command (otherwise the memory write command must be used)
	u16 vga_snoop:		1;		//	1 = will not respond to vga palette writes
	u16 parity_error:	1;		//	1 = will do normal action when parity error is detected
		//	otherwise will set the parity bit and operate as normally
	u16 reserved:		1;
	u16 enable_SERR:	1;		//	1 = SERR# driver is enabled (else is disabled)
	u16 enable_fbtb:	1;		//	1 = enable fast back to back transactions
	u16 interrupt_disable:	1;	//	1 = interrupt signal is disabled

} __attribute__((packed)) pci_command_t;

typedef struct pci_bist_t {
	//	built-in self test
	u8 code:		4;		//	0 = success
	u8 reserved:	2;
	u8 start:		1;		//	if set self test is invoekd
	u8 capable:		1;
} __attribute__((packed)) pci_bist_t;

typedef struct pci_header_type {
	u8 header_type:	7;
	u8 multifunc:	1;
} __attribute__((packed)) pci_header_type;

typedef struct pci_device_header {
	u16 vendor;
	u16 device;

	pci_command_t command;
	pci_status_t status;

	u8 revision;
	u8 programming;
	u8 subclass;
	u8 class;

	u8 cache_size;
	u8 latency;
	pci_header_type header_type;
	pci_bist_t bist;

} __attribute__((packed)) pci_device_header;

typedef struct pci_memory_base {

	u32 always_zero:	1;
	u32 type:			2;
	u32 prefetchable:	1;
	u32 base:			28;

} __attribute__((packed)) pci_memory_base;

typedef struct pci_io_base {

	u32 always_one:		1;
	u32 reserved:		1;
	u32 base:			30;

} __attribute__((packed)) pci_io_base;


typedef struct pci_header_general_device_t {
	//	code: 0x0

	pci_device_header header;
	pci_memory_base base_address[6];

	u32 cardbus_CIS_pointer;

	u16 subsystem_vendor_id;
	u16 subsystem_device_id;

	u32 ROM_base_address;
	u8 capabilities_offset;		//	offset (in config space) to linked list of new capabilities (if capabilities bit in status register is set to 1)
	u8 reserved[7];

	u8 interrupt_line;
	u8 interrupt_pin;

	u8 min_grant;		//	specifies burst period length (1/4 microseconds) (assuming 33mhz)
	u8 max_latency;		//	how often the device needs to access the bus (1/4 microseconds)

} __attribute__((packed)) pci_header_general_device_t;

typedef struct pci_header_pci_to_pci_t {
	//	code: 0x1

	pci_device_header header;
	pci_memory_base base_address[2];

	u8 primary_bus_number;
	u8 secondary_bus_number;
	u8 subordinate_bus_number;
	u8 secondary_latency_timer;

	u8 io_base;
	u8 io_limit;
	u16 secondary_status;

	u16 memory_base;
	u16 memory_limit;

	u16 prefetchable_memory_base;
	u16 prefetchable_memory_limit;

	u32 prefetchable_base1;
	u32 prefetchable_limit1;

	u16 io_base1;
	u16 io_limit1;

	u8 capability_ptr;
	u8 reserved[3];

	u32 ROM_base;

	u8 interrupt_line;
	u8 interrupt_pin;
	u16 bridge_control;

} __attribute__((packed)) pci_header_pci_to_pci_t;

typedef struct pci_header_pci_to_cardbus_t {
	//	code:	0x2

	pci_device_header header;

	u32 cardbus_socket;

	u8 capabilities_offset;
	u8 reserved;
	u16 secondary_status;

	u8 pci_bus_number;
	u8 cardbus_bus_number;
	u8 subordinate_bus_number;
	u8 cardbus_latency_timer;

	u32 memory_base;
	u32 memory_limit;
	u32 memory_base1;
	u32 memory_limit1;

	u32 io_base1;
	u32 io_limit1;

	u8 interrupt_line;
	u8 interrupt_pin;
	u16 bridge_control;

	u16 subsystem_device_ID;
	u16 subsystem_vendor_ID;

	u32 legacy_mode_base;

} __attribute__((packed)) pci_header_pci_to_cardbus_t;


enum pci_device_classes {
	pci_device_class_unclassified = 0x0,
	pci_device_class_mass_storage_controller = 0x1,
	pci_device_class_network_controller = 0x2,
	pci_device_class_display_controller = 0x3,
	pci_device_class_multimedia_controller = 0x4,
	pci_device_class_memory_controller = 0x5,
	pci_device_class_bridge = 0x6,
	pci_device_class_simple_communication_controller = 0x7,
	pci_device_class_base_system_peripheral = 0x8,
	pci_device_class_input_device_controller = 0x9,
	pci_device_class_processor = 0xB,
	pci_device_class_serial_bus_controller = 0xC,
	pci_device_class_wireless_controller = 0xD,
} pci_device_classes;

enum pci_controller_mass_storage {
	pci_mass_storage_scsi_bus = 0x0,
	pci_mass_storage_ide = 0x1,
	pci_mass_storage_floppy = 0x2,
	pci_mass_storage_ipi = 0x3,
	pci_mass_storage_raid = 0x4,
	pci_mass_storage_ata = 0x5,
	pci_mass_storage_serial_ata = 0x6,
	pci_mass_storage_sasc = 0x7,
	pci_mass_storage_nvm_controller = 0x8,
	pci_mass_storage_other = 0x80,
} pci_controller_mass_storage;

enum pci_serial_ata_programming {
	pci_serial_ata_vendor_specific = 0x0,
	pci_serial_ata_ahci = 0x1,
	pci_serial_ata_serial_storage_bus = 0x2
};

enum pci_controller_base_peripheral {
	pci_base_peripheral_pic = 0x0,
	pci_base_peripheral_dma_controller = 0x1,
	pci_base_peripheral_timer = 0x2,
	pci_base_peripheral_rtc_controller = 0x3,
	pci_base_peripheral_other = 0x80,
} pci_controller_base_peripheral;