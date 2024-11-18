//
//	hw/ahci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

void ahci_init() {

	if (!pci_initialized) {
		pci_init();
	}

	if (device)
	for (size_t i = 0;)


}
