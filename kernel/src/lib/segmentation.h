//
//	segmentation.h
//		part of the H-OS project
//

#ifndef H_OS_LIB_SEGMENTATION_H
	#define H_OS_LIB_SEGMENTATION_H

	namespace segment {
		namespace bits {
			const static u16 index {0xfff8};
			const static u16 table {1 << 2};
			const static u16 privilege {1 | 1 << 1};

			const static u8 data {0b111};
		}
		inline void set(u16& segvar, u16 index, u8 data);

		inline void load_cs(u16 cs);

		const static u16 gdt {0};
		const static u16 ldt {1 << 2};

		const static u8 (&privilege)[4] {gdt::access::privilege};

	}

#else
	#warning segmentation.h already included
#endif