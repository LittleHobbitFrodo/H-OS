//
//  interrupts.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_INTERRUPTS_H
	#define H_OS_LIB_INTERRUPTS_H

	namespace interrupt {

		typedef struct frame {

			//  this information is pushed into stack before calling interrupt handler

			void* instruction;
				//  64bit pointer (instruction pointer)
			u64 cs;
				//  code segment register
			u64 flags;
			void* stack;
				//  stack pointer
			u64 ss;
				//  the ss register
		} __attribute__((packed)) frame;

		typedef struct framerr {
			//  pushed to stack when interrupt is error

			u64 errcode;
			void* instruction;
			u64 cs;
			u64 flags;
			void* stack;
			u64 ss;

		} __attribute__((packed)) framerr;

		typedef struct entry {
			u16 off0;
			u16 selector;
			u8 ist_;
			u8 typeattr;
			u16 off1;
			u32 off2;
			u32 reserved;

			[[maybe_unused]] entry();
			inline void set_function(void* fun);
			inline void set_segment(u16 seg);
			//void set_code(u16 index, u8 data);
			inline void set_ist(u8 i);
			inline void set_attribute(u8 attr);

			typedef void (*func_t);

			inline func_t function();
			inline u16 segment();
			inline u8 ist();
			inline u8 attribute();


		} __attribute__((packed));

	}

#else
	#warning interrupts.h already included
#endif


//	location of IDT is stored in idt register
//		loaded by lidt instruction (takes pointer as argument)

//	what does IDT entry contain?
//		offset -> interrupt handler function pointer (what function to execute)
//		selector -> code segmet selector (GDT, LDT)
//		IST
//		reserved -> not used, is here to align each entry, set to 0


//	idt gate structure:
//		bits	purpose
//		16		first part of offset
//		16		code segment
//		8		IST (explained below)
//		8		type and attribute
//		16		second part of offset
//		32		third part of offset
//		32		reserved
