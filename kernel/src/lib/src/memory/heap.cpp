//
//  memory/heap.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_MEMORY_HEAP_H
	#ifndef H_OS_LIB_MEMORY_HEAP_CPP
		#define H_OS_LIB_MEMORY_HEAP_CPP

		mem::heap::segment::segment() {
			this->next = nullptr;
			this->used = 0;
			this->size = 0;
		}

		mem::heap::segment::segment(mem::heap::segment& seg) {
			this->next = seg.next;
			this->used = seg.used;
			this->size = seg.size;
		}



		bool mem::heap::reserve_memory() {
			struct limine::limine_memmap_entry *ent, *mixed_start = nullptr;
			u32 count = mem::map::request.response->entry_count;
			u64 mixed_len = 0;

			for (u32 i = 0; i < count; i++) {
				ent = mem::map::request.response->entries[i];
				if (ent == nullptr) {
					output << out::change(out::col::red) << "ERROR: memmap entry " << i + 1 << " is null" << out::endl;
					continue;
				}
				if (ent->type == LIMINE_MEMMAP_USABLE) {
					mixed_len += ent->length;
					if (mixed_start == nullptr) {
						mixed_start = (limine::limine_memmap_entry *) ent;
					}
				} else {
					if (mixed_len >= mem::heap::minimum_memmap_entry_size_in_kb * mem::byte_t::per_kb) {
						mem::heap::start = (struct mem::heap::segment *) mixed_start->base;
						mem::heap::end_physical = (void*) (ent->base + ent->length);
						return true;
					} else {
						mixed_start = nullptr;
						mixed_len = 0;
					}
				}
			}
			return false;
		}

		void mem::heap::init() {
			mem::heap::start->used = 0;
			mem::heap::start->next = nullptr;
			mem::heap::start->size = mem::heap::initial_block_size;
			mem::heap::end = (mem::heap::used_until = mem::heap::start);

			//  end is used for faster expanding of heap
			//  it is set to the last segment header structure (in initialization is set to same position as start)
		}

		void* mem::heap::alloc(size_t bytes) {
			struct mem::heap::segment* cur = mem::heap::used_until;
			struct mem::heap::segment* con = nullptr;
			u32 c_size = 0;
			//  find free block
			for (;true; cur = cur->next) {
				if (cur->used) {
					con = nullptr;
					c_size = 0;
				} else {
					if (cur->size >= bytes) {
						//	big enough -> return
						cur->used = true;
						if (mem::heap::used_until < cur) {
							mem::heap::used_until = cur;
						}
						if (cur->size > bytes * 2) {
							auto ret = mem::heap::divide_block(cur, bytes);
							ret->used = true;
							return ret;
						}
						return cur + sizeof(struct mem::heap::segment);
					} else if (con == nullptr) {
						//	set start of block connection
						con = cur;
						c_size = cur->size;
					} else if (c_size >= bytes) {
						//	connect and return
						con->used = true;
						con->size = c_size;
						con->next = cur;
						return con + sizeof (mem::heap::segment);
					} else {
						//	increment size
						c_size += cur->size;
					}
				}
				if (cur->next == nullptr) {
					break;
				}
			}
			if (c_size >= bytes) {
				//	if c_size > 0 -> con != null
				//	connect and return
				con->next = cur;
				con->size = c_size;
				con->used = true;
				return con + sizeof(struct mem::heap::segment);
			}
			return mem::heap::expand(bytes);
		}

		void* mem::heap::alloc(size_t bytes, u8& align) {
			struct mem::heap::segment* cur = mem::heap::used_until;
			struct mem::heap::segment* con = nullptr;

			if (align == 0) {
				return mem::heap::alloc(bytes);
			}

			u32 c_size = 0;
			size_t offset = 0;	//	space for align
			align &= 0b01111111;
			size_t c_space = 0;

			//  find free block
			for (;true; cur = cur->next) {
				if (cur->used) {
					con = nullptr;
					c_size = 0;
				} else {
					//	calculate offset

					offset = ((size_t)mem::align((void*)((size_t)cur + sizeof(struct mem::heap::segment)), align) - (size_t)cur);

					if (cur->size >= bytes) {
						//	big enough -> return
						cur->used = true;
						if (mem::heap::used_until < cur) {
							mem::heap::used_until = cur;
						}
						//output << out::change(out::col::blue) << "block address: " << cur << out::change() << out::endl;
						align = offset;
						if (cur->size < (bytes * 2)) {
							auto ret = mem::heap::divide_block(cur, bytes + offset);
							ret->used = true;
							return ret;
						}
						return cur + sizeof(struct mem::heap::segment) + offset;
					} else if (con == nullptr) {
						//	set start of block connection
						con = cur;
						c_size = cur->size;
						c_space = (size_t)cur + sizeof(struct mem::heap::segment);
						c_space = (align - (c_space % align) % align);
						c_space -= (size_t)cur + sizeof(struct mem::heap::segment);
					} else if (c_size >= bytes + c_space) {
						//	connect and return
						con->used = true;
						con->size = c_size+c_space;
						con->next = cur;
						align = offset;
						return con + sizeof (mem::heap::segment) + c_space;
					} else {
						//	increment size
						c_size += cur->size;
					}
				}
				if (cur->next == nullptr) {
					break;
				}
			}
			if (c_size >= bytes + c_space) {
				//	if c_size > 0 -> con != null
				//	connect and return
				con->next = cur;
				con->size = c_size;
				con->used = true;
				return con + sizeof(struct mem::heap::segment) + c_space;
			}
			return mem::heap::expand(bytes + offset);
		}

		inline void* mem::heap::expand(size_t bytes) {
			struct mem::heap::segment* cur = mem::heap::end;
			mem::heap::end += sizeof(struct mem::heap::segment) + cur->size;
			cur->next = mem::heap::end;
			mem::heap::end->used = true;
			mem::heap::end->size = bytes;
			mem::heap::end->next = nullptr;
			return mem::heap::end + sizeof(struct mem::heap::segment);
		}

		void mem::heap::free(void *ptr) {
			//  must be aligned exactly at the beginning of the block
			struct mem::heap::segment* seg = (struct mem::heap::segment*)ptr - sizeof(mem::heap::segment);
			seg->used = false;
			if (seg < mem::heap::used_until) {
				mem::heap::used_until = seg;
			}
		}

		void* mem::heap::realloc(void *ptr, size_t bytes) {
			struct mem::heap::segment* seg = (struct mem::heap::segment*)ptr - sizeof(struct mem::heap::segment);
			if (seg->size >= bytes) {
				return ptr;
			}
			u8* data = (u8*)mem::heap::alloc(bytes);
			for (u32 i = 0; i < bytes; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void* mem::heap::realloc(void *ptr, size_t bytes, size_t add) {
			struct mem::heap::segment* seg = (struct mem::heap::segment*)ptr - sizeof(struct mem::heap::segment);
			if (seg->size >= bytes) {
				return ptr;
			}
			u8* data = (u8*)mem::heap::alloc(bytes + add);
			for (u32 i = 0; i < bytes; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void *mem::heap::realloc(void *ptr, size_t bytes, size_t add, u8& offset, u8 original_align) {
			struct mem::heap::segment* seg = (struct mem::heap::segment*)ptr - offset;
			if (seg->size >= bytes + offset) {
				return ptr;
			}
			u8* data = (u8*)mem::heap::alloc(bytes + add, original_align);
			for (u32 i = 0; i < bytes + add; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			offset = original_align;
			return data;
		}


		inline u32 mem::heap::block_size(void *ptr) {
			return ((struct mem::heap::segment*)ptr - sizeof(struct mem::heap::segment))->size;
		}


		void mem::heap::debug() {
			u32 col = out::color;
			out::color = out::col::red;
			struct mem::heap::segment* seg = mem::heap::start;
			for (u32 i = 0;; i++) {
				output << i+1 << ":\tused:\t" << seg->used << "\n\tsize:\t" << seg->size << "\n\tnext: " << seg->next << out::endl;
				if (seg->next == nullptr) {
					break;
				}
				seg = seg->next;
			}
			out::color = col;
		}


		size_t mem::heap::size() {
			return ((size_t)mem::heap::end_physical - (size_t)mem::heap::start);
		}


		struct mem::heap::segment* mem::heap::divide_block(mem::heap::segment *seg, size_t size) {
			if (seg->size >= size + sizeof(struct mem::heap::segment) + mem::heap::divide_min_bytes) {
				size_t rsize = seg->size - sizeof(struct mem::heap::segment);
				seg->size = size;
				struct mem::heap::segment* nw = seg + seg->size;
				nw->next = seg->next;
				seg->next = nw;
				nw->used = false;
				nw->size = rsize - size;
				return nw;
			}
			return seg;
		}




	#else
		#warning memory/heap.cpp already included
	#endif
#else
	#error memory/heap.cpp: memory/heap.h not included
#endif