//
//  io/keyboard.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_IO_KEYBOARD_H
	#define H_OS_LIB_IO_KEYBOARD_H

	namespace kb {
		inline u8 scan(u16 port);
		//[[gnu::no_caller_saved_registers]] inline u8 scann(u16 port);
			//  inb

		//[[gnu::no_caller_saved_registers]] inline void outputn(u16 port, u8 val);
		inline void output(u16 port, u8 val);
			//  outb

	}

#else
	#warning io/keyboard.h already included
#endif