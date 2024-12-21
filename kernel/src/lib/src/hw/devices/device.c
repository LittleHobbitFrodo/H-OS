//
//	hw/device.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../../hw/devices/device.h"

void dev_destruct(device_t* dev) {
	if (dev->ptr != null) {
		switch (dev->type) {
			case device_type_undefined: case device_type_unsupported: {
				heap.global.free(&heap.global, dev->ptr);
				break;
			}
			case device_type_disk: {
				disk_destruct((disk_t*)dev->ptr);
				break;
			}
			default: {
				report("dev_destruct: unknown device type (", report_error);
				printu(dev->type); endl();
				break;
			}
		}
	}
}

void* device_init(device_t* dev, enum device_types type) {
	switch (dev->type) {
		case device_type_disk: {
			dev->allocated = true;
			dev->ptr = heap.global.alloc(&heap.global, sizeof(disk_t));
			disk_construct((disk_t *) dev->ptr);
			dev->type = type;
			return dev->ptr;
		}
		default: {
			dev->ptr = null;
			dev->allocated = false;
			dev->type = device_type_undefined;
			return null;
		}
	}
}
