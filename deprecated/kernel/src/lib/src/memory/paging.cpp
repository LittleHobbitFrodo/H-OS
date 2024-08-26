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

		bool mem::page::entry::executable() {
			return (this->bits & mem::page::entry::flags::no_exec) == 0;
		}

		bool mem::page::entry::present() {
			return (this->bits & 1) != 0;
		}

		mem::page::entry* mem::page::find() {
			mem::page::entry* ret;
			asm volatile("mov %0, cr3" : "=r"(ret));
			return ret;
		}

		void mem::page::use() {
			asm volatile("cli");
			asm("mov %%cr3, %0" :: "r"(mem::page::custom.ptr()) : "memory");
			asm volatile("sti");
		}

		template<typename T> void mem::page::use(aligned_ptr<T> *ptr) {
			asm volatile("cli");
			asm("mov %%cr3, %0" :: "r"(ptr->ptr()) : "memory");
			asm volatile("sti");
		}



		void mem::page::init() {

			//	set physical and virtual base address
			mem::base::physical = (void*)mem::address.response->physical_base;
			mem::base::virtual_ = (void*)mem::address.response->virtual_base;
			mem::address.response = nullptr;

			//	heap needs to be initialized
			{
				//	allocates page table (aligned to 4096)
				size_t align = 4096;
				//auto* ptr = (mem::page::entry*)mem::heap::alloc(mem::page::count * sizeof(struct mem::page::entry), align);
				auto ptr = (mem::page::entry*)mem::heap::enlarge(mem::page::count * sizeof(mem::page::entry), align);
				mem::page::custom.take_over(ptr, 4096,align);
				//	TODO (optional) eliminate unused memory
			}
			//	sets data from old page table to new
			mem::page::entry* old = mem::page::find();
			mem::page::entry* p = mem::page::custom.ptr();
			for (u32 i = 0; i < mem::page::count; i++) {
				p[i].bits = old[i].bits;
			}
		}

	#else
		#warning memory/paging.cpp already included
	#endif
#else
	#error memory/paging.cpp: memory/paging.h not included
#endif