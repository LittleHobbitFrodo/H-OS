//
//	memory/heap/physical.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

void *palloc(size_t bytes) {
	struct heap_segment_t *i = heap.used_until;
	struct heap_segment_t *con = null;
	size_t c_size = 0;

	for (;; i = i->next) {
		if (i->used) {
			if (c_size > bytes) {
				heap_connect(con, i);
				con->used = true;
				return (void *) ((size_t) con + sizeof(heap_segment_t));
			}
			c_size = 0;
			con = null;
		} else {
			if (i->size >= bytes) {
				i->used = true;
				if ((size_t) heap.used_until < (size_t) i) {
					heap.used_until = i;
				}
				if (i->size > bytes + sizeof(heap_segment_t) + 8) {
					heap_divide(i, bytes);
				}
				return (void *) ((size_t) i + sizeof(heap_segment_t));
			} else if (con == null) {
				con = i;
				c_size = i->size;
			} else if (c_size > bytes) {
				heap_connect(con, i);
				con->used = true;
				return (void *) ((size_t) con + sizeof(heap_segment_t));
			}
		}

		if (i->next == null) {
			break;
		}
	}
	return pheap_enlarge(bytes);
}


void *prealloc(void *ptr, size_t bytes) {
	heap_segment_t *seg = (heap_segment_t *) ((size_t) ptr - sizeof(heap_segment_t));
	if (seg->size >= bytes) {
		if (seg->size > bytes + sizeof(heap_segment_t) + 8) {
			heap_divide(seg, bytes);
		}
		return ptr;
	}
	if (unlikely((size_t)heap.end == (size_t)seg)) {
		seg->size = bytes;
		return ptr;
	}
	u8 *data = palloc(bytes);
	size_t bs = heap_bsize(ptr);
	size_t size = (bytes < bs) ? bytes : bs;
	for (size_t i = 0; i < size; i++) {
		data[i] = ((u8 *) ptr)[i];
	}
	seg->used = false;
	return data;
}

void *prealloca(void *ptr, size_t bytes, size_t add) {
	heap_segment_t *seg = (heap_segment_t *) ((size_t) ptr - sizeof(heap_segment_t));
	if (seg->size >= bytes) {
		return ptr;
	}
	if ((size_t) heap.end == (size_t) seg) {
		seg->size = bytes;
		return ptr;
	}
	u8 *data = palloc(bytes + add);
	size_t bs = heap_bsize(ptr);
	size_t size = (bytes + add < bs) ? bytes + add : bs;
	for (size_t i = 0; i < size; i++) {
		data[i] = ((u8 *) ptr)[i];
	}
	seg->used = false;
	return data;
}


void *pheap_expand(size_t bytes) {
	struct heap_segment_t *last = heap.end;
	heap.end = (struct heap_segment_t *) ((size_t) heap.end + sizeof(heap_segment_t) + last->size);
	last->next = heap.end;
	heap.end->used = true;
	heap.end->size = bytes;
	heap.end->next = null;
	return (void *) ((size_t) heap.end + sizeof(heap_segment_t));
}

void heap_connect(heap_segment_t *beg, heap_segment_t *end) {
	if ((size_t) beg < (size_t) end) {
		beg->next = end;
		beg->size = ((size_t) end - sizeof(heap_segment_t) - (size_t) beg);
	}
}

heap_segment_t *heap_divide(heap_segment_t *seg, size_t size) {
	if (seg->size > size + sizeof(heap_segment_t) + 8) {
		size_t rsize = seg->size - sizeof(heap_segment_t);
		seg->size = size;
		heap_segment_t *nw = (heap_segment_t *) ((size_t) seg + sizeof(heap_segment_t) + size);
		nw->next = seg->next;
		seg->next = nw;
		nw->used = false;
		nw->size = rsize - size;
		return nw;
	}
	return seg;
}

void *pheap_enlarge(size_t bytes) {
	if (!heap.end->used) {
		heap.end->size = bytes;
		heap.end->next = null;
		heap.end->used = true;
		return (void *) ((size_t) heap.end + sizeof(heap_segment_t));
	}
	return pheap_expand(bytes);
}


void *palign_alloc(size_t bytes, size_t *align_) {
	struct heap_segment_t *i = heap.used_until;
	struct heap_segment_t *con = null;
	size_t c_size = 0;
	void *ptr;
	size_t offset = 0, c_offset = 0;

	for (;; i = i->next) {
		if (i->used) {
			c_size = 0;
			con = null;
		} else {
			ptr = align((void *) ((size_t) i + sizeof(heap_segment_t)), *align_);
			offset = (size_t) ptr - ((size_t) i + sizeof(heap_segment_t));
			if (i->size >= bytes + offset) {
				//	block big enough
				i->used = true;
				if ((size_t) heap.used_until < (size_t) i) {
					heap.used_until = i;
				}
				if (i->size > bytes + offset + sizeof(heap_segment_t) + 8) {
					heap_divide(i, bytes + offset);
				}
				if (offset > sizeof(heap_segment_t) + 8) {
					heap_segment_t *next = i->next;
					heap_segment_t *nseg = (heap_segment_t *) ((size_t) i + offset);
					i->next = nseg;
					i->size = (size_t) nseg - (size_t) i - sizeof(heap_segment_t);
					nseg->next = next;
					nseg->size = (size_t) next - (size_t) nseg - sizeof(heap_segment_t);
					nseg->used = true;
					*align_ = 0;
				} else {
					*align_ = offset;
				}
				return ptr;
			} else if (con == null) {
				con = i;
				c_size = i->size;
				c_offset = (size_t) align((void *) ((size_t) con + sizeof(heap_segment_t)), *align_) - ((size_t) i + sizeof(heap_segment_t));
			} else if (c_size > bytes + c_offset) {
				heap_connect(con, i);
				con->used = true;
				if (c_offset > sizeof(heap_segment_t) + 8) {
					heap_segment_t *next = i->next;
					heap_segment_t *nseg = (heap_segment_t *) ((size_t) i + c_offset);
					i->next = nseg;
					i->size = (size_t) nseg - (size_t) i - sizeof(heap_segment_t);
					nseg->next = next;
					nseg->size = (size_t) next - (size_t) nseg - sizeof(heap_segment_t);
					nseg->used = true;
					*align_ = 0;
				} else {
					*align_ = c_offset;
				}
				return (void *) ((size_t) con + sizeof(heap_segment_t) + c_offset);
			}
		}

		if (i->next == null) {
			break;
		}
	}
	return pheap_align_enlarge(bytes, align_);
}

void *palign_realloc(void *ptr, size_t *offset, size_t align, size_t bytes) {
	heap_segment_t *seg = (heap_segment_t *) ((size_t) ptr - sizeof(heap_segment_t) - *offset);
	if (seg->size >= bytes + *offset) {
		if (seg->size > bytes + sizeof(heap_segment_t) + 8) {
			heap_divide(seg, bytes + *offset);
		}
		return ptr;
	}
	if ((size_t) heap.end == (size_t) seg) {
		seg->size = bytes + *offset;
		return ptr;
	}
	size_t original_offset = *offset;
	*offset = align;
	u8 *data = palign_alloc(bytes, offset);
	size_t bs = heap_bsize((void *) ((size_t) ptr - original_offset)) - original_offset;
	size_t size = (bytes < bs) ? bytes : bs;
	for (size_t i = 0; i < size; i++) {
		data[i] = ((u8 *) ptr)[i];
	}
	seg->used = false;
	return data;
}

void *palign_reallocf(void *ptr, size_t *offset, size_t align, size_t bytes, void (*on_realloc)(void *)) {
	heap_segment_t *seg = (heap_segment_t *) ((size_t) ptr - sizeof(heap_segment_t) - *offset);
	if (seg->size >= bytes + *offset) {
		if (seg->size > bytes + sizeof(heap_segment_t) + 8) {
			heap_divide(seg, bytes + *offset);
		}
		return ptr;
	}
	if ((size_t) heap.end == (size_t) seg) {
		seg->size = bytes + *offset;
		return ptr;
	}
	size_t original_offset = *offset;
	*offset = align;
	u8 *data = palign_alloc(bytes, offset);
	size_t bs = heap_bsize((void *) ((size_t) ptr - original_offset)) - original_offset;
	size_t size = (bytes < bs) ? bytes : bs;
	for (size_t i = 0; i < size; i++) {
		data[i] = ((u8 *) ptr)[i];
	}
	seg->used = false;
	if (on_realloc != null) {
		on_realloc(data);
	}
	return data;
}


void *palign_realloca(void *ptr, size_t *offset, size_t align, size_t bytes, size_t add) {
	heap_segment_t *seg = (heap_segment_t *) ((size_t) ptr - sizeof(heap_segment_t) - *offset);
	if (seg->size >= bytes + *offset) {
		return ptr;
	}
	if ((size_t) heap.end == (size_t) seg) {
		//	is last block -> enlarge
		seg->size = bytes + *offset;
		return ptr;
	}
	size_t original_offset = *offset;
	*offset = align;
	u8 *data = palign_alloc(bytes + add, offset);
	size_t bs = heap_bsize((void *) ((size_t) ptr - original_offset)) - original_offset;
	size_t size = (bytes + add < bs) ? bytes + add : bs;
	for (size_t i = 0; i < size; i++) {
		data[i] = ((u8 *) ptr)[i];
	}
	seg->used = false;
	return data;
}

void *palign_reallocaf(void *ptr, size_t *offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void *)) {
	heap_segment_t *seg = (heap_segment_t *) ((size_t) ptr - sizeof(heap_segment_t) - *offset);
	if (seg->size >= bytes + *offset) {
		return ptr;
	}
	if ((size_t) heap.end == (size_t) seg) {
		//	is last block -> enlarge
		seg->size = bytes + *offset;
		return ptr;
	}
	size_t original_offset = *offset;
	*offset = align;
	u8 *data = palign_alloc(bytes + add, offset);
	size_t bs = heap_bsize((void *) ((size_t) ptr - original_offset)) - original_offset;
	size_t size = (bytes + add < bs) ? bytes + add : bs;
	for (size_t i = 0; i < size; i++) {
		data[i] = ((u8 *) ptr)[i];
	}
	seg->used = false;
	if (on_realloc != null) {
		on_realloc(data);
	}
	return data;
}

void *pheap_align_enlarge(size_t bytes, size_t *align_) {
	if (!heap.end->used) {
		void *ptr = align((void *) ((size_t) heap.end + sizeof(heap_segment_t)), *align_);
		size_t offset = (size_t) ptr - ((size_t) heap.end + sizeof(heap_segment_t));
		if (offset >= sizeof(heap_segment_t) + 8) {
			heap_segment_t *last = heap.end;
			heap.end = (heap_segment_t *) ((size_t) heap.end + offset);
			last->next = heap.end;
			last->size = (size_t) heap.end - (size_t) last - sizeof(heap_segment_t);

			heap.end->size = bytes;
			heap.end->next = null;
			heap.end->used = true;
			*align_ = 0;
			return ptr;
		}
		heap.end->size = bytes + offset;
		heap.end->used = true;
		*align_ = offset;
		return ptr;
	}
	return pheap_align_expand(bytes, align_);
}

void *pheap_align_expand(size_t bytes, size_t *align_) {
	heap_segment_t *last = heap.end;
	heap.end = (heap_segment_t *) ((size_t) last + sizeof(heap_segment_t) + last->size);
	last->next = heap.end;

	void *ptr = align((void *) ((size_t) heap.end + sizeof(heap_segment_t)), *align_);
	size_t offset = (size_t) ptr - (size_t) heap.end - sizeof(heap_segment_t);
	if (offset > sizeof(heap_segment_t) + 8) {
		heap_segment_t *nseg = (heap_segment_t *) ((size_t) heap.end + offset);
		heap.end->next = nseg;
		heap.end->used = false;
		heap.end->size = (size_t) nseg - (size_t) heap.end - sizeof(heap_segment_t);
		heap.end = nseg;

		heap.end->next = null;
		heap.end->size = bytes;
		heap.end->used = true;
		*align_ = 0;
		return ptr;
	} else {
		heap.end->size = offset + bytes;
		heap.end->next = null;
		heap.end->used = true;
		*align_ = offset;
		return ptr;
	}
}
