//
//	hw/ahci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../hw/ahci/ahci.h"

void ahci_init() {

	//  ahci.pci_address != null

	if (ahci.initialized) {
		report("ahci already initialized\n", report_problem);
		return;
	}

	if (ahci.pci_address.enable == 0) {
		report("ahci init: cannot find ahci config space\n", report_error);
		ahci.initialized = false;
		ahci.used = false;
		return;
	}

	//	gather address from PCI controller address register 5
	ahci.base = (void*)pci_read_bar(ahci.pci_address, 5, null);

	if (ahci.base == null) {
		report("AHCI: could not find ahci controller address\n", report_error);
		return;
	}

	ahci.ports = ahci.base->ports_implemented;
	for (size_t i = 0; i < sizeof(u32)*8; i++) {
		if ((ahci.ports & (1 << i)) != 0) {
			ahci.port_count++;
		}
	}

	ahci.initialized = true;
	if (vocality >= vocality_report_everything) {
		report("AHCI initialization success\n", report_note);
	}
}

/*void ahci_find() {
	pci_memory_base controller;
	u8 offset = (sizeof(pci_device_header) / sizeof(u32)) + 5;
	{
		u32 *specific = (u32 *) &controller;
		*specific = pci_read(ahci.pci_address.bus, ahci.pci_address.slot, ahci.pci_address.function, offset);
	}
	if (controller.always_zero != 0) {
		//	check if the controller address is memory controller address
		report("AHCI find: invalid controller address\n", report_error);
		return;
	}

	switch (controller.type) {
		case 0: {
			//	memory layout is 32-bit
			ahci.controller = (void *) ((((size_t) controller.controller << 4) + pages.hhdm));
			break;
		}
		case 1: {
			//	reserved for PCI 3.0
			report("AHCI controller address is PCI 3.0\n", report_warning);
			break;
		}
		case 2: {
			//	64-bit address
			size_t a = controller.controller << 4;
			a |= pci_read(ahci.pci_address.bus, ahci.pci_address.slot, ahci.pci_address.function, ++offset);
			ahci.controller = (void *) (a + pages.hhdm);
			break;
		}
		default: break;
	}
}*/

