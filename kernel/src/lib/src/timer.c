//
//	timer.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_TIMER_H
	#ifndef H_OS_LIB_TIMER_C
		#define H_OS_LIB_TIMER_C

		void timer_init() {

			//	configure RTC
			disable_interrupts;
			outb(0x70, 0x8a);
			outb(0x71, 0x20);

			outb(0x70, 0x8b);
			byte prev = inb(0x71);
			outb(0x70, 0x8b);
			outb(0x71, prev | 0x40);

			enable_interrupts;

			if (vocality >= vocality_report_everything) {
				report("timer configuration succeeded\n", report_note);
			}
		}

		void pit_set_divisor(size_t divisor) {
			disable_interrupts;

			u8 lobyte = (u8)(divisor & 0xff);
			u8 hibyte = (u8)((divisor >> 8) & 0xff);

			//	set PIT mode 2 (rate generator)
			outb(PIT_PORT_COMMAND, PIT_MODE_RATE_GENERATOR);

			//	set divider
			outb(PIT_PORT_CHANEL0, lobyte);
			outb(PIT_PORT_CHANEL0, hibyte);

			enable_interrupts;
		}

	#endif
#else
	#error timer.c: timer.h not included
#endif