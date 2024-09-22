//
//	font.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_FONT_H
	#define H_OS_LIB_FONT_H

	//	supported font sizes:
		//	8x8
		//	16x16
		//	32x32
		//	64x64

	static struct font {
		u16 size;
		//u8 table[97][8];
		void* table[97];
	} font;

	void font_init();

	//	default font is used for rendering text before custom font is loaded
	static struct default_font {
		u8 table[97][8];
		u16 size;
	} default_font;

	//[[maybe_unused]] static void font_load();
		//	maybe in future :D


#endif