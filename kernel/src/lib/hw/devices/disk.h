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
	DEVICE_HEADER;

	//  mountpoint/index specific
	//  read/write function pointers

} __attribute__((packed)) disk_t;

__attribute__((always_inline, nonnull(1)))
inline void disk_construct(disk_t* disk) {
	disk->type = disk_type_undefined;
	disk->connect.type = device_connect_unknown;
	disk->connect.ptr = null;
}
__attribute__((always_inline))
inline void disk_destruct([[maybe_unused]] disk_t* disk) {}

vector_instance_cd(disks, disk_t, disk_vector, disk_construct, disk_destruct);
