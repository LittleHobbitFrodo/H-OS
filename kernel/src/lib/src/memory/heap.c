//
//	memory/heap.c
//		part of the CORE kernel belonging to the H-OS project
//

#ifdef H_OS_LIB_MEMORY_HEAP_H
	#ifndef H_OS_LIB_MEMORY_HEAP_C
		#define H_OS_LIB_MEMORY_HEAP_C

		bool heap_reserve_memory(bool include_reclaimable_entries) {

			struct limine_memmap_entry* mstart = null;
			size_t mlen = 0;

			size_t size = memmap_req.response->entry_count;
			struct limine_memmap_entry* ent = null;

			if (include_reclaimable_entries) {
				for (size_t i = 0; i < size; i++) {
					ent = memmap_req.response->entries[i];
					if (ent == null) {
						output.color = col.critical;
						print("ERROR");
						output.color = col.white;
						print(":\tmemory map entry ");
						printu(i);
						printl(" is NULL");
						continue;
					}
					if ((ent->type == LIMINE_MEMMAP_USABLE) || (ent->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)) {
						if (mstart == null) {
							mstart = ent;
						}
						mlen += ent->length;
					} else {
						mstart = null;
						mlen = 0;
					}
					if (mlen >= HEAP_MINIMAL_ENTRY_SIZE * KB) {
						//	mlen != 0 => mstart != null
						heap_start = (heap_segment*)mstart->base;
						heap_end_physical = (void*)((mstart->base + mlen));
						return true;
					}
				}
			} else {
				for (size_t i = 0; i < size; i++) {
					ent = memmap_req.response->entries[i];
					if (ent == null) {
						output.color = col.critical;
						print("ERROR");
						output.color = col.white;
						print(":\tmemory map entry ");
						printu(i);
						printl(" is NULL");
						continue;
					}
					if (ent->type == LIMINE_MEMMAP_USABLE) {
						if (mstart == null) {
							mstart = ent;
						}
						mlen += ent->length;
					} else {
						mstart = null;
						mlen = 0;
					}
					if (mlen >= HEAP_MINIMAL_ENTRY_SIZE * KB) {
						//	mlen != 0 => mstart != null
						heap_start = (heap_segment*)mstart->base;
						heap_end_physical = (void*)((mstart->base + mlen));
						return true;
					}
				}
			}
			return false;
		}

		void heap_init() {
			//	heap_reserve_memory() must be called before this function
			heap_start->next = null;
			heap_start->used = false;
			heap_start->size = HEAP_INITIAL_BLOCK_SIZE;
			heap_end = (heap_used_until = heap_start);
		}


		void* heap_expand(size_t bytes) {
			struct heap_segment* last = heap_end;
			heap_end = (struct heap_segment*)((size_t)heap_end + sizeof(heap_segment) + last->size);
			last->next = heap_end;
			heap_end->used = true;
			heap_end->size = bytes;
			heap_end->next = null;
			return (void*)((size_t)heap_end + sizeof(heap_segment));
		}

		void* heap_enlarge(size_t bytes) {
			if (!heap_end->used) {
				heap_end->size = bytes;
				heap_end->next = null;
				heap_end->used = true;
				return (void*)((size_t)heap_end + sizeof(heap_segment));
			}
			return heap_expand(bytes);
		}

		void heap_connect(heap_segment* beg, heap_segment* end) {
			if ((size_t)beg < (size_t)end) {
				beg->next = end;
				beg->size = ((size_t)end - sizeof(heap_segment) - (size_t)beg);
			}
		}

		heap_segment* heap_divide(heap_segment* seg, size_t size) {
			if (seg->size > size + sizeof(heap_segment) + 8) {
				size_t rsize = seg->size - sizeof(heap_segment);
				seg->size = size;
				heap_segment* nw = (heap_segment*)((size_t)seg + sizeof(heap_segment) + size);
				nw->next = seg->next;
				seg->next = nw;
				nw->used = false;
				nw->size = rsize - size;
				return nw;
			}
			return seg;
		}





		void* alloc(size_t bytes) {
			struct heap_segment* i = heap_used_until;
			struct heap_segment* con = null;
			size_t c_size = 0;

			for (;; i = i->next) {

				if (i->used) {
					c_size = 0;
					con = null;
				} else {
					if (i->size >= bytes) {
						i->used = true;
						if ((size_t)heap_used_until < (size_t)i) {
							heap_used_until = i;
						}
						if (i->size > bytes + sizeof(heap_segment) + 8) {
							heap_divide(i, bytes);
						}
						return (void*)((size_t)i + sizeof(heap_segment));
					} else if (con == null) {
						con = i;
						c_size = i->size;
					} else if (c_size > bytes) {
						heap_connect(con, i);
						con->used = true;
						return (void*)((size_t)con + sizeof(heap_segment));
					}
				}

				if (i->next == null) {
					break;
				}
			}
			return heap_enlarge(bytes);
		}

		void* realloc(void* ptr, size_t bytes) {
			heap_segment* seg = (heap_segment*)((size_t)ptr - sizeof(heap_segment));
			if (seg->size >= bytes) {
				if (seg->size > bytes + sizeof(heap_segment) + 8) {
					heap_divide(seg, bytes);
				}
				return ptr;
			}
			if (unlikely((size_t)heap_end == (size_t)seg)) {
				seg->size = bytes;
				return ptr;
			}
			u8* data = alloc(bytes);
			size_t bs = heap_bsize(ptr);
			size_t size = (bytes < bs)? bytes : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void* realloca(void* ptr, size_t bytes, size_t add) {
			heap_segment* seg = (heap_segment*)((size_t)ptr - sizeof(heap_segment));
			if (seg->size >= bytes) {
				return ptr;
			}
			if ((size_t)heap_end == (size_t)seg) {
				seg->size = bytes;
				return ptr;
			}
			u8* data = alloc(bytes + add);
			size_t bs = heap_bsize(ptr);
			size_t size = (bytes + add < bs)? bytes + add : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void heap_debug() {
			u32 c = output.color;
			heap_segment* i = heap_start;
			output.color = col.cyan;
			printl("heap scheme: ");
			size_t ii = 0;
			for (;; i = i->next) {
				output.color = col.white;
				printu(ii); print("\t");
				output.color = col.cyan;
				printp(i);
				print("\tnext("); printp(i->next); print("):\tsize(");
				printu(i->size); print("):\t");
				printl((i->used)? "used" : "free");
				if (i->next == null) {
					break;
				}
				++ii;
			}
			endl();
			output.color = c;
		}

		void* align_alloc(size_t bytes, size_t* align_)  {
			struct heap_segment* i = heap_used_until;
			struct heap_segment* con = null;
			size_t c_size = 0;
			void* ptr;
			size_t offset = 0, c_offset = 0;

			for (;; i = i->next) {

				if (i->used) {
					c_size = 0;
					con = null;
				} else {
					ptr = align((void*)((size_t)i + sizeof(heap_segment)), *align_);
					offset = (size_t)ptr - ((size_t)i + sizeof(heap_segment));
					if (i->size >= bytes + offset) {
						//	block big enough
						i->used = true;
						if ((size_t)heap_used_until < (size_t)i) {
							heap_used_until = i;
						}
						if (i->size > bytes + offset + sizeof(heap_segment) + 8) {
							heap_divide(i, bytes + offset);
						}
						*align_ = offset;
						return ptr;
					} else if (con == null) {
						con = i;
						c_size = i->size;
						c_offset = (size_t)align((void*)((size_t)con + sizeof(heap_segment)), *align_) - ((size_t)i + sizeof(heap_segment));
					} else if (c_size > bytes + c_offset) {
						heap_connect(con, i);
						con->used = true;
						return (void*)((size_t)con + sizeof(heap_segment) + c_offset);
					}
				}

				if (i->next == null) {
					break;
				}
			}
			return heap_align_enlarge(bytes, align_);
		}

		void* align_alloco(size_t bytes, size_t* align_)  {
			struct heap_segment* i = heap_used_until;
			struct heap_segment* con = null;
			size_t c_size = 0;
			void* ptr;
			size_t offset = 0, c_offset = 0;

			for (;; i = i->next) {
				if (i->used) {
					c_size = 0;
					con = null;
				} else {
					ptr = align((void*)((size_t)i + sizeof(heap_segment)), *align_);
					offset = (size_t)ptr - ((size_t)i + sizeof(heap_segment));
					if (i->size >= bytes + offset) {
						//	block big enough
						i->used = true;
						if ((size_t)heap_used_until < (size_t)i) {
							heap_used_until = i;
						}
						if (i->size > bytes + offset + sizeof(heap_segment) + 8) {
							heap_divide(i, bytes + offset);
						}
						if (offset > sizeof(heap_segment) + 8) {
							heap_segment* next = i->next;
							heap_segment* nseg = (heap_segment*)((size_t)i + offset);
							i->next = nseg;
							i->size = (size_t)nseg - (size_t)i - sizeof(heap_segment);
							nseg->next = next;
							nseg->size = (size_t)next - (size_t)nseg - sizeof(heap_segment);
							nseg->used = true;
							*align_ = 0;
						} else {
							*align_ = offset;
						}
						return ptr;
					} else if (con == null) {
						con = i;
						c_size = i->size;
						c_offset = (size_t)align((void*)((size_t)con + sizeof(heap_segment)), *align_) - ((size_t)i + sizeof(heap_segment));
					} else if (c_size > bytes + c_offset) {
						heap_connect(con, i);
						con->used = true;
						if (c_offset > sizeof(heap_segment) + 8) {
							heap_segment* next = i->next;
							heap_segment* nseg = (heap_segment*)((size_t)i + c_offset);
							i->next = nseg;
							i->size = (size_t)nseg - (size_t)i - sizeof(heap_segment);
							nseg->next = next;
							nseg->size = (size_t)next - (size_t)nseg - sizeof(heap_segment);
							nseg->used = true;
							*align_ = 0;
						} else {
							*align_ = c_offset;
						}
						return (void*)((size_t)con + sizeof(heap_segment) + c_offset);
					}
				}

				if (i->next == null) {
					break;
				}
			}
			return heap_align_enlargeo(bytes, align_);
		}

		void* align_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) {
			heap_segment* seg = (heap_segment*)((size_t)ptr - sizeof(heap_segment) - *offset);
			if (seg->size >= bytes + *offset) {
				if (seg->size > bytes + sizeof(heap_segment) + 8) {
					heap_divide(seg, bytes + *offset);
				}
				return ptr;
			}
			if ((size_t)heap_end == (size_t)seg) {
				seg->size = bytes + *offset;
				return ptr;
			}
			size_t original_offset = *offset;
			*offset = align;
			u8* data = align_alloc(bytes, offset);
			size_t bs = heap_bsize((void*)((size_t)ptr - original_offset)) - original_offset;
			size_t size = (bytes < bs)? bytes : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void* align_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*)) {
			heap_segment* seg = (heap_segment*)((size_t)ptr - sizeof(heap_segment) - *offset);
			if (seg->size >= bytes + *offset) {
				if (seg->size > bytes + sizeof(heap_segment) + 8) {
					heap_divide(seg, bytes + *offset);
				}
				return ptr;
			}
			if ((size_t)heap_end == (size_t)seg) {
				seg->size = bytes + *offset;
				return ptr;
			}
			size_t original_offset = *offset;
			*offset = align;
			u8* data = align_alloc(bytes, offset);
			size_t bs = heap_bsize((void*)((size_t)ptr - original_offset)) - original_offset;
			size_t size = (bytes < bs)? bytes : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			if (on_realloc != null) {
				on_realloc(data);
			}
			return data;
		}

		void* align_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) {
			heap_segment* seg = (heap_segment*)((size_t)ptr - sizeof(heap_segment) - *offset);
			if (seg->size >= bytes + *offset) {
				return ptr;
			}
			if ((size_t)heap_end == (size_t)seg) {
				//	is last block -> enlarge
				seg->size = bytes + *offset;
				return ptr;
			}
			size_t original_offset = *offset;
			*offset = align;
			u8* data = align_alloc(bytes + add, offset);
			size_t bs = heap_bsize((void*)((size_t)ptr - original_offset)) - original_offset;
			size_t size = (bytes + add < bs)? bytes + add : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void* align_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*)) {
			heap_segment* seg = (heap_segment*)((size_t)ptr - sizeof(heap_segment) - *offset);
			if (seg->size >= bytes + *offset) {
				return ptr;
			}
			if ((size_t)heap_end == (size_t)seg) {
				//	is last block -> enlarge
				seg->size = bytes + *offset;
				return ptr;
			}
			size_t original_offset = *offset;
			*offset = align;
			u8* data = align_alloc(bytes + add, offset);
			size_t bs = heap_bsize((void*)((size_t)ptr - original_offset)) - original_offset;
			size_t size = (bytes + add < bs)? bytes + add : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			if (on_realloc != null) {
				on_realloc(data);
			}
			return data;
		}

		void* heap_align_enlarge(size_t bytes, size_t* align_) {
			if (!heap_end->used) {
				void* ptr = align((void*)((size_t)heap_end + sizeof(heap_segment)), *align_);
				*align_ = (size_t)ptr - ((size_t)heap_end + sizeof(heap_segment));
				heap_end->size = bytes + *align_;
				heap_end->next = null;
				heap_end->used = true;
				return ptr;
			}
			//print("expanding heap: "); printu(*align_); endl();
			return heap_align_expand(bytes, align_);
		}


		void* heap_align_expand(size_t bytes, size_t* align_) {
			struct heap_segment* last = heap_end;
			heap_end = (struct heap_segment*)((size_t)heap_end + sizeof(heap_segment) + last->size);
			last->next = heap_end;

			//	calculate pointer position and offset
			void* ptr = align((void*)((size_t)heap_end + sizeof(heap_segment)), *align_);
			*align_ = (size_t)ptr - ((size_t)heap_end + sizeof(heap_segment));

			//print("align expand: "); printp(ptr); print(" : "); printp((void*)((size_t)heap_end + sizeof(heap_segment))); endl();

			heap_end->used = true;
			heap_end->size = bytes + *align_;
			heap_end->next = null;
			return ptr;
		}

		void* heap_align_enlargeo(size_t bytes, size_t* align_) {
			if (!heap_end->used) {

				void* ptr = align((void*)((size_t)heap_end + sizeof(heap_segment)), *align_);
				size_t offset = (size_t)ptr - ((size_t)heap_end + sizeof(heap_segment));
				if (offset >= sizeof(heap_segment) + 8) {
					heap_segment* last = heap_end;
					heap_end = (heap_segment*)((size_t)heap_end + offset);
					last->next = heap_end;
					last->size = (size_t)heap_end - (size_t)last - sizeof(heap_segment);

					heap_end->size = bytes;
					heap_end->next = null;
					heap_end->used = true;
					*align_ = 0;
					return ptr;
				}
				heap_end->size = bytes + offset;
				heap_end->used = true;
				*align_ = offset;
				return ptr;
			}
			return heap_align_expando(bytes, align_);
		}

		void* heap_align_expando(size_t bytes, size_t* align_) {
			heap_segment* last = heap_end;
			heap_end = (heap_segment*)((size_t)last + sizeof(heap_segment) + last->size);
			last->next = heap_end;

			void* ptr = align((void*)((size_t)heap_end + sizeof(heap_segment)), *align_);
			size_t offset = (size_t)ptr - (size_t)heap_end - sizeof(heap_segment);
			if (offset > sizeof(heap_segment) + 8) {
				heap_segment* nseg = (heap_segment*)((size_t)heap_end + offset);
				heap_end->next = nseg;
				heap_end->used = false;
				heap_end->size = (size_t)nseg - (size_t)heap_end - sizeof(heap_segment);
				heap_end = nseg;

				heap_end->next = null;
				heap_end->size = bytes;
				heap_end->used = true;
				*align_ = 0;
				return ptr;
			} else {
				heap_end->size = offset + bytes;
				heap_end->next = null;
				heap_end->used = true;
				*align_ = offset;
				return ptr;
			}
		}

#else
		#warning memory/heap.c already included
	#endif
#else
	#error memory/heap.c: memory/heap.h not included
#endif