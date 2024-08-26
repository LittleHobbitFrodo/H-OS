//
//	utils.c
//		part of the CORE kernel belonging to the H-OS project
//

#ifdef H_OS_LIB_UTILS_H
	#ifndef H_OS_LIB_UTILS_C
		#define H_OS_LIB_UTILS_C

		i8 strcmp(const char* s1, const char* s2) {
			size_t l1 = strlen(s1);
			size_t l2 = strlen(s2);
			if (l1 < l2) {
				return -1;
			} else if (l1 > l2) {
				return 1;
			}
			for (size_t i = 0; i < l1; i++) {
				if (s1[i] != s2[i]) {
					return (i8)(((s1[i] > s2[i]) * 2) - 1);
					//  dt[i] > -> 2 - 1 = 1
					//  dt[i] < -> 0 - 1 = -1
				}
			}
			return 0;
		}

	#else
		#warning utils.c already included
	#endif
#else
	#error utils.c: utils.h not included
#endif