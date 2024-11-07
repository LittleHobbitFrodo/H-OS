//
//	hw/pci.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once


#define PCI_CONFIG_ADDRESS 0xCF8
	//	32bit register
#define PCI_CONFIG_DATA 0xCFC

typedef struct pci_address_t {
	u32 offset:		8;
	u32 function:	3;
	u32 slot:		5;
	u32 bus:		8;
	u32 reserved:	7;
	u32 enable:		1;
} __attribute__((packed)) pci_address_t;

union pci_address_u32 {
	pci_address_t address;
	u32 u32;
} pci_address_u32;

typedef struct pci_status {

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

} __attribute__((packed)) pci_status;

typedef struct pci_command {

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

} __attribute__((packed)) pci_command;

typedef struct pci_device_header_t {
	u16 vendor;
	u16 device;		//	IDs are allocated by vendor
	u16 command;
	u16 status;

	u8 revision;		//	allocated by vendor
	u8 prog_interface;	//	specifies register-level programming (read-only)
	u8 subclass;	//	specific function (read-only)
	u8 class;		//	function (read-only)

	u8 cache_line_size;	//	32bit units
	u8 latency_timer;	//	units: pci bus locks
	u8 header_type;		//	bit 7: multi function
	u8 bist;	//	built-in self test

} __attribute__((packed)) pci_device_header_t;

static void pci_init();

u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset);


enum pci_device_header_types {
	pci_header_general_device = 0x0,
	pci_header_pci_to_pci = 0x1,
	pci_header_pci_to_cardbus = 0x2
};


typedef struct pci_header_general_device_t {

	pci_device_header_t header;
	u32 base_address[6];

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