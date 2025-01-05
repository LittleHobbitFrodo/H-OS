//
//	hw/aci/sata-frames.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

//	defines structures for sata communication/configuration

enum sata_frame_types {
	sata_register_host_to_device_frame_type = 0x27,
	sata_register_device_to_host_frame_type = 0x34,
	sata_DMA_activate_device_to_host_frame_type = 0x39,
	sata_DMA_setup_bidirectional_frame_type = 0x41,
	sata_DMA_setup_frame_type = 0x41,
	sata_data_bidirectional_frame_type = 0x46,
	sata_BIST_activate_bidirectional_frame_type = 0x58,
	sata_PIO_setup_device_to_host_frame_type = 0x5F,
	sata_device_bits_device_to_host_frame_type = 0xA1,
} sata_frame_types;

typedef struct sata_register_host_to_device_frame {

	//	0x27
	//	sends command or control to device

	u8 frame_type;
	u8 port_multiplier:		4;
	u8 _reserved:			3;
	u8 iscommand:			1;		//	1 = command. 0 = control
	u8 command;
	u8 feature;

	u8 lba[3];
	u8 device;

	u8 lba_h[3];
	u8 feature_h;

	u16 count;
	u8 icc;		//	Isonchronous command completion
	u8 control;

	u32 reserved2;


} __attribute__((packed)) sata_register_host_to_device_frame;

typedef struct sata_register_device_to_host_frame {

	//	0x34
	//	used by device to tell that some register has changed

	u8 frame_type;
	u8 port_multiplier:		4;
	u8 _reserved:			2;
	u8 interrupt:			1;
	u8 _reserved2:			1;
	u8 status;
	u8 error;

	u8 lba[3];
	u8 device;

	u8 lba_h[3];
	u8 reserved;

	u16 count;
	u16 reserved2;

	u32 reserved3;

} __attribute__((packed)) sata_register_device_to_host_frame;

typedef struct sata_data_bidirectional_frame {

	//	0x46
	//	used to send data (bidirectional)

	u8 frame_type;
	u8 port_multiplier:		4;
	u8 reserved:			4;
	u16 reserved1;

	u32 data[];

} __attribute__((packed)) sata_data_bidirectional_frame;

typedef struct sata_PIO_setup_device_to_host_frame {

	//	0x5F
	//	used by device to tell host that it is ready to send/receive data payload

	u8 frame_type;
	u8 port_multiplier:		4;
	u8 _reserved:			1;
	u8 transfer_dir:		1;
	u8 interrupt:			1;
	u8 _reserved2:			1;
	u8 status;
	u8 error;

	u8 lba[3];
	u8 device;

	u8 lba_h[3];
	u8 reserved;

	u16 count;
	u8 reserved2;
	u8 e_status;

	u16 transfer_count;
	u32 reserved3;

} __attribute__((packed)) sata_PIO_setup_device_to_host_frame;

typedef struct sata_DMA_setup_bidirectional_frame {

	//	0x41
	//	?

	u8 frame_type;
	u8 port_miltiplier:		4;
	u8 _reserved:			1;
	u8 transfer_dir:		1;
	u8 interrupt:			1;
	u8 auto_activate:		1;		//	is DMA activate FIS needed?
	u16 reserved;

	u64 FMA_buffer_id;		//	?

	u32 transfer_count;

	u32 reserved2;

} __attribute__((packed)) sata_DMA_setup_bidirectional_frame;

typedef struct sata_command {
	u8 frame_length:		5;		//	in dwords
	u8 atapi:				1;
	u8 write:				1;		//	1 = host to device, 0 = device to host
	u8 reset:				1;
	u8 bist:				1;
	u8 clear:				1;		//	busy upon R_OK
	u8 reserved:			1;
	u8 port_multiplier:		1;

	u16 phys_reg_len;			//	in entries
	volatile u32 transfer_byte_count;		//	physical region descriptor byte count transferred

} __attribute__((packed)) sata_command;
