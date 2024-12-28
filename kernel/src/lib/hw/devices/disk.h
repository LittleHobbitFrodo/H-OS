//
//	hw/disk.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../integers.h"
#include "../../vector/vector.h"
#include "./device.h"

//device_connection enum is defined in device.h

enum disk_types {
	disk_type_undefined,
	disk_type_unsupported,
	disk_type_ssd,
	disk_type_hdd,
	disk_type_nvm
} disk_types;

typedef struct disk_t {

	//  header
	device_header_t header;
	enum disk_types type;
	u128 guid;

	char index[2];

	//  mountpoint/index specific
	//  read/write function pointers

} disk_t;

__attribute__((always_inline, nonnull(1)))
inline void disk_construct(disk_t* disk) {
	disk->header.type = device_type_disk;
	disk->header.connect.type = device_connect_unknown;
	disk->header.connect.specific = null;

	disk->type = disk_type_undefined;
	disk->index[0] = '0';
	disk->index[0] = '0';
	disk->guid = 0;
}
__attribute__((always_inline))
inline void disk_destruct(disk_t* disk) {
	if (disk->header.connect.allocated) {
		device_connect_destruct(disk->header.connect.specific);
		heap.global.free(&heap.global, disk->header.connect.specific);
	}
	if (disk->header.discovery.allocated) {
		device_discovery_destruct(disk->header.discovery.specific);
		heap.global.free(&heap.global, disk->header.discovery.specific);
	}
}

vector_instance(disks, disk_t, disk_vector, disk_construct, disk_destruct);
