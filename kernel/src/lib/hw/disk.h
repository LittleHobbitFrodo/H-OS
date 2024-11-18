//
//	hw/disk.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../integers.h"
//#include "../string.h"
#include "../vector/vector.h"
#include "./device.h"

typedef enum {
	disk_connect_undefined = 0,
	disk_connect_pci = 0x80,
	disk_connect_ahci = 1,
	disk_connect_unsupported = 2,
} disk_connection_types;

typedef enum {
	disk_type_unknown = 0,
	disk_type_ssd,
	disk_type_hdd,
	disk_type_nvme
} disk_types;

typedef struct disk_t {

	struct {
		disk_connection_types type;

		void* data;
			//	PCI -> pci_connection_data
	} connect;


	char index[2];
	u64 guid[2];
	//	vector<string> mountpoints;
	//	vector<partition> partitions;

	disk_types type;

	//	void (*read)();

} disk_t;

__attribute__((always_inline))
inline void disk_construct(disk_t* disk) {
	disk->connect.type = disk_connect_undefined;
	disk->connect.data = null;
	disk->index[0] = 0;
	disk->index[1] = 0;
	disk->guid[0] = 0;
	disk->guid[1] = 0;
}

__attribute__((always_inline))
inline void disk_destruct(disk_t* disk) {
	if (disk->connect.data != null) {
		//	switch (disk->connect.type)
		free(disk->connect.data);
	}
}

vector_instance_cd(disks, disk_t, disk_vector, disk_construct, disk_destruct);

