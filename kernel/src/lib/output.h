//
//	output.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"
#include "./font.h"
//#include "./string.h"


#define FONT_PLACE_SUB 31
#define FONT_SIZE 97	//	128 - 31

#ifndef SPACE_BETWEEN_LINES_DEFAULT
#define SPACE_BETWEEN_LINES_DEFAULT 2
#endif

#ifndef OUT_TAB_SPACE_COUNT
#define OUT_TAB_SPACE_COUNT 4
#endif

typedef struct screen_t {
	u64 count;

	//	no framebuffer => everything 0 and address null

	u64 h;
	u64 w;
	u16 bpp;

	u32 *address;
} screen_t;

static screen_t screen;

void screen_init();

static void screen_flush();

static void screen_flush_at(size_t line, size_t column);


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

	u32 hint;
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

void printi(const i64 i);

void printu(const u64 u);

void printp(void *p);
void printh(size_t h);

void printb(size_t bin);

void printn(const char* str, size_t n);

void _printc_8(const char c);
void _printc_16(const char c);
void _printc_32(const char c);
void _printc_64(const char c);

static void (*printc)(const char) __attribute__((nonnull)) = _printc_8;