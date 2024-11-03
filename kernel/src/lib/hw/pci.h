//
//	hw/pci.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#define PCI_CONFIG_ADDRESS 0xCF8
	//	32bit register
#define PCI_CONFIG_DATA 0xCFC

typedef struct pci_config_address_t {
	u32 offset:		8;
	u32 function:	3;
	u32 device:		5;
	u32 bus:		8;
	u32 reserved:	7;
	u32 enable:		1;
} __attribute__((packed)) pci_config_address_t;

static void pci_init();