//
//  memory/paging.cpp
//    part of the H-OS
//

#ifdef H_OS_LIB_MEMORY_HEAP_H
  #ifndef H_OS_LIB_MEMORY_HEAP_CPP
    #define H_OS_LIB_MEMORY_HEAP_CPP

      mem::heap::segment::segment() {
        this->next = nullptr;
        this->size = 0;
        this->used = false;
      }
      mem::heap::segment::segment(mem::heap::segment &seg) {
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
        mem::heap::start->used = false;
        mem::heap::start->next = nullptr;
        mem::heap::start->size = mem::heap::initial_block_size;
        mem::heap::end = (mem::heap::used_until = mem::heap::start);
      }





      void mem::heap::debug() {
        u32 col = out::color;
        out::color = out::col::red;
        struct mem::heap::segment* seg = mem::heap::start;
        for (u32 i = 0;; i++) {
          output << i+1 << ":\tused:\t" << seg->used << "\n\tsize:\t" << seg->size << "\t: " << (void*)((size_t)seg + sizeof(mem::heap::segment) + seg->size) << "\n\tnext: " << seg->next << out::endl;
          if (seg->next == nullptr) {
            break;
          }
          seg = seg->next;
        }
        out::color = col;
      }

      size_t mem::heap::size() {
        return ((size_t)mem::heap::end + mem::heap::end->size) - (size_t)mem::heap::start;
      }

      u32 mem::heap::block_size(void *ptr) {
        return ((struct mem::heap::segment*)((size_t)ptr - sizeof(struct mem::heap::segment)))->size;
      }

      void mem::heap::connect(mem::heap::segment *beg, mem::heap::segment *fin) {
        if ((size_t)beg < (size_t)fin) {
          beg->next = fin;
          beg->size = (size_t)fin - ((size_t)beg + sizeof(struct mem::heap::segment));
        }
      }


      struct mem::heap::segment* mem::heap::divide_block(mem::heap::segment *seg, size_t size) {
        if (seg->size >= size + sizeof(struct mem::heap::segment) + mem::heap::divide_min_bytes) {
          size_t rsize = seg->size - sizeof(struct mem::heap::segment);
          seg->size = size;
          auto nw = (struct mem::heap::segment*)((size_t)seg + seg->size);
          nw->next = seg->next;
          seg->next = nw;
          nw->used = false;
          nw->size = rsize - size;
          return nw;
        }
        return seg;
      }




      //  normal pointers


      void* mem::heap::alloc(size_t bytes) {
        struct mem::heap::segment* i = mem::heap::used_until;

        struct mem::heap::segment* con = nullptr;
        size_t c_size = 0;
          //  variables used for connecting unused blocks


        for (;;i = i->next) {

          if (i->used) {
            con = nullptr;
            c_size = 0;
          } else {
            if (i->size >= bytes) {
              i->used = true;
              if ((size_t)mem::heap::used_until < (size_t)i) {
                mem::heap::used_until = i;
              }
              return (void*)((size_t)i + sizeof(struct mem::heap::segment));
            } else if (con == nullptr) {
              con = i;
              c_size = i->size;
            } else if (c_size >= bytes) {
              mem::heap::connect(con, i);
              con->used = true;
              return (void*)((size_t)con + sizeof(struct mem::heap::segment));
            } else {
              c_size += i->size + sizeof(struct mem::heap::segment);
                //  add size of the segment structure (else it loses data space)
            }
          }

          if (i->next == nullptr) {
            break;
          }
        }
        return mem::heap::expand(bytes);
      }

      void *mem::heap::realloc(void *ptr, size_t bytes) {
        auto seg = (struct mem::heap::segment*)((size_t)ptr - sizeof(struct mem::heap::segment));
        if (seg->size >= bytes) {
          return ptr;
        } else {
          u8* data = (u8*)mem::heap::alloc(bytes);
          size_t bs = mem::heap::block_size(data);
          u64 sz = (seg->size < bs)? seg->size : bs;
          for (size_t i = 0; i < sz; i++) {
            data[i] = ((u8*)ptr)[i];
          }
          seg->used = false;
          return data;
        }
      }

      void *mem::heap::realloc(void *ptr, size_t bytes, size_t add) {
        auto seg = (struct mem::heap::segment*)((size_t)ptr - sizeof(struct mem::heap::segment));
        if (seg->size >= bytes) {
          return ptr;
        } else {
          u8* data = (u8*)mem::heap::alloc(bytes + add);
          size_t bs = mem::heap::block_size(data);
          u64 sz = (seg->size < bs)? seg->size : bs;
          for (size_t i = 0; i < sz; i++) {
            data[i] = ((u8*)&ptr)[i];
          }
          seg->used = false;
          return data;
        }
      }


      void mem::heap::free(void *ptr) {
        auto seg = (struct mem::heap::segment*)((size_t)ptr - sizeof(struct mem::heap::segment));
        seg->used = false;
        if ((size_t)seg < (size_t)mem::heap::used_until) {
          mem::heap::used_until = seg;
        }
      }

      void* mem::heap::expand(size_t bytes) {
        const auto last = mem::heap::end;
        mem::heap::end = (struct mem::heap::segment*)((size_t)mem::heap::end + sizeof(struct mem::heap::segment) + last->size);
        last->next = mem::heap::end;
        mem::heap::end->next = nullptr;
        mem::heap::end->size = bytes;
        mem::heap::end->used = true;
        return (void*)((size_t)mem::heap::end + sizeof(struct mem::heap::segment));
      }

      void* mem::heap::enlarge(size_t bytes) {
        auto cur = mem::heap::end;
        if (!cur->used) {
          cur->size = bytes;
          cur->next = nullptr;  //  just to be sure
          cur->used = true;
          return (void*)((size_t)cur + sizeof(struct mem::heap::segment));
        } else {
          return mem::heap::expand(bytes);
        }
      }







      //  aligned pointers

      void* mem::heap::alloc(size_t bytes, size_t& align) {
        struct mem::heap::segment* i = mem::heap::used_until;

        struct mem::heap::segment* con = nullptr;
        size_t c_size = 0;
        //  variables used for connecting unused blocks

        size_t offset {0};
        void* ptr {nullptr};


        for (;;i = i->next) {

          if (i->used) {
            con = nullptr;
            c_size = 0;
          } else {

            ptr = mem::align((void*)((size_t)i + sizeof(struct mem::heap::segment)), 4096);
            offset = (size_t)ptr - ((size_t)i + sizeof(struct mem::heap::segment));

            if (i->size >= offset + bytes) {
              i->used = true;
              if ((size_t)mem::heap::used_until < (size_t)i) {
                mem::heap::used_until = i;
              }
              align = offset;
              return ptr;
            } else if (con == nullptr) {
              con = i;
              c_size = i->size;
            } else if (c_size >= bytes) {
              mem::heap::connect(con, i);

              //  recalculate pointer and offset for con (not i)
              ptr = mem::align((void*)((size_t)con + sizeof(struct mem::heap::segment)), 4096);
              offset = (size_t)ptr - ((size_t)con + sizeof(struct mem::heap::segment));
              con->used = true;
              align = offset;
              return ptr;
            } else {
              c_size += i->size + sizeof(struct mem::heap::segment);
              //  add size of the segment structure (else it loses data space)
            }
          }

          if (i->next == nullptr) {
            break;
          }
        }
        return mem::heap::expand(bytes, align);
      }


      void *mem::heap::realloc(void *ptr, size_t bytes, size_t add, size_t& offset, size_t original_align) {
        auto seg = (struct mem::heap::segment*)((size_t)ptr - sizeof(struct mem::heap::segment));
        if (seg->size >= bytes) {
          return ptr;
        } else {
          u8* data = (u8*)mem::heap::alloc(bytes + add, original_align);
          for (size_t i = 0; i < seg->size; i++) {
            data[i] = ((u8*)&ptr)[i];
          }
          offset = original_align;
          seg->used = false;
          return data;
        }
      }


      void* mem::heap::expand(size_t bytes, size_t& align) {
        const auto last = mem::heap::end;
        mem::heap::end = (struct mem::heap::segment*)((size_t)mem::heap::end + sizeof(struct mem::heap::segment) + last->size);
        last->next = mem::heap::end;

        void* ptr = mem::align((void*)((size_t)mem::heap::end + sizeof(struct mem::heap::segment)), align);
        align = (size_t)ptr - ((size_t)mem::heap::end + sizeof(struct mem::heap::segment));

        mem::heap::end->next = nullptr;
        mem::heap::end->size = bytes + align;
        mem::heap::end->used = true;
        return ptr;
      }

      void* mem::heap::enlarge(size_t bytes, size_t& align) {
        auto cur = mem::heap::end;
        if (!cur->used) {

          void* ptr = mem::align((void*)((size_t)cur + sizeof(struct mem::heap::segment)), align);
          align = (size_t)ptr - ((size_t)cur + sizeof(struct mem::heap::segment));

          cur->size = bytes + align;
          cur->next = nullptr;  //  just to be sure
          cur->used = true;
          return ptr;
        } else {
          return mem::heap::expand(bytes, align);
        }
      }


  #else
    #warning memory/heap.cpp already included
  #endif
#else
  #error memory/heap.cpp: memory/heap.h not included
#endif