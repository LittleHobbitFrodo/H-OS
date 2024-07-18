//
//	segmentation.cpp
//		part of the H-OS project
//

#ifdef H_OS_LIB_SEGMENTATION_H
	#ifndef H_OS_LIB_SEGMENTATION_CPP
		#define H_OS_LIB_SEGMENTATION_CPP

		void segment::set(u16 &segvar, u16 index, u8 data) {
			segvar = ((index << 3) & segment::bits::index) | (data & segment::bits::data);
		}

		[[maybe_unused]] [[deprecated]] void segment::load_cs(u16 cs) {
			//	deprecated function should not be used
			struct fp {
				u64 offset;
				u16 seg;
			} __attribute__((packed));

			struct fp f = {(u64)&&next_instruction, cs};

			asm volatile("ljmp %0\n" :: "m"(f));
			next_instruction:
			asm volatile("nop");

		}



	#else
		#warning segmentation.cpp already included
	#endif
#else
	#error segmentation.cpp: segmentation.h not included
#endif