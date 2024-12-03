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

	//	gather address from PCI base address register 5
	ahci.base = pci_read_bar(ahci.pci_address, 5);

	if (ahci.base == null) {
		report("AHCI: could not find ahci base address\n", report_error);
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
	pci_memory_base base;
	u8 offset = (sizeof(pci_device_header) / sizeof(u32)) + 5;
	{
		u32 *ptr = (u32 *) &base;
		*ptr = pci_read(ahci.pci_address.bus, ahci.pci_address.slot, ahci.pci_address.function, offset);
	}
	if (base.always_zero != 0) {
		//	check if the base address is memory base address
		report("AHCI find: invalid base address\n", report_error);
		return;
	}

	switch (base.type) {
		case 0: {
			//	memory layout is 32-bit
			ahci.base = (void *) ((((size_t) base.base << 4) + pages.hhdm));
			break;
		}
		case 1: {
			//	reserved for PCI 3.0
			report("AHCI base address is PCI 3.0\n", report_warning);
			break;
		}
		case 2: {
			//	64-bit address
			size_t a = base.base << 4;
			a |= pci_read(ahci.pci_address.bus, ahci.pci_address.slot, ahci.pci_address.function, ++offset);
			ahci.base = (void *) (a + pages.hhdm);
			break;
		}
		default: break;
	}
}*/

