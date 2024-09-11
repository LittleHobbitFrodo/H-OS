//
//	memory/heap/physical.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifndef H_OS_LIB_HEAP_H
	#ifndef H_OS_LIB_MEMORY_HEAP_PHYSICAL_C
		#define H_OS_LIB_MEMORY_HEAP_PHYSICAL_C

		void* palloc(size_t bytes) {
			struct heap_segment* i = heap_used_until;
			struct heap_segment* con = null;
			size_t c_size = 0;

			for (;; i = i->next) {

				if (i->used) {
					if (c_size > bytes) {
						heap_connect(con, i);
						con->used = true;
						return (void*)((size_t)con + sizeof(heap_segment));
					}
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
			return pheap_enlarge(bytes);
		}


		void* prealloc(void* ptr, size_t bytes) {
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
			u8* data = palloc(bytes);
			size_t bs = heap_bsize(ptr);
			size_t size = (bytes < bs)? bytes : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void* prealloca(void* ptr, size_t bytes, size_t add) {
			heap_segment* seg = (heap_segment*)((size_t)ptr - sizeof(heap_segment));
			if (seg->size >= bytes) {
				return ptr;
			}
			if ((size_t)heap_end == (size_t)seg) {
				seg->size = bytes;
				return ptr;
			}
			u8* data = palloc(bytes + add);
			size_t bs = heap_bsize(ptr);
			size_t size = (bytes + add < bs)? bytes + add : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}


		void* pheap_expand(size_t bytes) {
			struct heap_segment* last = heap_end;
			heap_end = (struct heap_segment*)((size_t)heap_end + sizeof(heap_segment) + last->size);
			last->next = heap_end;
			heap_end->used = true;
			heap_end->size = bytes;
			heap_end->next = null;
			return (void*)((size_t)heap_end + sizeof(heap_segment));
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

		void* pheap_enlarge(size_t bytes) {
			if (!heap_end->used) {
				heap_end->size = bytes;
				heap_end->next = null;
				heap_end->used = true;
				return (void*)((size_t)heap_end + sizeof(heap_segment));
			}
			return pheap_expand(bytes);
		}


		void* palign_alloc(size_t bytes, size_t* align_)  {
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
			return pheap_align_enlarge(bytes, align_);
		}

		void* palign_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) {
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
			u8* data = palign_alloc(bytes, offset);
			size_t bs = heap_bsize((void*)((size_t)ptr - original_offset)) - original_offset;
			size_t size = (bytes < bs)? bytes : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void* palign_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*)) {
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
			u8* data = palign_alloc(bytes, offset);
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


		void* palign_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) {
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
			u8* data = palign_alloc(bytes + add, offset);
			size_t bs = heap_bsize((void*)((size_t)ptr - original_offset)) - original_offset;
			size_t size = (bytes + add < bs)? bytes + add : bs;
			for (size_t i = 0; i < size; i++) {
				data[i] = ((u8*)ptr)[i];
			}
			seg->used = false;
			return data;
		}

		void* palign_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*)) {
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
			u8* data = palign_alloc(bytes + add, offset);
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

		void* pheap_align_enlarge(size_t bytes, size_t* align_) {
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
			return pheap_align_expand(bytes, align_);
		}

		void* pheap_align_expand(size_t bytes, size_t* align_) {
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



	#endif
#else
	#error memory/heap/physical.c: memory/heap.h not included
#endif