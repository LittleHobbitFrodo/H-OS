//
//	int-handler.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./interrupts.h"

extern void isr_int_timer_pit(void);

extern void isr_int_keyboard_input(void);
//extern void isr_int_keyboard_input(void);

//extern void interrupt_keyboard_input_scs2(void);

extern void interrupt_keyboard_input(void);
