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

		bool utilstr::cmp(const char *s1, const char *s2) {
			u32 l = utilstr::len(s1);
			if (l != utilstr::len(s2)) {
				return false;
			}
			output << "\t\ts1: " << s1 << out::endl;
			output << "\t\ts2: " << s2 << out::endl;
			output << "\t\tl: " << l << out::endl;
			halt();

			for (u32 i = 0; i < l; i++) {
				if (s1[i] != s2[i]) {
					return false;
				}
			}
			return true;
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