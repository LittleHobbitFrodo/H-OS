//
//	output.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"


#define FONT_PLACE_SUB 31
#define FONT_SIZE 97	//	128 - 31

#ifndef SPACE_BETWEEN_LINES_DEFAULT
#define SPACE_BETWEEN_LINES_DEFAULT 2
#endif

#ifndef OUT_TAB_SPACE_COUNT
#define OUT_TAB_SPACE_COUNT 4
#endif

static struct screen {
	u64 count;

	//	no framebuffer => everything 0 and address null

	u64 h;
	u64 w;
	u16 bpp;

	u32 *address;
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
	volatile size_t line;
	volatile size_t column;

	u8 space_between_lines;
	size_t fb;

	u32 color;

	volatile bool lock;
} output;

void output_init();


static inline void endl() {
	output.line += (output.column / screen.w) + (1 * (output.column < screen.w));
	output.column = 0;
}

/*__attribute__((always_inline))*/
static inline void tab() {
	output.column += OUT_TAB_SPACE_COUNT - (output.column % OUT_TAB_SPACE_COUNT);
	if (output.column >= screen.w) {
		endl();
	}
}

void print(const char *s);

void printl(const char *s);

void prints(const string *s);

void printsl(const string *s);

void printi(const i64 i);

void printu(const u64 u);

void printp(void *p);

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
				u32 *ptr = screen.address + ((output.line * screen.w * (font.size + output.space_between_lines))) + (output.column * font.size);

				switch (font.size) {
					case 8: {
						u8 *fnt;
						for (u16 i = 0; i < font.size; i++) {
							fnt = font.table[actual];
							for (u16 ii = 0; ii < font.size; ii++) {
								*(ptr + (i * screen.w) + (font.size - ii)) = output.color * ((fnt[i] >> ii) & 1);
							}
						}
						break;
					}
					case 16: {
						u16 *fnt;
						for (u16 i = 0; i < font.size; i++) {
							fnt = font.table[actual];
							for (u16 ii = 0; ii < font.size; ii++) {
								*(ptr + (i * screen.w) + (font.size - ii)) = output.color * ((fnt[i] >> ii) & 1);
							}
						}
						break;
					}
					case 32: {
						u32 *fnt;
						for (u16 i = 0; i < font.size; i++) {
							fnt = font.table[actual];
							for (u16 ii = 0; ii < font.size; ii++) {
								*(ptr + (i * screen.w) + (font.size - ii)) = output.color * ((fnt[i] >> ii) & 1);
							}
						}
						break;
					}
					case 64: {
						u64 *fnt;
						for (u16 i = 0; i < font.size; i++) {
							fnt = font.table[actual];
							for (u16 ii = 0; ii < font.size; ii++) {
								*(ptr + (i * screen.w) + (font.size - ii)) = output.color * ((fnt[i] >> ii) & 1);
							}
						}
						break;
					}
					default: break;
				}


				output.column++;
				if (output.column >= screen.w) {
					endl();
				}
			}
		}
	}
}