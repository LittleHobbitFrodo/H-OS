//
//  utils.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_UTILS_H
	#ifndef H_OS_LIB_UTILS_CPP
		#define H_OS_LIB_UTILS_CPP

		constexpr size_t utilstr::comptime::len(const char *str) {
			size_t i = 0;
			for (; str[i] != '\0'; i++);
			return ++i;
		}

		//  memcmp, memcpy, memset

		void memcpy(void* src, void* dest, size_t size) {
			//  optimize?
			for (size_t i = 0; i < size; i++) {
				*((u8*)src) = *((u8*)dest);
			}
		}

		inline size_t utilstr::len(const char* str) {
			size_t i = 0;
			for (; str[i] != '\0'; i++);
			return ++i;
		}

		inline void utilstr::reverse(char* str, size_t len) {
			size_t start = 0;
			size_t end = len-1;
			char tmp;
			for (;start < end; start++) {
				tmp = str[start];
				str[start] = str[end];
				str[end] = tmp;
				end--;
			}
		}

		void halt() {
			asm("cli");
			while (true) {
				asm("hlt");
			}
		}


	#else
		#warning utils.cpp already included
	#endif
#else
	#error utils.cpp: utils.h not included
#endif