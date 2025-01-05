//
//	hw/pci/pci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../hw/pci/pci.h"
#include "../../../hw/ahci/ahci.h"

void pci_init() {

	if (vocality >= vocality_report_everything) {
		report("proceeding with PCI initialization\n", report_note);
	}

	pci_enumerate();

	if (ahci.used) {
		ahci_init();
	}

	pci_initialized = true;

	if (vocality >= vocality_report_everything) {
		report("PCI initialization completed\n", report_note);
	}

}

u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset) {

	union pci_address_u32 address;
	address.address.zero = 0;
	address.address.offset = offset;
	address.address.function = function;
	address.address.slot = slot;
	address.address.bus = bus;
	address.address.reserved = 0;
	address.address.enable = 1;

	outd(PCI_CONFIG_ADDRESS, address.u32);

	return ind(PCI_CONFIG_DATA);
}


void pci_scan_bus(u8 bus) {

	for (size_t i = 0; i < 8; i++) {
		for (size_t ii = 0; ii < 8; ii++) {
			if (!pci_exists(bus, i , ii)) {
				continue;
			}
			u8 class = pci_read_class(bus, i, ii);
			u8 subclass = pci_read_subclass(bus, i, ii);
			u8 programming = pci_read_programming(bus, i, ii);

			switch (class) {
				case pci_device_class_mass_storage_controller: {

					device_t* device = devices_push(1);
					device->type = device_type_disk;
					switch (subclass) {
						case pci_mass_storage_serial_ata: {
							switch (programming) {
								case pci_serial_ata_ahci: {
									devices.len--;  //  pop
									ahci.pci_address = pci_address_construct(bus, i, ii, 0, 1);
									ahci.used = true;
									break;
								}
								case pci_serial_ata_vendor_specific: {
									disk_t* disk = disks_push(1);
									device->ptr = (device_header*)disk;
									disk->connect.type = device_connect_vendor_specific;
									disk->discovery.type = device_discovery_pci;
									disk->discovery.ptr = alloc(sizeof(pci_discovery_data));
									pci_discovery_data* ddata = disk->discovery.ptr;
									ddata->address = pci_address_construct(bus, i, ii, 0, 1);
									ddata->class = class;
									ddata->subclass = subclass;
									ddata->programming = programming;
									u8* bist = (u8*)&ddata->test;
									*bist = (pci_read(bus, i, ii, 3) >> 24) & 0xff;
									break;
								}
								case pci_serial_ata_serial_storage_bus: {
									disk_t* disk = disks_push(1);
									device->ptr = (device_header*)disk;
									disk->connect.type = device_connect_ata_bus;
									break;
								}
								default: {
									disk_t* disk = disks_push(1);
									device->ptr = (device_header*)disk;
									disk->connect.type = device_connect_unknown;
									break;
								}
							}
							break;
						}
						case pci_mass_storage_nvm_controller: {
							disk_t* disk = disks_push(1);
							disk->connect.type = device_connect_nvm;
							device->ptr = (device_header*)disk;
							break;
						}
						default: {
							disk_t* disk = disks_push(1);
							disk->connect.type = device_connect_unknown;
							device->ptr = (device_header*)disk;
							break;
						}
					}
					break;
				}
				case pci_device_class_display_controller: {
					device_t* device = devices_push(1);
					device->type = device_type_display_controller;
					break;
				}
				case pci_device_class_processor: {
					device_t* device = devices_push(1);
					device->type = device_type_processor;
					break;
				}
				case pci_device_class_base_system_peripheral: {
					device_t* device = devices_push(1);
					device->type = device_type_base_peripheral;
					break;
				}
				default: break;
			}
		}
	}
}


u8 pci_enumerate() {
	for (u16 i = 0; i < 256; i++) {
		pci_scan_bus((u8)i);
	}

	return ok;
}