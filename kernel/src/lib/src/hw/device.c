//
//	hw/device.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

void device_construct(device_t* device) {
	device->type.type = device_type_undefined;
	device->type.data = null;
	device->connect.connect = device_connect_unknown;
	device->connect.hw = device_connect_hw_undefined;
}

void device_destruct(device_t* device) {
	switch (device->type.type) {
		case device_type_disk: {
			disk_destruct(device->type.data);
			free(device->type.data);
			break;
		}
		default: break;
	}
}

void device_init(device_t* device, device_types type) {
	//	allocates type.data
	device->type.type = type;
	switch (type) {
		case device_type_disk: {
			device->type.data = alloc(sizeof(disk_t));
			disk_construct(device->type.data);
			break;
		}
		default: break;
	}
}