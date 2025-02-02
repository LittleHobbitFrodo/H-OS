## Important

- [X] memmap_parse redo => unaligned heaps (multiple in one entry)

- [x] create TODO file
- [X] fix paging (again) (again)
  - yeah, some specific pages have different layout
  - [X] make new structure for 2mb pages
      make all heaps (+ page heaps) use 2mb pages (paging.h, paging.c)
- [X] create vector_with_different_allocator_type
    - vector_with_different_allocator_type_type: vector with different type of allocator for vector and instances

- [ ] write driver for NVME

  - static vector disks should be diskptr_vector
- [ ] implement limine device tree blob feature
  - update limine

## Someday

- [ ] resolve SIMD and GPRs
  - vx512f?
  - for faster page table copying etc
- [ ] update limine
    - 8.6.0
- [ ] fix quick page mapping
  - crashes for some reason (idk)

- [ ] resolve gcc warning and error
  - linker input file