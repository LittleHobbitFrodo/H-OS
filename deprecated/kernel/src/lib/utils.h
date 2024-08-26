//
//  utils.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_UTILS_H
	#define H_OS_LIB_UTILS_H

	void memcpy(void* src, void* dest, size_t size);

	namespace utilstr {

		namespace comptime {
			constexpr size_t len(const char* str);
		}

		inline size_t len(const char* str);

		inline void reverse(char* str, size_t len);
		bool cmp(const char* s1, const char* s2);

	}

	void halt();


	#ifndef KERNEL_VERSION
		#define KERNEL_VERSION "0.0.0.1"
	#endif

#else
	#warning utils.h already included
#endif