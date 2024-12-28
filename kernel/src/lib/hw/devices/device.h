//
//	hw/device.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../integers.h"
#include "./device-structures.h"
//#include "./disk.h"

__attribute__((nonnull(1)))
void device_connect_construct(device_connect_t* con);
__attribute__((nonnull(1)))
void device_connect_destruct(device_connect_t* con);
void device_connect_init(device_connect_t* con, enum device_connection type);

__attribute__((nonnull(1)))
void device_discovery_construct(device_discovery_t* disc);
__attribute__((nonnull(1)))
void* device_discovery_init(device_discovery_t* disc, enum device_discovery type);
void device_discovery_destruct(device_discovery_t* disc);



typedef struct device_t {

	void* specific;		//	disk_t for example
	enum device_types type;
	bool allocated;		//	if is allocated here or just referencing

	//	discovery, connection, etc. fields are pointed by specific
	//	all devices are allocated in heap.global
	//	all device specific structures contains device_header_t
} device_t;

__attribute__((always_inline, nonnull(1)))
inline void dev_construct(device_t* dev) {
	dev->type = device_type_undefined;
	dev->specific = null;
	dev->allocated = false;
}

__attribute__((nonnull(1)))
void dev_destruct(device_t* dev);


__attribute__((nonnull(1)))
void* device_init(device_t* dev, enum device_types type);

readonly_vector(devices, device_t, device_rvec_t, dev_construct, dev_destruct);
