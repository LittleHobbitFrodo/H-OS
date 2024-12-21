//
//	memory/heap/multipurpose/heap.h
//		part of the CORE kernel belonging to the H-OS project
//

//	atomic buddy heap implementation
	//	each CPU has its own heap

#pragma once

#include "../../../integers.h"
#include "./structures.h"

//	in KB
#define HEAP_GLOBAL_MINIMAL_SIZE 1024
#define HEAP_MINIMAL_SIZE 256
#define HEAP_INITIAL_BLOCK_SIZE 64


#define heap_wait_and_lock(instance) while (__atomic_test_and_set(&instance->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define heap_wait_lock(instance) while (__atomic_load_n(&instance->lock, __ATOMIC_ACQUIRE)) {iowait();}
#define heap_lock(instance) __atomic_test_and_set(&instance->lock)
#define heap_unlock(instance) __atomic_clear(&instance->lock, __ATOMIC_RELEASE);
#define heap_wait_pending(allocator) while (__atomic_load_n(&allocator->pending, __ATOMIC_ACQUIRE)) {iowait();}
#define heap_wait_and_mark_pending(allocator) while (__atomic_test_and_set(&allocator->pending, __ATOMIC_ACQUIRE)) {iowait();}
#define heap_pending_unmark(allocator) allocator->pending = false
#define heap_pending_mark(allocator) __atomic_test_and_set(&allocator->pending)

//	these functions will not lock alloc
	//	will check for pending
void* multipurpose_alloc(allocator_t* alloc, heap_size_t bytes);
void* multipurpose_realloc(allocator_t* alloc, void* ptr, heap_size_t bytes);
void* multipurpose_realloca(allocator_t* alloc, void* ptr, heap_size_t bytes, heap_size_t add);
void multipurpose_free(allocator_t* alloc, void* ptr);


//	lock alloc and pending
void* multipurpose_foreign_alloc(allocator_t* alloc, heap_size_t bytes);
void* multipurpose_foreign_realloc(allocator_t* alloc, void* ptr, heap_size_t bytes);
void* multipurpose_foreign_realloca(allocator_t* alloc, void* ptr, heap_size_t bytes, heap_size_t add);
inline void multipurpose_foreign_free(allocator_t* alloc, void* ptr) {
	heap_wait_and_lock(alloc);
	heap_wait_pending(alloc);
	heap_block* block = ((heap_block*)ptr - 1);
	block->used = false;
	heap_unlock(alloc);
}

//[[maybe_unused]] static bool heap_map(allocator_t* alloc);

__attribute__((always_inline))
inline size_t heap_bsize(void* ptr) {
	return (((heap_block*)ptr) - 1)->size;
}

void* _heap_enlarge(allocator_t* alloc, heap_size_t bytes);
void* _heap_expand(allocator_t* alloc, heap_size_t bytes);

void _heap_connect(allocator_t* alloc, heap_block* beg, heap_block* fin);
void _heap_divide(allocator_t* alloc, heap_block* block, heap_size_t bytes);

void heap_debug(allocator_t* alloc);

static void heap_init();
//static void heap_allocator_init(allocator_t* alloc);

/*static void heap_init();

void heap_debug();

bool heap_map(size_t physical, void** virt, size_t size, void** tableptr);

__attribute__((always_inline, nonnull))
inline void free(void* ptr) {
	heap_segment_t* seg = (heap_segment_t*)((size_t)ptr - sizeof(heap_segment_t));
	if ((size_t)heap.used_until > (size_t)seg) {
		heap.used_until = seg;
	}
	seg->used = false;
}

__attribute__((always_inline, nonnull(1))) inline void align_free(void* ptr, size_t offset) {
	free((void*)((size_t)ptr - offset));
}

[[nodiscard]] void* alloc(size_t bytes) __attribute__((returns_nonnull));

[[nodiscard]] void* realloc(void* ptr, size_t bytes) __attribute__((nonnull, returns_nonnull));
[[nodiscard]] void* realloca(void* ptr, size_t bytes, size_t add) __attribute__((nonnull, returns_nonnull));
//	reserve more/less space if reallocation is needed

__attribute__((always_inline, nonnull)) inline size_t heap_bsize(void* ptr) {
	return ((heap_segment_t*)((size_t)ptr - sizeof(heap_segment_t)))->size;
}

[[nodiscard]] void* heap_expand(size_t bytes) __attribute__((returns_nonnull));

void heap_connect(heap_segment_t* beg, heap_segment_t* end) __attribute__((nonnull(1, 2)));

heap_segment_t* heap_divide(heap_segment_t* seg, size_t size);

[[nodiscard]] void* heap_enlarge(size_t bytes) __attribute__((returns_nonnull));


[[nodiscard]] void* align_alloc(size_t bytes, size_t* align) __attribute__((nonnull(2), returns_nonnull));
//	allocates aligned memory and create new block at the empty space

[[nodiscard]] void* align_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) __attribute__((nonnull(1, 2), returns_nonnull));
[[nodiscard]] void* align_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
//	if data is reallocated -> calls on_realloc and pass pointer to it

[[nodiscard]] void* align_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) __attribute__((nonnull(1, 2), returns_nonnull));
[[nodiscard]] void* align_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*)) __attribute__((nonnull(1, 2), returns_nonnull));
//	if data is reallocated -> calls on_realloc and pass pointer to it

[[nodiscard]] void* heap_align_enlarge(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));

[[nodiscard]] void* heap_align_expand(size_t bytes, size_t* align_) __attribute__((nonnull(2), returns_nonnull));*/