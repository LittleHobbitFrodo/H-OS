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

			//	start PIC init sequence
			outb(PIC_PORT_SLAVE_COMMAND, 0x11);
			outb(PIC_PORT_MASTER_COMMAND, 0x11);

			outb(PIC_PORT_SLAVE_DATA, 40);
			outb(PIC_PORT_MASTER_DATA, 32);

			outb(PIC_PORT_SLAVE_DATA, 0x02);
			outb(PIC_PORT_MASTER_DATA, 0x04);

			outb(PIC_PORT_SLAVE_DATA, 0);
			outb(PIC_PORT_MASTER_DATA, 0);

			outb(PIC_PORT_MASTER_DATA, 0);
			outb(PIC_PORT_SLAVE_DATA, 0);

			if (vocality >= vocality_report_everything) {
				report("Interrupt controller remapped\n", report_note);
			}

		}

	#endif
#else
	#error pic.c: pic.h not included
#endif
