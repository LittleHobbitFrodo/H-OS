//
//	hw/device.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "../../integers.h"
#include "./disk.h"

typedef struct device_connect_t {

	//  stores info about connection to machine

	u16 type;
	void* ptr;

} device_connect_t;

typedef struct device_discovery_t {

	//  stores info about device discovery

	u16 type;
	void* ptr;

} device_discovery_t;

enum device_types {
	device_type_undefined,
	device_type_unsupported,
	device_type_disk,
	device_type_display_controller,
	device_type_processor,
	device_type_base_peripheral,
	device_type_ahci
} device_types;

enum device_discovery {
	device_discovery_pci,
	device_discovery_ahci
} device_discovery;

enum device_connection {
	//  16 bit
	device_connect_unknown = 0,
	device_connect_pci = 0x8000,
	device_connect_ahci = 0b1,
	device_connect_nvm = 0b10,
	device_connect_vendor_specific = 0x4000,
	device_connect_ata_bus = 4,
} device_connection;

#define DEVICE_HEADER u16 type;\
	device_connect_t connect;\
	device_discovery_t discovery
	//  macro for less codey declarations

typedef struct device_header {

	//  all devices are have the same header
		//  note: for devices only (disk_t, ...)
	DEVICE_HEADER;

	//  u16 type;
		//      ->  device type (enum device_types)
	//  device_connect_t connect;
		//      u16 type;       ->  device connection type (enum device_connection)
		//      void* ptr;		->	connection specific data
	//	device_discovery_t discovery;
		//		u16 type;		->	original discovery (enum device_discovery)
		//		void* ptr;		->	discovery specific data (maybe deleted when not needed)

} device_header;

typedef struct device_t {

	device_header* ptr;				//	device type specific
	enum device_types type;			//  enum device types
	bool allocated;

} device_t;

__attribute__((always_inline, nonnull(1)))
inline void dev_construct(device_t* dev) {
	dev->type = device_type_undefined;
	dev->ptr = null;
	dev->allocated = false;
}

__attribute__((nonnull(1)))
void dev_destruct(device_t* dev);


__attribute__((nonnull(1)))
void* device_init(device_t* dev, enum device_types type);

readonly_vector(devices, device_t, device_rvec_t, dev_destruct);

//vector_instance(devices, device_t, device_vector, dev_construct, dev_destruct);
