//
//  interrupts/interrupts.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_INTERRUPTS_H
	#ifndef H_OS_LIB_INTERRUPTS_INTERRUPTS_CPP
		#define H_OS_LIB_INTERRUPTS_INTERRUPTS_CPP


		interrupt::entry::entry() {
			this->off0 = 0;
			this->selector = 0;
			this->ist_ = 0;
			this->typeattr = 0;
			this->off1 = 0;
			this->off2 = 0;
			this->reserved = 0;
		}

		void interrupt::entry::set_function(void *fun) {
			size_t f = (size_t)fun;
			this->off0 = (u16)f;
			this->off1 = (u16)(f >> 16);
			this->off2 = (u32)(f >> 32);
		}

		void interrupt::entry::set_segment(u16 seg) {
			this->selector = seg;
		}

		void interrupt::entry::set_ist(u8 i) {
			this->ist_ = i;
		}

		void interrupt::entry::set_attribute(u8 attr) {
			this->typeattr = attr;
		}



		interrupt::entry::func_t interrupt::entry::function() {
			size_t ret = (size_t)this->off2 << 32;
			ret |= (size_t)this->off0 & 0xffff;
			ret |= (size_t)(this->off1 << 16) & 0xffff0000;
			return (interrupt::entry::func_t)(void*)ret;
		}

		u16 interrupt::entry::segment() {
			return this->selector;
		}

		u8 interrupt::entry::ist() {
			return this->ist_;
		}

		u8 interrupt::entry::attribute() {
			return this->typeattr;
		}

	#else
		#warning interrupts/interrupts.cpp already included
	#endif
#else
	#error interrupts/interrupts.cpp: interrupts/interrupts.h not included
#endif