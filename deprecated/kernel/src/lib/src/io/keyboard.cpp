//
//  io/keyboard.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_IO_KEYBOARD_H
	#ifndef H_OS_LIB_IO_KEYBOARD_CPP
		#define H_OS_LIB_IO_KEYBOARD_CPP

		inline u8 kb::scan(u16 port) {
			u8 code;
			asm volatile("inb %1, %0" : "=a"(code) : "r"(port));
			return code;
		}

		/*inline u8 kb::scann(u16 port) {
			//  same function -> no_caller_saved_registers attribute used
			u8 code;
			asm volatile("inb %1, %0" : "=a"(code) : "r"(port));
			return code;
		}*/


		inline void kb::output(u16 port, u8 val) {
			asm volatile("outb %0, %1" :: "r"(val), "Nd"(port));
		}

		/*inline void kb::outputn(u16 port, u8 val) {
			asm volatile("outb %0, %1" :: "r"(val), "Nd"(port));
		}*/

	#else
		#warning io/keyboard.cpp already included
	#endif
#else
	#error io/keyboard.cpp: io/keyboard.h not included
#endif