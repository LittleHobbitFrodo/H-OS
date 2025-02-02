//
//	hw/devices/device-structures.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

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
	device_discovery_undefined = 0,
	device_discovery_pci,
	device_discovery_ahci
};

enum device_connection {
	//  16 bit
	device_connect_unknown = 0,
	device_connect_pci = 0x8000,
	device_connect_ahci = 0b1,
	device_connect_nvm = 0b10,
	device_connect_vendor_specific = 0x4000,
	device_connect_ata_bus = 4,
};

typedef struct device_connect_t {

	//  stores info about connection to machine
	void* specific;
	enum device_connection type;
	bool allocated;

} device_connect_t;

typedef struct device_discovery_t {

	//  stores info about device discovery
	void* specific;
	enum device_discovery type;
	bool allocated;

} device_discovery_t;

#define DEVICE_HEADER enum device_types type;\
device_connect_t connect;\
device_discovery_t discovery;\

typedef struct device_header_t {

	//  all device specific structures have the same header
	//  note: for devices only (disk_t, ...)
	enum device_types type;
	device_connect_t connect;
	device_discovery_t discovery;

	//  u16 type;
	//      ->  device type (enum device_types)
	//  device_connect_t connect;
	//      u16 type;       ->  device connection type (enum device_connection)
	//      void* specific;		->	connection specific data
	//	device_discovery_t discovery;
	//		u16 type;		->	original discovery (enum device_discovery)
	//		void* specific;		->	discovery specific data (maybe deleted when not needed)

} device_header_t;