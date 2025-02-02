//
//	hw/pci/pci.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../../hw/pci/pci.h"
#include "../../../hw/ahci/ahci.h"


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

u64 pci_read_bar(pci_address address, u8 bar, bool* is_io) {
	//	returns PCI BAR X
		//	writes 1 if BAR is registered as IO

	if (bar >= 6) {
		return 0;
	}

	address.offset = sizeof(pci_device_header)/sizeof(u32) + bar;
	union pci_base_reg_u32 reg = {.u32 = pci_reada((union pci_address_u32)address)};

	if (is_io != null) {
		*is_io = reg.reg.io_reg;
	}

	switch (reg.reg.type) {
		case 0: {
			//	32 bit
			return reg.reg.base << 4;
		}
		case 1: default: {
			//	reserved
			return 0;
		}
		case 2: {
			address.offset++;
			size_t a = (u64)(reg.reg.base << 4);
			print("BAR0:\t\t"); printp((void*)((size_t)reg.reg.base << 4)); endl();
			a &= 0xffffffff;
			print("BAR1:\t\t"); printp((void*)((size_t)pci_reada((union pci_address_u32)address))); endl();
			a |= (u64)((u64)pci_reada((union pci_address_u32)address) << 32);
			return a;
		}
	}
}


void pci_init() {

	size_t line = 0;

	if (vocality >= vocality_report_everything) {
		line = report("initializing system peripherals (PCI)\n", report_note);
	}

	u32 count = pci_enumerate();
	if (count == 0) {
		if (vocality >= vocality_report_everything) {
			report_status("PARTIAL FAILURE", line, col.yellow);
		}
		report("no PCI device found\n", report_warning);
		pci.used = false;
		pci.initialized = true;
	} else {
		print("detected "); printu(count); printl(" PCI devices");
	}

	//	build readonly vector for PCI devices
	devices_alloc(&pci.devices, &heap.global, count);

	pci_scan();

	if (ahci.used) {
		ahci_init();
	}

	if (nvm.used) {
		nvm_init();
	}

	pci.initialized = true;
	pci.used = true;

	if (vocality >= vocality_report_everything) {
		report_status("SUCCESS", line, col.green);
	}

}


u32 pci_enumerate() {

	u32 count = 0;
	pci_device_info_t info = {0};
	const pci_device_info_t* device = null;

	for (size_t i = 0; i < 256; i++) {
		for (size_t ii = 0; ii < 8; ii++) {
			for (size_t iii = 0; iii < 8; iii++) {
				if (!pci_exists(i, ii, iii)) {
					continue;
				}

				info = pci_read_info(i, ii, iii);

				for (size_t iv = 0; iv < pci_supported_devices_count; iv++) {
					device = &pci_supported_devices[iv];
					if ((device->class != info.class) && (info.class != pci_device_info_any)) {
						continue;
					}
					if ((device->subclass != info.subclass) && (info.subclass != pci_device_info_any)) {
						continue;
					}
					if ((device->programming != info.programming) && (info.programming != pci_device_info_any)) {
						continue;
					}
					//	device is supported
					count++;
				}
			}
		}
	}

	return count;
}

#define _pci_print(i, ii, iii) printu(i); print(" : "); printu(ii); print(" : "); printu(iii)

void pci_scan() {

	//	scans each bus and writes data into vector

	pci_device_info_t info = {0};
	const pci_device_info_t* supported_device = null;
	size_t v = 0;		//	vector iterator
	bool supported = false;

	for (size_t i = 0; i < 256; i++) {
		for (size_t ii = 0; ii < 8; ii++) {
			for (size_t iii = 0; iii < 8; iii++) {
				if (!pci_exists(i, ii, iii)) {
					continue;
				}

				supported = false;
				info = pci_read_info(i, ii, iii);

				for (size_t iv = 0; iv < pci_supported_devices_count; iv++) {
					supported_device = &pci_supported_devices[iv];
					if ((supported_device->class != info.class) && (info.class != pci_device_info_any)) {
						continue;
					}
					if ((supported_device->subclass != info.subclass) && (info.subclass != pci_device_info_any)) {
						continue;
					}
					if ((supported_device->programming != info.programming) && (info.programming != pci_device_info_any)) {
						continue;
					}
					//	supported_device is supported
					supported = true;
					break;
				}
				if (!supported) {
					continue;
				}

				switch (info.class) {
					case pci_device_class_mass_storage_controller: {
						switch (info.subclass) {
							case pci_mass_storage_serial_ata: {
								switch (info.programming) {
									case pci_serial_ata_ahci: {
										printl("found AHCI");
										ahci.used = true;
										ahci.pci_address = pci_address_construct(i, ii, iii, 0, true);

										device_t* device = &pci.devices.data[v];

										disk_t* disk = device_init(device, device_type_disk);
										disk->header.discovery.specific = heap.global.alloc(&heap.global, sizeof(pci_discovery_data));
										disk->header.discovery.type = device_discovery_pci;
										disk->header.discovery.allocated = true;

										pci_discovery_data* data = disk->header.discovery.specific;
										data->class = info.class;
										data->subclass = info.subclass;
										data->programming = info.programming;
										data->address = pci_address_construct(i, ii, iii, 0, true);

										disk->type = disk_type_ssd;
										disk->header.connect.allocated = false;
										disk->header.connect.type = device_connect_pci;
										disk->header.connect.specific = disk->header.discovery.specific;

										break;
									}
									default: {
										printl("mass storage: SATA: unknown");
										break;
									}
								}
								break;
							}
							case pci_mass_storage_nvm_controller: {
								switch (info.programming) {
									case pci_nvm_controller_nvme: {
										//printl("found NVMe");
										nvm.address.pci = pci_address_construct(i, ii, iii, 0, true);
										nvm.used = true;

										device_t* device = &pci.devices.data[v];
										disk_t* disk = device_init(device, device_type_disk);
										disk->type = disk_type_nvm;
										disk->header.type = device_type_disk;

										disk->header.discovery.type = device_discovery_pci;
										disk->header.discovery.allocated = false;
										disk->header.discovery.specific = heap.global.alloc(&heap.global, sizeof(pci_discovery_data));
										pci_discovery_data* data = disk->header.discovery.specific;
										data->address = pci_address_construct(i, ii, iii, 0, true);
										data->class = info.class;
										data->subclass = info.subclass;
										data->programming = info.programming;

										disk->header.connect.type = device_connect_pci;
										disk->header.connect.allocated = false;
										disk->header.connect.specific = disk->header.discovery.specific;

										//	construct supported_device
										break;
									}
									case pci_nvm_controller_nvmhci: {
										printl("found NVMHCI (unsupported)");
										break;
									}
									default: {
										printl("mass storage: NVM: unknown");
										break;
									}
								}
								break;
							}
						}
						break;
					}
					case pci_device_class_display_controller: {
						switch (info.subclass) {
							case 0x0: {		//	VGA compatible
								printl("display controller");
								break;
							}
							default: {
								printl("display controller (unknown)");
								break;
							}
						}
						break;
					}
					case pci_device_class_processor: {
						printl("processor");
						break;
					}
					case pci_device_class_base_system_peripheral: {
						switch (info.subclass) {
							case pci_base_peripheral_pic: {
								printl("PIC");
								break;
							}
							case pci_base_peripheral_rtc_controller: {
								printl("RTC");
								break;
							}
							case pci_base_peripheral_timer: {
								printl("timer");
								break;
							}
						}
						break;
					}
					default: {
						printl("unknown");
					}
				}

				v++;

			}
		}
	}

}

void pci_set_interrupt(pci_address address, u8 interrupt) {
	address.offset = (sizeof(pci_header_general_device_t)/sizeof(u32))-1;
		//	offset at interrupt_line (pci_header_general_device_t)
	u32 data = pci_reada((union pci_address_u32)address);
	data &= 0xff;
	data |= interrupt;
	pci_writea((union pci_address_u32)address, data);
}

i16 pci_find_and_set_interrupt(pci_address address, void (*interrupt)(void)) {
	//	finds empty place in IDT
	for (size_t i = 31; i < 255; i++) {
		if (idt_address(&idt[i]) == null) {
			idt_set_address(&idt[i], interrupt);
			pci_set_interrupt(address, (u8)i);
			return (i16)i;
		}
	}
	return -1;
}

