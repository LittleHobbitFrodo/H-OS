//
//	pic.c
//		part of the CORE kernel belonging to the H-OS project
//

//	PIC = Programmable Interrupt Controller

#pragma once

#ifdef H_OS_LIB_PIC_H
	#ifndef H_OS_LIB_PIC_C
		#define H_OS_LIB_PIC_C

		void pic_remap() {

			/*u8 mask_master, mask_slave;

			mask_master = inb(PIC_PORT_MASTER_DATA);
			mask_slave = inb(PIC_PORT_SLAVE_DATA);*/

			//	start PIc init sequance
			outb(PIC_PORT_MASTER_COMMAND, PIC_COMMAND_INIT | PIC_COMMAND_ICW4_IS_PRESENT);
			outb(PIC_PORT_SLAVE_COMMAND, PIC_COMMAND_INIT | PIC_COMMAND_ICW4_IS_PRESENT);
			iowait();

			//	master/slave vector offset (ICW2)
			outb(PIC_PORT_MASTER_DATA, 32);
			outb(PIC_PORT_SLAVE_DATA, 40);
			iowait();

			//	tell master that there is slave PIC on irq2 (ICW3)
			outb(PIC_PORT_MASTER_DATA, 4);
			outb(PIC_PORT_SLAVE_DATA, 2);
			iowait();

			//	use 8086 operation mode (ICW4)
			outb(PIC_PORT_MASTER_DATA, PIC_COMMAND_ICW4_8086);
			outb(PIC_PORT_SLAVE_DATA, PIC_COMMAND_ICW4_8086);
			iowait();

			outb(PIC_PORT_MASTER_DATA, 0);
			outb(PIC_PORT_SLAVE_DATA, 0);



			//	start init sequence (cascade mode)
			/*outb(PIC_PORT_MASTER_COMMAND, PIC_COMMAND_INIT | PIC_COMMAND_ICW4_IS_PRESENT);
			iowait();
			outb(PIC_PORT_SLAVE_COMMAND, PIC_COMMAND_INIT | PIC_COMMAND_ICW4_IS_PRESENT);
			iowait();

			//	master vector offset
			outb(PIC_PORT_MASTER_DATA, 32);
			iowait();
			//	slave vector offset
			outb(PIC_PORT_SLAVE_DATA, 32+8);
			iowait();

			//	tell master that there is slave PIC irq
			outb(PIC_PORT_MASTER_DATA, 0b0100);	//	4
			iowait();

			//	tell slave that there is cascade identity
			outb(PIC_PORT_SLAVE_DATA, 0x10);		//	2
			iowait();

			//	use 8086 mode (instead of 8080)
			outb(PIC_PORT_MASTER_DATA, PIC_COMMAND_ICW4_8086);
			iowait();
			outb(PIC_PORT_SLAVE_DATA, PIC_COMMAND_ICW4_8086);
			iowait();

			//	set old masks
			pic_mask_master(0);
			pic_mask_slave(0);*/

		}

		void pic_init() {
			pic_remap();
			//	exceptions covers interrupts 0-31
			timer_init();

			pit_set_divisor(PIT_DIVISOR_1MS);
				//	set timer interrupt to every milisecond
		}

	#endif
#else
	#error pic.c: pic.h not included
#endif
