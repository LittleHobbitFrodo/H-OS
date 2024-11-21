//
//	hw/ahci/ahci.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../integers.h"
#include "../devices/device.h"
#include "./structures.h"

typedef struct ahci_t {

	u8 used:        1;
	u8 initialized: 1;

	device_vector drives;

	void* config_base;
	pci_address pci_address;

} ahci_t;

static ahci_t ahci = {0};

static void ahci_init();


/*typedef struct ahci_connection_data {

} ahci_connection_data;*/