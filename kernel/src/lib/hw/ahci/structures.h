//
//	hw/ahci/structures.h
//      part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../../integers.h"

enum sata_frame_types {
	//	information about sata frame information structure
	sft_register_host_to_device,
	sft_register_device_to_host,
	sft_dma_activate,
	sft_dma_setup,
	sft_data,
	sft_bist,
	sft_pio_setup,
	sft_device_bits

} sata_frame_types;

typedef struct sata_host_to_device_frame {

    u8 frame_type;          //  sata_frame_types
	u8 port_multiplier:     4;
	u8 port_reserved:       3;
	u8 port_command:        1;  //  1 = command, 0 = control
    u8 command;             //  command register
    u8 feature;

    u8 lba[3];
    u8 device;      //  device register

    u8 lba_high[3];
    u8 feature_high;

    u16 count;
    i8 iic;         //  isochronous command completion
    u8 control;     //  control register

    u32 reserved;

} __attribute__((packed)) sata_host_to_device_frame;

typedef struct sata_device_to_host_frame {

	u8 frame_type;
	u8 port_multiplier:     4;
	u8 reserved0:           2;
	u8 interrupt:           1;
	u8 reserved1:           1;
	u8 status;
	u8 error;

	u8 lba[3];
	u8 device;

	u8 lba_high[3];
	u8 reserved2;

	u16 count;
	u16 reserved3;

	u32 reserved4;


} __attribute__((packed)) sata_device_to_host_frame;

typedef struct sata_bidirectional_frame {
	u8 frame_type;
	u8 port_multiplier:     4;
	u8 reserved:            4;

	u16 reserved1;

	u32 data[];     //  size can vary

} __attribute__((packed)) sata_bidirectional_frame;