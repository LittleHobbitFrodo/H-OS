//
//	memory/heap.h
//		part of the CORE kernel belonging to the H-OS project
//

#ifndef H_OS_LIB_MEMORY_HEAP_H
	#define H_OS_LIB_MEMORY_HEAP_H

	#ifndef HEAP_INITIAL_BLOCK_SIZE
		#define HEAP_INITIAL_BLOCK_SIZE 32
	#endif

	#define HEAP_MINIMAL_ENTRY_SIZE 512
		//	minimum memmap entry size (in kb)

	typedef struct heap_segment{
		struct heap_segment* next;
		size_t size;
		bool used;
	} heap_segment;

	bool heap_reserve_memory(bool include_reclaimable_entries);

	void heap_init();

	static heap_segment* heap_start = null;
	static void* heap_end_physical = null;

	static heap_segment* heap_used_until = null;
	static heap_segment* heap_end = null;





	void* alloc(size_t bytes) __attribute__((returns_nonnull));
	void* realloc(void* ptr, size_t bytes) __attribute__((nonnull(0), returns_nonnull));
	void* realloca(void* ptr, size_t bytes, size_t add) __attribute__((nonnull(0), returns_nonnull));
		//	reserve more space if reallocation is needed

	__attribute__((always_inline, nonnull)) inline void free(void* ptr) {
		((heap_segment*)((size_t)ptr - sizeof(heap_segment)))->used = false;
	}

	__attribute__((always_inline, nonnull)) inline size_t heap_bsize( void* ptr) {
		return ((heap_segment*)((size_t)ptr - sizeof(heap_segment)))->size;
	}

	[[nodiscard]] void* heap_expand(size_t bytes) __attribute__((returns_nonnull));
	void heap_connect(heap_segment* beg, heap_segment* end) __attribute__((nonnull(0, 1)));
	heap_segment* heap_divide(heap_segment* seg, size_t size);
	[[nodiscard]] void* heap_enlarge(size_t bytes) __attribute__((returns_nonnull));

	void heap_debug();

	void* align_alloc(size_t bytes, size_t* align_) __attribute__((nonnull(1), returns_nonnull));
	void* align_alloco(size_t bytes, size_t* align) __attribute__((nonnull(1), returns_nonnull));
		//	allocates aligned memory and create new block at the empty space
	void* align_realloc(void* ptr, size_t* offset, size_t align, size_t bytes) __attribute__((nonnull(0, 1), returns_nonnull));
	void* align_reallocf(void* ptr, size_t* offset, size_t align, size_t bytes, void (*on_realloc)(void*)) __attribute__((nonnull(0, 1), returns_nonnull));
		//	if data is reallocated -> calls on_realloc and pass pointer to it
	void* align_realloca(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add) __attribute__((nonnull(0, 1), returns_nonnull));
	void* align_reallocaf(void* ptr, size_t* offset, size_t align, size_t bytes, size_t add, void (*on_realloc)(void*)) __attribute__((nonnull(0, 1), returns_nonnull));
		//	if data is reallocated -> calls on_realloc and pass pointer to it

	void* heap_align_enlarge(size_t bytes, size_t* align_) __attribute__((nonnull(1), returns_nonnull));
	void* heap_align_expand(size_t bytes, size_t* align_) __attribute__((nonnull(1), returns_nonnull));

	void* heap_align_enlargeo(size_t bytes, size_t* align_) __attribute__((nonnull(1), returns_nonnull));
	void* heap_align_expando(size_t bytes, size_t* align_) __attribute__((nonnull(1), returns_nonnull));

	__attribute__((always_inline, nonnull)) inline void align_free(void* ptr, size_t offset) {
		free((void*)((size_t)ptr - offset));
	}


#else
	#warning memory/heap.h already included
#endif