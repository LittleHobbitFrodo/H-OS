//
//  memory.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_MEMORY_H
	#ifndef H_OS_LIB_MEMORY_CPP
		#define H_OS_LIB_MEMORY_CPP


		void* mem::align(void *ptr, size_t align) {
			u64 addr = reinterpret_cast<u64>(ptr);
			return reinterpret_cast<void*>((addr + align - 1) & ~(align - 1));
		}


		constexpr size_t mem::set_bits(u16 count, bool beggining) {
			size_t ret {0};
			if (beggining) {
				for (size_t i = 0; i < count; i++) {
					ret |= 1 << i;
				}
			} else {
				for (ssize_t i = 63; i > 63-count; i--) {
					ret |= 1 << i;
				}
			}
			return ret;
		}

		void mem::set(void *ptr, size_t size, u8 value) {
			u8* p = (u8*)ptr;
			for (size_t i = 0; i < size; i++) {
				*p++ = value;
			}
		}


		template<typename T> typename type::enable<mem::is_mem_conv_t<T>::value, char>::valid inline mem::byte_t::calculate(T cnt) {
			if constexpr (type::is_same<T, size_t>::value) {
				this->count = cnt;
			} else if constexpr (type::is_same<T, mem::byte_t>::value) {
				this->count = cnt.count;
			} else if constexpr (type::is_same<T, mem::kb_t>::value) {
				this->count = cnt.count * 1024;
			} else if constexpr (type::is_same<T, mem::mb_t>::value) {
				this->count = cnt.count * 1024 * 1024;
			} else if constexpr (type::is_same<T, mem::gb_t>::value) {
				this->count = cnt.count * 1024 * 1024 * 1024;
			}
			return 0;
		}

		mem::byte_t::byte_t() {
			this->count = 0;
		}
		mem::byte_t::byte_t(size_t bytes) {
			this->count = bytes;
		}


		template<typename T, bool return_bytes> typename type::enable<mem::is_mem_conv_t<T>::value && return_bytes, size_t>::valid inline mem::kb_t::calculate(T cnt) {
			if constexpr (type::is_same<T, size_t>::value) {
				this->count = cnt / 1024;
			} else if constexpr (type::is_same<T, mem::byte_t>::value) {
				this->count = cnt.count / 1024;
				if constexpr (return_bytes) {
					return cnt.count - (this->count * 1024);
				}
			} else if constexpr (type::is_same<T, mem::kb_t>::value) {
				this->count = cnt;
			} else if constexpr (type::is_same<T, mem::mb_t>::value) {
				this->count = cnt * 1024;
			} else if constexpr (type::is_same<T, mem::gb_t>::value) {
				this->count = cnt * 1024 * 1024;
			}
			if constexpr (return_bytes) {
				return 0;
			} else {
				return;
			}
		}

		mem::kb_t::kb_t() {
			this->count = 0;
		}
		mem::kb_t::kb_t(size_t kb) {
			this->count = kb;
		}

		template<typename T, bool return_kbs> typename type::enable<mem::is_mem_conv_t<T>::value && return_kbs, mem::kb_t>::valid inline mem::mb_t::calculate(T cnt) {
			if constexpr (type::is_same<T, size_t>::value) {
				this->count = cnt / 1024 / 1024;
				if constexpr (return_kbs) {
					return cnt - (this->count * 1024 * 1024);
				}
			} else if constexpr (type::is_same<T, mem::byte_t>::value) {
				this->count = cnt.count / 1024 / 1024;
				if constexpr (return_kbs) {
					return cnt.count - (this->count * 1024 * 1024);
				}
			} else if constexpr (type::is_same<T, mem::kb_t>::value) {
				this->count = cnt.count / 1024;
				if constexpr (return_kbs) {
					return cnt.count - (this->count * 1024);
				}
			} else if constexpr (type::is_same<T, mem::mb_t>::value) {
				this->count = cnt.count;
			} else if constexpr (type::is_same<T, mem::gb_t>::value) {
				this->count = cnt.count * 1024;
			}
			if constexpr (return_kbs) {
				return 0;
			} else {
				return;
			}
		}
		mem::mb_t::mb_t() {
			this->count = 0;
		}
		mem::mb_t::mb_t(size_t mb) {
			this->count = mb;
		}

		template<typename T, bool return_mbs> typename type::enable<mem::is_mem_conv_t<T>::value && return_mbs, mem::mb_t>::valid inline mem::gb_t::calculate(T cnt) {
			if constexpr (type::is_same<T, size_t>::value) {
				this->count = cnt / 1024 / 1024 / 1024;
				if constexpr (return_mbs) {return mem::mb_t((cnt % mem::byte_t::per_gb) / mem::byte_t::per_mb);}
			} else if constexpr (type::is_same<T, mem::byte_t>::value) {
				this->count = cnt.count / 1024 / 1024 / 1024;
				if constexpr (return_mbs) {return mem::mb_t((cnt.count % mem::byte_t::per_gb) / mem::byte_t::per_mb);}
			} else if constexpr (type::is_same<T, mem::kb_t>::value) {
				this->count = cnt.count / 1024 / 1024;
				if constexpr (return_mbs) {return mem::mb_t((cnt.count % mem::kb_t::per_gb) / mem::kb_t::per_mb);}
			} else if constexpr (type::is_same<T, mem::mb_t>::value) {
				this->count = cnt.count / 1024;
				if constexpr (return_mbs) {return mem::mb_t(cnt.count % mem::mb_t::per_gb);}
			}else if constexpr (type::is_same<T, mem::gb_t>::value) {
				this->count = cnt.count;
				if constexpr (return_mbs) {
					return mem::mb_t(0);
				} else {
					return;
				}
			}
		}
		mem::gb_t::gb_t() {
			this->count = 0;
		}
		mem::gb_t::gb_t(size_t gb) {
			this->count = gb;
		}


		mem::page::table_array *mem::page::find() {
			mem::page::table_array* ret;
			asm volatile("mov %0, cr3" : "=r"(ret));
			return ret;
		}




		size_t mem::init() {


			if (mem::map::request.response == nullptr) {
				return manage::codes::mem::memmap_not_found;
			}

			//  1)		reclaim reclaimable entries
			mem::map::reclaim();

			//  2)		choose location for kernel heap
			if (!mem::heap::reserve_memory()) {
				return manage::codes::mem::cannot_allocate_memory_for_kernel_heap;
			}


			//  3)    initialize paging
			mem::page::tables = mem::page::find();
			if (mem::page::mode_request.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
				return manage::codes::mem::unsupported_paging_mode;
			}


			//  4.1)    initialize heap
			mem::heap::init();

			//	5)		create new memmap
			mem::map::parse();


			output << "\n\n\nnew memmap:" << out::endl;
			mem::map::display();

			//	6)		initialize global descriptor table
			gdt::init();
			gdt::apply();


			output << "stack ptr: " << stack::kernel_ptr << out::endl;

			manage::state++;
			return manage::codes::mem::ok;
		}


		u8 mem::map::type(u64 constant) {
			namespace t = mem::map::types;
			switch (constant) {
				case LIMINE_MEMMAP_USABLE: {
					return t::usable;
				}
				case LIMINE_MEMMAP_RESERVED: {
					return t::reserved;
				}
				case LIMINE_MEMMAP_FRAMEBUFFER: {
					return t::other;
				}
				case LIMINE_MEMMAP_BAD_MEMORY: {
					return t::bad;
				}
				case LIMINE_MEMMAP_ACPI_NVS: case LIMINE_MEMMAP_ACPI_RECLAIMABLE: {
					return t::acpi;
				}
				case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
					return t::kernel;
				}
				case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: {
					return t::reclaimable;
				}
				default: {
					return t::other;
				}
			}
			/*switch (constant) {
				case LIMINE_MEMMAP_USABLE: {
					output << ""
					return mem::map::types::usable;
				}
				case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: case LIMINE_MEMMAP_ACPI_RECLAIMABLE: {
					return mem::map::types::reclaimable;
				}
				case LIMINE_MEMMAP_RESERVED: {
					return mem::map::types::reserved;
				}
				case LIMINE_MEMMAP_ACPI_NVS: {
					return mem::map::types::acpi;
				}
				case LIMINE_MEMMAP_BAD_MEMORY: {
					return mem::map::types::bad;
				}
				case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
					return mem::map::types::kernel;
				}
				default: {
					return mem::map::types::other;
				}
			}*/
		}



		void mem::map::reclaim() {
			struct limine::limine_memmap_entry* entry;
			struct limine::limine_memmap_entry* kern = nullptr;
			u64 count = mem::map::request.response->entry_count;
			for (u64 i = 0; i < count; i++) {
				if (mem::map::request.response->entries[i] != nullptr) {
					entry = mem::map::request.response->entries[i];
					switch (entry->type) {
						case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: {
							entry->type = LIMINE_MEMMAP_USABLE;
							break;
						}
						case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
							kern = entry;
							break;
						}
						default: break;
					}
				}
			}
			if (kern == nullptr) {
				//  makes sure that kernel memmap entry exists
				panic(manage::codes::mem::cannot_locate_kernel_entry);
			}
		}


		void mem::map::display() {
			if (mem::map::custom.data() == nullptr) {
				u64 count = mem::map::request.response->entry_count;
				limine::limine_memmap_entry* ent;
				for (u64 i = 0; i < count; i++) {
					ent = mem::map::request.response->entries[i];
					if (ent == nullptr) {
						output << out::change(out::col::red) << "ERROR: memmap entry " << i+1 << " is null" << out::change() << out::endl;
						continue;
					}
					switch (ent->type) {
						case LIMINE_MEMMAP_USABLE: {
							output << out::change(out::col::green) << "usable: " << (void*)ent->base << " to " << (void*)(ent->base + ent->length) << out::endl;
							break;
						}
						case LIMINE_MEMMAP_RESERVED: {
							output << out::change(0x9a9a9a) << "reserved: " << (void*)ent->base << " to " << (void*)(ent->base + ent->length) << out::endl;
							break;
						}
						case LIMINE_MEMMAP_KERNEL_AND_MODULES: {
							output << out::change() << "kernel: " << (void*)ent->base << " to " << (void*)(ent->base + ent->length) << out::endl;
							break;
						}
						case LIMINE_MEMMAP_ACPI_RECLAIMABLE: case LIMINE_MEMMAP_ACPI_NVS: {
							output << out::change(out::col::yellow) << "ACPI: " << (void*)ent->base << " to " << (void*)(ent->base + ent->length) << out::endl;
							break;
						}
						case LIMINE_MEMMAP_FRAMEBUFFER: {
							output << out::change(out::col::blue) << "framebuffer: " << (void*)ent->base << " ot " << (void*)(ent->base + ent->length) << out::endl;
							break;
						}
						case LIMINE_MEMMAP_BAD_MEMORY: {
							output << out::change(out::col::critical) << "BAD MEMORY: " << (void*)ent->base << " to " << (void*)(ent->base + ent->length) << out::endl;
							break;
						}
						default: {
							output << out::change(out::col::critical) << "ERROR: " << (void*)ent->base << " to " << (void*)(ent->base + ent->length) << " index: " << ent->type << out::endl;
							break;
						}
					}
				}
			} else {
				output << "custom memmap:" << out::endl;
				for (u32 i = 0; i < mem::map::custom.size(); i++) {
					struct mem::map::entry& ent = mem::map::custom[i];
					switch (ent.type) {
						case mem::map::types::usable: {
							output << out::change(out::col::green) << "usable:\t\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::kernel: {
							output << out::change() << "kernel:\t\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::heap: {
							output << out::change() << "kheap:\t\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::reserved: {
							output << out::change(out::col::grey) << "reserved:\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::reclaimable: {
							output << out::change(out::col::yellow) << "reclaimable:\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::acpi: {
							output << out::change(out::col::orange) << "acpi:\t\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::bad: {
							output << out::change(out::col::critical) << "BAD MEM:\t\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::stack: {
							output << out::change(out::col::cyan) << "stack:\t\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						case mem::map::types::other: {
							output << out::change(out::col::red) << "other:\t\t"  << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << out::endl;
							break;
						}
						default: {
							output << out::change(out::col::critical) << "ERROR:\t\t" << ent.base << " to " << (void*)((size_t)ent.base + ent.len) << "\tindex: " << ent.type << out::endl;
							break;
						}
					}
				}
			}
			out::change();
		}

		mem::map::entry::entry() {
			this->base = nullptr;
			this->len = 0;
			this->type = mem::map::types::undefined;
		}


		template<typename T> safe_ptr<T>::safe_ptr() {
			this->p = nullptr;
		}

		template<typename T> safe_ptr<T>::safe_ptr(size_t bytes) {
			this->p = (T*)mem::heap::alloc(bytes);
		}

		template<typename T> void safe_ptr<T>::alloc(size_t bytes) {
			if (this->p == nullptr) {
				this->p = (T*)mem::heap::alloc(bytes);
			}
		}
		template<typename T> void safe_ptr<T>::realloc(size_t bytes, size_t add) {
			if (this->p != nullptr) {
				this->p = (T*)mem::heap::realloc(this->p, bytes, add);
			} else {
				this->p = (T*)mem::heap::alloc(bytes);
			}
		}
		template<typename T> void safe_ptr<T>::free() {
			if (this->p != nullptr) {
				mem::heap::free(this->p);
				this->p = nullptr;
			}
		}

		template<typename T> T* safe_ptr<T>::ptr() {
			return this->p;
		}

		template<typename T> safe_ptr<T>::~safe_ptr() {
			if (this->p != nullptr) {
				mem::heap::free(this->p);
			}
		}

		template<typename T> aligned_ptr<T>::aligned_ptr() {
			this->ptr_ = nullptr;
			this->align_ = 0;
			this->offset_ = 0;
		}
		template<typename T> aligned_ptr<T>::aligned_ptr(size_t bytes, u8 a) {
			this->offset_ = (this->align_ = a);
			this->ptr_ = (T*)mem::heap::alloc(bytes, offset_);
		}
		template<typename T> aligned_ptr<T>::aligned_ptr(u8 align) {
			this->offset_ = (this->align_ = align);
			this->ptr_ = nullptr;
		}
		template<typename T> aligned_ptr<T>::~aligned_ptr() {
			this->free();
		}

		template<typename T> bool aligned_ptr<T>::set_align(u8 align) {
			if (this->ptr_ == nullptr) {
				this->offset_ = (this->align_ = align);
			}
			return this->ptr_ == nullptr;
		}


		template<typename T> u8 aligned_ptr<T>::align() const {
			return this->align_;
		}
		template<typename T> T* aligned_ptr<T>::ptr() const {
			return this->ptr_;
		}
		template<typename T> u8 aligned_ptr<T>::offset() const {
			return this->offset_;
		}


		template<typename T> void aligned_ptr<T>::alloc(size_t bytes) {
			if (this->ptr_ != nullptr) {
				if constexpr (type::is_same<T, void>::value) {
					mem::heap::free((void*)((size_t)this->ptr_ - this->offset_));
				} else {
					mem::heap::free(this->ptr_ - this->offset_);
				}
			}

			if (this->align_ == 0) {
				this->offset_ = (this->align_ = 0);
				this->ptr_ = (T*)mem::heap::alloc(bytes);
			} else {
				this->offset_ = this->align_;
				this->ptr_ = (T*)mem::heap::alloc(bytes, this->offset_);
			}
		}

		template<typename T> void aligned_ptr<T>::realloc(size_t bytes, size_t add) {
			if (this->ptr_ != nullptr) {
				this->ptr_ = (T*)mem::heap::realloc(this->ptr_, bytes, add, this->offset_, this->align_);
			}
		}

		template<typename T> void aligned_ptr<T>::free() {
			if (this->ptr_ != nullptr) {
				if constexpr (type::is_same<T, void>::value) {
					mem::heap::free((void*)((size_t)this->ptr_) - this->offset_);
				} else {
					mem::heap::free(this->ptr_ - this->offset_);
				}

				this->offset_ = (this->align_ = 0);
				this->ptr_ = nullptr;
			}
		}


		void mem::map::parse() {
			//	heap must has reserved memory
				//	mem::heap::reserve_memory() must be called before this function


			namespace m = mem::map;
			limine::limine_memmap_entry** ents = mem::map::request.response->entries;
			u32 vsize = 1, msize = mem::map::request.response->entry_count;

			u8 type = MAX_U8;
			u32 heap_index = 0;

			//	locate stack
			i64 stack_index = -1;
			{
				i64 ssize = 0;
				for (i64 i = msize - 1; i >= 0; i--) {
					if (ents[i]->type == LIMINE_MEMMAP_USABLE) {
						ssize += (i64)ents[i]->length;
					} else {
						ssize = 0;
					}
					if (ssize >= stack::size::kernel + stack::size::interrupt) {
						stack_index = ++i;
						++vsize;
						break;
					}
				}
				if (stack_index < 0) {
					panic(manage::codes::mem::cannot_allocate_memory_for_stacks);
				}
			}
			vector<m::entry>& v = m::custom;


			for (u32 i = 0; i < msize; i++) {
				if (i == stack_index) {

					u64 ssize = ((stack::size::kernel + stack::size::interrupt) * mem::byte_t::per_kb);

					stack_index = v.size() - 1;
					//v[stack_index].len = ents[i]->base + ents[i]->length - ((stack::size::interrupt + stack::size::kernel) * mem::byte_t::per_kb);
					v[stack_index].len = (ents[i]->base - (u64)v[stack_index].base) - ssize;

					v.pushn(m::entry());

					m::entry& e = v[v.size() - 1];
					e.type = m::types::stack;
					stack::kernel_ptr = (e.base = (void*)((size_t)v[stack_index].base + v[stack_index].len));
					e.len = ssize;
					stack::interrupt_ptr = (void*)((size_t)stack::kernel_ptr - (stack::size::kernel * mem::byte_t::per_kb));
				} else if (ents[i]->base == (u64)mem::heap::start) {
					//output << "heap\n";
					heap_index = i;
					//	heap entry
					v.pushn(m::entry());
					v[v.size() - 1].base = (void*)ents[i]->base;
					v[v.size() - 1].len = mem::heap::minimum_memmap_entry_size_in_kb * mem::byte_t::per_kb;
					v[v.size() - 1].type = m::types::heap;

					//	next entry
					v.pushn(m::entry());
					v[v.size() - 1].base = (void*)((u64)v[v.size() - 2].base + v[v.size() - 2].len);
					v[v.size() - 1].type = type = m::type(ents[i]->type);
					type = m::type(ents[i]->type);
					continue;
				}
				if (type != m::type(ents[i]->type)) {
					v[v.size() - 1].len = ents[i-1]->base + ents[i-1]->length - (u64)v[v.size() - 1].base;
					type = m::type(ents[i]->type);
					v.pushn(m::entry());
					v[v.size() - 1].base = (void*)ents[i]->base;
					v[v.size() - 1].len = ents[i]->length;
					v[v.size() - 1].type = type;
				}
				output << out::endl;
			}
		}



	#else
		#warning memory.cpp already included
	#endif
#else
	#error memory.cpp: memory.h not included
#endif