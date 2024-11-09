//
//	hw/pci/pci.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../integers.h"
#include "./structures.h"

#define PCI_CONFIG_ADDRESS 0xCF8
	//	32bit register
#define PCI_CONFIG_DATA 0xCFC

typedef struct pci_t {
	vector devices;
	bool supported;
} pci_t;

static pci_t pci;

typedef struct pci_device_t {
	vector functions;
} pci_device_t;


static void pci_init();

u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset);

bool pci_enumerate();



//	so SSD should be:
	//	class code:	0x1 -> mass storage
	//	subclass:	0x6 -> serial ATA
