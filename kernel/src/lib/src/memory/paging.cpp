//
//  memory/paging.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_MEMORY_PAGING_H
	#ifndef H_OS_LIB_MEMORY_PAGING_CPP
		#define H_OS_LIB_MEMORY_PAGING_CPP



		inline void* mem::page::entry::address() {
			return (void*)((this->bits >> 12) | mem::set_bits(48));
		}
		inline void mem::page::entry::address(void* ptr) {
			this->bits &= ~0x000ffffffffff000;
			this->bits |= (((size_t)ptr << mem::page::entry::flags::address_shift) & 0x000ffffffffff000);
		}

		inline u64 mem::page::entry::flags() {
			return this->bits & (0x8ff | ((u64)1 << (u64)63));
		}

		inline void mem::page::entry::unset(u64 bts) {
			this->bits &= ~bts;
		}


		void mem::page::table_array::clean() {
			size_t* ptr = (size_t*)this;
			for (size_t i = 0; i < sizeof(mem::page::table_array) / sizeof(size_t); i++) {
				ptr[i] = 0;
			}
		}

		void mem::page::table_array::init() {
			struct mem::page::entry* ent = (struct mem::page::entry*)&mem::page::tables;
			for (u32 i = 0; i < 512 * 4; i++) {
				ent[i].address((void*)((size_t)i * 0x100));
				ent[i].bits |= mem::page::entry::flags::write | mem::page::entry::flags::present;
			}
		}

	#else
		#warning memory/paging.cpp already included
	#endif
#else
	#error memory/paging.cpp: memory/paging.h not included
#endif