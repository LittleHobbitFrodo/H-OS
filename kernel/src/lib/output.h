//
//	output.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_OUTPUT_H
	#define H_OS_LIB_OUTPUT_H


	#define FONT_PLACE_SUB 31
	#define FONT_SIZE 97	//	128 - 31

	#ifndef SPACE_BETWEEN_LINES_DEFAULT
		#define SPACE_BETWEEN_LINES_DEFAULT 2
	#endif

	#ifndef OUT_TAB_SPACE_COUNT
		#define OUT_TAB_SPACE_COUNT 4
	#endif

	static struct font {
		u16 h;
		u16 w;
		u8 table[97][8];
	} font;

	void font_init();	//	in bitmap.c

	static volatile struct limine_framebuffer_request fb = {
		.id = LIMINE_FRAMEBUFFER_REQUEST,
		.revision = 0
	};

	static struct screen {

		u64 count;

		//	no framebuffer => everything 0 and address null

		u64 h;
		u64 w;
		u16 bpp;

		u32* address;

	} screen;

	void screen_init();
	__attribute__((always_inline)) static inline void screen_flush() {
		size_t size = screen.w * screen.h;
		for (size_t i = 0; i < size; i++) {
			screen.address[i] = 0;
		}
	}

	static struct col {
		u32 white;
		u32 green;
		u32 red;
		u32 blue;
		u32 critical;

		u32 yellow;
		u32 orange;
		u32 cyan;

		u32 grey;
	} col;

	static struct output {
		size_t line;
		size_t column;

		u8 space_between_lines;
		size_t fb;

		u32 color;
	} output;

	void output_init();


	/*__attribute__((always_inline))*/ static inline void endl() {
		output.line += (output.column / screen.w) + (1 * (output.column < screen.w));
		output.column = 0;
	}
	/*__attribute__((always_inline))*/ static inline void tab() {
		output.column += OUT_TAB_SPACE_COUNT - (output.column % OUT_TAB_SPACE_COUNT);
		if (output.column >= screen.w) {
			endl();
		}
	}

	void print(const char* s);
	void printl(const char* s);
	void prints(const string* s);
	void printsl(const string* s);

	void printi(const i64 i);
	void printu(const u64 u);
	void printp(void* p);
	void printb(size_t bin);


	__attribute__((target("general-regs-only"))) static inline void printc(const char c) {
		if (c >= 0) {
			switch (c) {
				case '\n': {
					endl();
					break;
				}
				case ' ': {
					output.column++;
					if (output.column >= screen.w) {
						endl();
					}
					break;
				}
				case '\t': {
					tab();
					break;
				}
				default: {
					u8 actual = c * (c > FONT_PLACE_SUB) - (FONT_PLACE_SUB * (c > FONT_PLACE_SUB));
					u32* ptr = screen.address + ((output.line * screen.w * (font.h + output.space_between_lines))) + (output.column * font.w);

					for (u16 i = 0; i < font.h; i++) {
						for (u16 ii = 0; ii < font.w; ii++) {
							*(ptr + (i * screen.w) + (font.h - ii)) = output.color * ((font.table[actual][i] >> ii) & 1);
						}
					}

					output.column++;
					if (output.column >= screen.w) {
						endl();
					}
				}
			}
		}
	}

#endif
//	#warning output.h already included
//#endif