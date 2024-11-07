//
//	hw/pci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../hw/pci.h"

void pci_init() {
	report("proceeding to initialize PCI\n", report_warning);
	wait(500);

}

u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset) {

	union pci_address_u32 address;
	address.address.offset = offset & ~0b11;
	address.address.function = function;
	address.address.slot = slot;
	address.address.bus = bus;
	address.address.reserved = 0;
	address.address.enable = 1;

	outd(PCI_CONFIG_ADDRESS, address.u32);

	return ind(PCI_CONFIG_DATA);
}