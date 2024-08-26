//
//	utils.h
//		part of the CORE kernel belonging to the H-OS project
//

#ifndef H_OS_LIB_UTILS_H
	#define H_OS_LIB_UTILS_H


	i8 strcmp(const char* s1, const char* s2);

	[[nodiscard]] __attribute__((always_inline)) inline size_t strlen(const char* s) {
		size_t i = 0;
		for (; s[i] != '\0'; i++);
		++i;
		return i;
	}

	__attribute__((always_inline)) inline bool strcmpb(const char* s1, const char* s2) {
		size_t l = strlen(s1);
		bool r = true;
		if (l == strlen(s2)) {
			for (size_t i = 0; i < l; i++) {
				if (s1[i] != s2[i]) {
					r = false;
					break;
				}
			}
		} else {
			r = false;
		}
		return r;
	}

	__attribute__((always_inline)) inline void memcpy(void* src, void* dest, size_t size) {
		for (size_t i = 0; i < size; i++) {
			((u8*)dest)[i] = ((u8*)src)[i];
		}
	}

	__attribute__((always_inline)) inline void memset(void* ptr, size_t size, u8 val) {
		for (size_t i = 0; i < size; i++) {
			((u8*)ptr)[i] = val;
		}
	}

	__attribute__((always_inline)) inline void strrev(char* str, size_t len) {
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

	extern void hang();
	extern void halt();

#else
	#warning utils.h already included
#endif