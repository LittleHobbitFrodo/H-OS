//
//	int-handler.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_INT_HANDLER_H
	#ifndef H_OS_LIB_INT_HANDLER_C
		#define H_OS_LIB_INT_HANDLER_C

		void interrupt_timer_pit([[maybe_unused]] int_stack_frame* frame) {
			INTERRUPT_ENTER;
			tick++;

			outb(0x20, 0x20);

			INTERRUPT_LEAVE;
		}

	#endif
#else
	#error int-handler.c: int-handler.h not included
#endif