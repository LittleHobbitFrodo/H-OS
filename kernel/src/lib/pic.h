//
//	pic.h
//		part of the CORE kernel belonging to the H-OS project
//

//	PIC = Programmable Interrupt Controller

#pragma once

#ifndef H_OS_LIB_PIC_H
	#define H_OS_LIB_PIC_H

	#define PIC_PORT_MASTER_COMMAND 0x20
	#define PIC_PORT_MASTER_DATA 0x21
	#define PIC_PORT_SLAVE_COMMAND 0xA0
	#define PIC_PORT_SLAVE_DATA 0xA1

	#define PIC_COMMAND_ICW4_IS_PRESENT 0x01	//	tells pic that ICW4 will be present
	#define PIC_COMMAND_CASCADE_MODE 0x02		//	enables single mode
	#define PIC_COMMAND_INTERVAL4 0x04			//	call address interval 4
	#define PIC_COMMAND_LEVEL 0x08				//	level triggered mode
	#define PIC_COMMAND_INIT 0x10

	#define PIC_COMMAND_ICW4_8086 0x01
	#define PIC_COMMAND_ICW4_EOI 0x02			//	end of interrupt
	#define PIC_COMMAND_ICW4_MODE_MASTER 0x0c	//	buffered mode/master
	#define PIC_COMMAND_ICW4_MODE_SLAVE 0x08	//	buffered mode/slave

	//static void pic_init();
		//	Programmable Interrupt Controller

	 [[maybe_unused]] static void pic_remap();
		//	remap interrupts to start from index 32 (to not collide with exceptions)


	//	interrupt mask -> imr register (inside PIT)
		//	8bits -> if bit is 0 -> interrupt (with the bits index) can be executed)

	__attribute__((always_inline)) static inline void pic_mask_master(u8 mask) {
		//	set new mask for master
		outb(PIC_PORT_MASTER_DATA, mask);
	}

	__attribute__((always_inline)) static inline void pic_mask_slave(u8 mask) {
		//	set new mask for slave
		outb(PIC_PORT_SLAVE_DATA, mask);
	}

	[[maybe_unused]] __attribute__((always_inline)) static inline void pic_set_master(u8 interrupt) {
		//	enable master interrupt
		outb(PIC_PORT_MASTER_DATA, inb(PIC_PORT_MASTER_DATA) | ~(1 << interrupt));
	}
	[[maybe_unused]] __attribute((always_inline)) static inline void pic_set_slave(u8 interrupt) {
		//	enable slave interrupt
		outb(PIC_PORT_SLAVE_DATA, inb(PIC_PORT_SLAVE_DATA) | ~(1 << interrupt));
	}

	[[maybe_unused]] __attribute__((always_inline)) static inline void pic_unset_master(u8 interrupt) {
		//	disable master interrupt
		outb(PIC_PORT_MASTER_DATA, inb(PIC_PORT_MASTER_DATA) | (1 << interrupt));
	}

	[[maybe_unused]] __attribute__((always_inline)) static inline void pic_unset_slave(u8 interrupt) {
		outb(PIC_PORT_SLAVE_DATA, inb(PIC_PORT_SLAVE_DATA) | (1 << interrupt));
	}

//	note:	master PIC handles interrupts 0-7, needed to remap to 32 (collisions with exceptions)
	//		slave PIC handles interrupts 8-15, needed to remap to 40 (collisions with exceptions and master interrupts)


#endif