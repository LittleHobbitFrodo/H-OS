//
//	hw/device.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../../hw/devices/device.h"
#include "../../../hw/pci/structures.h"

void dev_destruct(device_t* dev) {
	if (dev->allocated) {
		switch (dev->type) {
			case device_type_disk: {
				disk_destruct(dev->specific);
				heap.global.free(&heap.global, dev->specific);
				break;
			}
			//	add other device types
			default: {
				break;
			}
		}
	}
}

void* device_init(device_t* dev, enum device_types type) {
	//	allocate and constructs specific device types
	switch (type) {
		case device_type_disk: {
			dev->type = device_type_disk;
			dev->specific = heap.global.alloc(&heap.global, sizeof(disk_t));
			disk_construct(dev->specific);
			dev->allocated = true;
			return dev->specific;
		}
		//	add other device types
		default: {
			dev->type = device_type_undefined;
			dev->specific = null;
			dev->allocated = false;
			break;
		}
	}
	return null;
}

void device_connect_construct(device_connect_t* con) {
	con->allocated = false;
	con->specific = null;
	con->type = device_connect_unknown;
}
void device_connect_destruct(device_connect_t* con) {
	con->specific = null;
	con->type = device_connect_unknown;
}


void device_discovery_construct(device_discovery_t* disc) {
	disc->type = device_discovery_undefined;
	disc->specific = null;
	disc->allocated = false;
}
void* device_discovery_init(device_discovery_t* disc, enum device_discovery type) {
	switch (type) {
		case device_discovery_pci: {
			disc->allocated = true;
			disc->type = device_discovery_pci;
			disc->specific = heap.global.alloc(&heap.global, sizeof(pci_discovery_data));
			return disc->specific;
		}
		default: {
			disc->allocated = false;
			disc->type = device_discovery_undefined;
			disc->specific = null;
			break;
		}
	}
	return null;
}

void device_discovery_destruct(device_discovery_t* disc) {
	if (disc->allocated) {
		heap.global.free(&heap.global, disc->specific);
	}
	disc->specific = null;
	disc->type = device_discovery_undefined;
}