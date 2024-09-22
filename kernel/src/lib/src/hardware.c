//
//	hardware.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_HARDWARE_H
	#ifndef H_OS_LIB_HARDWARE_C
		#define H_OS_LIB_HARDWARE_C

		void hardware_init() {

			pic_init();

			if (vocality >= vocality_report_everything) {
				report("hardware initialization success\n", report_note);
			}

		}

	#endif
#else
	#error hardware.c: hardware.h not included
#endif