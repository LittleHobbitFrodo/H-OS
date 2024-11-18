//
//	hw/device.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../integers.h"
#include "./disk.h"

typedef enum device_types {
	device_type_undefined = 0,
	device_type_disk = 1,
	device_type_unknown = 0x80,
} device_types;

typedef enum device_connect {
	device_connect_unknown = 0,
	device_connect_pci = 1,
	device_connect_ahci = 2,		//	not supported yet
} device_connect;

typedef enum {
	device_connect_hw_undefined,
	device_connect_hw_sata,
	device_connect_hw_vendor_specific,
} device_connect_hw;

typedef struct device_t {

	struct {
		device_types type;
			//	disk
		void* data;
			//	disk = disk_t*
	} type;

	struct {
		device_connect connect;
		device_connect_hw hw;
	} connect;

} device_t;


void device_construct(device_t* device);
void device_destruct(device_t* device);

void device_init(device_t* device, device_types type);

vector_instance_cd(devices, device_t, devices_vector, device_construct, device_destruct);