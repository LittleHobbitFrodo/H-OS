//
//	timer.c
//		part of the CORE kernel belonging to the H-OS project
//

//	RTC timer is used

#pragma once

#ifndef H_OS_LIB_TIMER_H
	#define H_OS_LIB_TIMER_H

	static volatile size_t tick = 0;

	typedef struct time_t {
		u16 hours;
		u16 minutes;
		u16 seconds;
		u16 milliseconds;
	} __attribute__((packed)) time_t;

	static void timer_init();

	#define PIT_PORT_COMMAND 0x43
	#define PIT_PORT_CHANEL0 0x40
	#define PIT_FREQUENCY 1193182

	#define PIT_MODE_RATE_GENERATOR 0x36

	#define PIT_DIVISOR_1MS 1000

	static void pit_set_divisor(size_t divisor);






#endif