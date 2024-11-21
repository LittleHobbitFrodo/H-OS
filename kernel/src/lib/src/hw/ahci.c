//
//	hw/ahci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../hw/ahci/ahci.h"
#include "../../k_management.h"
#include "../../hw/pci/pci.h"

void ahci_init() {

	//  ahci.pci_address != null

	if (ahci.initialized) {
		report("ahci already initialized\n", report_problem);
		return;
	}

	report("proceeding with AHCI initialization\n", report_note);
	wait(500);

	if (ahci.pci_address.enable == 0) {
		report("ahci init: cannot find ahci config space\n", report_error);
		ahci.initialized = false;
		ahci.used = false;
		return;
	}

	//	gather base address from pci config space (offset 0x24)
	pci_memory_base base;
	{
		u32* ptr = (u32*)&base;
		*ptr = pci_read(ahci.pci_address.bus, ahci.pci_address.slot, ahci.pci_address.function, 0x24);
	}
	if (base.always_zero != 0) {
		report("base.always_zero != 0\n", report_error);
		return;
	}
	print("AHCI config base: "); printp((void*)((size_t)base.base)); endl();




	/*if (device == null) {
		for (size_t i = 0; i < devices.len; i++) {
			device_t* device_ = devices_at(i);
			if ((device_->ptr != null) && (device_->ptr->connect.type == device_connect_ahci)) {
				device = device_;
				break;
			}
		}
	}

	if (device == null) {
		report("couldn`t find AHCI-connected device\n", report_problem);
		return;
	}

	pci_address address;
	disk_t* disk = null;

	if (device->type.type == device_type_disk) {
		disk = device->type.data;
	} else {
		//#ifdef KERNEL_DEBUG
		report("ahci init: invalid device structure initialization: device->type.data is NULL\n", report_error);
		//#endif
		return;
	}

	//  gather PCI configuration base address
	if ((disk->connect.type & (disk_connect_ahci | disk_connect_pci)) == 0b11) {
		address = ((pci_connection_data*)disk->connect.data)->address;
	} else {
		report("ahci init: couldn`t find address of ahci config space\n", report_error);
		return;
	}

	//  gather memory-mapped configuration base address (offset 0x24)
	u32 data = pci_read(address.bus, address.slot, address.function, 0x24);
	print("address read from PCI: "); printp((void*)((size_t)data)); endl();

	ahci.initialized = true;
	ahci.used = true;*/

}
