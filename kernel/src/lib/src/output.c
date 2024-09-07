//
//	output.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_OUTPUT_H
	#ifndef H_OS_LIB_OUTPUT_C
		#define H_OS_LIB_OUTPUT_C


		//	initialization functions
		void screen_init() {

			//	initialize screen structure
			screen.count = 0;
			screen.address = null;
			screen.bpp = 0;
			screen.h = 0;
			screen.w = 0;

			//	initialize colors
			col.white = 0xffffff;
			col.green = 0x88ff88;
			col.red = 0xff8888;
			col.blue = 0x8888ff;
			col.critical = 0xff0000;

			col.yellow = 0xffff00;
			col.orange = 0xffa500;
			col.cyan = 0x00ffff;
			col.grey = 0xaaaaaa;

			if (fb.response == null) {
				return;
			}

			if (fb.response->framebuffer_count < 1) {
				return;
			}

			struct limine_framebuffer* f = fb.response->framebuffers[0];

			if (f->memory_model != LIMINE_FRAMEBUFFER_RGB) {
				return;
			}

			//	set screen data
			screen.count = fb.response->framebuffer_count;
			screen.bpp = f->bpp;
			screen.address = (u32*)f->address;
			screen.h = f->height;
			screen.w = f->width;

		}
		void output_init() {
			output.line = (output.column = 0);
			output.space_between_lines = SPACE_BETWEEN_LINES_DEFAULT;
			output.fb = 0;
			output.color = col.white;
		}


		//	output functions
		void print(const char* s) {
			for (size_t i = 0; s[i] != '\0'; i++) {
				printc(s[i]);
			}
		}

		void printl(const char* s) {
			for (size_t i = 0; s[i] != '\0'; i++) {
				printc(s[i]);
			}
			endl();
		}

		void prints(const string* s) {
			for (size_t i = 0; i < s->size; i++) {
				printc(s->data[i]);
			}
		}

		void printsl(const string* s) {
			for (size_t i = 0; i < s->size; i++) {
				printc(s->data[i]);
			}
			endl();
		}


		void printi(const i64 i) {
			char num[INTLEN_I64];
			to_stringi((char*)&num, i);
			print((const char*)&num);
		}

		void printu(const u64 u) {
			char num[INTLEN_U64];
			to_string((char*)&num, u);
			print((const char*)&num);
		}

		void printp(void* p) {
			char num[HEXLEN_PTR];
			to_hex((char*)num, p);
			print((const char*)&num);
		}

		void printb(size_t bin) {
			for (ssize_t i = (sizeof(size_t) * 8) - 1; i >= 0; i--) {
				printc('0' + ((bin >> i) & 1));
			}
		}



	#endif
	//	#warning output.c already included
	//#endif
#else
	#error output.c: output.h not included
#endif