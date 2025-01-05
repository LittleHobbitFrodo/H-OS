//
//	hw/ahci/ahci.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../integers.h"
#include "../devices/device.h"
#include "./sata-frames.h"
#include "./structures.h"

typedef volatile struct ahci_t {

	u8 used:        1;
	u8 initialized: 1;
	u8 res:			6;

	ahci_generic_host_control* base;
	pci_address pci_address;

	u32 ports;
	u32 port_count;

} ahci_t;

static ahci_t ahci = {0};

static void ahci_init();

static void ahci_find();
