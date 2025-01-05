//
//	hw/ahci/structures.h
//      part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../integers.h"
#include "./sata-frames.h"

typedef volatile struct ahci_port {
	u64 command_list_base;
	u64 frame_type_base;
	u32 interrupt_status;
	u32 interrupt_enable;
	u32 cmd_and_status;
	u32 reserved;
	u32 task_file_data;
	u32 signature;
	u32 sata_status;
	u32 sata_control;
	u32 sata_error;
	u32 sata_active;
	u32 command_issue;
	u32 sata_notification;
	u32 fis_switch_control;
} __attribute__((packed)) ahci_port;

typedef volatile struct ahci_generic_host_control {

	u32 capabilities;		//	host
	u32 global_host_control;
	u32 interrupt_status;
	u32 ports_implemented;		//	each bit is one port
	u32 version;
	u32 ccc_control;	//	command completion coalescing control
	u32 ccc_ports;
	u32 enclosure_location;
	u32 enclosure_control;
	u32 capabilities_ext;
	u32 handoff;		//	BIOS/OS handoff

	u32 reserved[(0xA0 - 0x2C) / sizeof(u32)];
	u32 vendor_specific[(0x100 - 0xA0) / sizeof(u32)];

	ahci_port ports[];


} __attribute__((packed)) ahci_generic_host_control;

/*typedef volatile struct ahci_port_t {
	u64 cmd_base;		//	1kb aligned
	u64 frame_base;		//	256 aligned
	u32 interrupt_status;
	u32 interrupt_enable;
	u32 cmd;			//	command and status
	u32 reserved;
	u32 task_file;
	u32 signature;
	u32 sata_status;
	u32 sata_control;
	u32 sata_error;
	u32 sata_active;
	u32 command_issue;
	u32 sata_notification;
	u32 frame_switch_control;
	u32 reservec1[11];
	u32 vendor_specific[4];
} __attribute__((packed)) ahci_port_t;

typedef struct ahci_generic_host_control_t {
	u32 capabilities_host;
	u32 global_host_control;
	u32 interrupt_status;
	u32 port_implemented;
	u32 version;
	u32 ccc_control;		//	command completion coalescing control
	u32 ccc_port;			//	command completion coalescing port
	u32 em_location;		//	enclosure management location
	u32 em_control;			//	enclosure management control
	u32 capabilities_ext;	//	capabilities extended
	u32 bohc;				//	BIOS/OS handoff control and status

	//	0x2C - 0x9F is reserved
	u8 reserved[0xA0 - 0x2C];

	//	0xA0 - 0xFF is vendor specific
	u8 vendor_specific[0xFF - 0xA0];

	ahci_port_t ports[];


} __attribute__((packed)) ahci_generic_host_control_t;

typedef struct ahci_base_register {

} __attribute__((packed)) ahci_base_register;*/


