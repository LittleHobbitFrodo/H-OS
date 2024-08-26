//
//	memory/paging.h
//		part of the CORE kernel belonging to the H-OS project
//

#ifndef H_OS_LIB_MEMORY_PAGING_H
	#define H_OS_LIB_MEMORY_PAGING_H

	//	each pdpt entry covers 1GB of RAM

	static volatile struct limine_paging_mode_request page_mode_req = {
		.id = LIMINE_PAGING_MODE_REQUEST,
		.revision = 0,
		.mode = LIMINE_PAGING_MODE_X86_64_4LVL
	};

	static void* virtual_base = null;
	static void* physical_base = null;

	#define PAGE_COUNT 512
	#define PAGE_SIZE 4096

	void page_init();

	enum page_flags {					//	structure of page_entry
		present = 1,					//	page is present?
		write = (1 << 1),				//	writable or only readable?
		user = (1 << 2),				//	can user processes read it?
		write_through = (1 << 3),		//	write-through caching?
		cache_disable = (1 << 4),		//	disable caching?
		accessed = (1 << 5),			//	accessed?
		ignored1 = (1 << 6),			//	ignored bit
			//	in pml4, pdpt, pd if PS == 0
		dirty = (1 << 6),				//	has been written to?
			//	only for PTs or larger (if PS == 1)
				//	else ignored
		PS = (1 << 7),					//	page size
			//			pml4, pt -> ignored
			//			pdpt -> 1Gig
			//			pt ->
		global = (1 << 8),				//	global? (used by more processes)
			//	only in PT entries
		available = (1 << 9 | 1 << 10 | 1 << 11),	//	ignored by hardware
		address = (0xffffffffff000),
		available2 = (0x3f << 52),
		reserved_ = (0b111 << 59),		//	used by hardware (must be set to 0)

		//	bits 9,10,11 are ignored (can be used by OS)

		no_exec = (1 << 63),			//	disable executing

		address_shift = 12
	};


	typedef u64 page_entry;

	page_entry* page_find();

	void page_entry_info(page_entry ent);

	typedef struct page_table {
		//	helps managing paging on heap
		page_entry* base;			//	base address (on heap)
		size_t offset;		//	offset (from block start) (before allocation: align)
		size_t size;
	} page_table;

	//	initialization
		//	pml4 and pdpts are global (only kernel mapped memory (PDs, PTs) are globally too)
		//	each process will have its own PD/PDs and PTs
	static page_table pml4 			= {.base = null, .offset = 4096, .size = 0};
	static page_table pdpt 			= {.base = null, .offset = 4096, .size = 0};
	static page_table kpd			= {.base = null, .offset = 4096, .size = 0};
		//	kpd: mapping ring 0 stuff (pml4[0], pdpt[0])

	static page_table page_ring0	= {.base = null, .offset = 4096, .size = 0};
	static page_table page_heap		= {.base = null, .offset = 4096, .size = 0};
		//	since kernel heap can grow it is separated

	static struct kpages {
		page_entry* kernel;
		page_entry* stacks;
		page_entry* heap;
		struct count {
			size_t kernel;
			size_t heap;
			size_t stacks;
		} count;
	} kpages;

	__attribute__((always_inline)) inline void* page_address(page_entry entry) {
		return (void*)((entry >> 12) & (0xffffffffffff));
	}

	__attribute__((always_inline)) inline void page_set_address(page_entry* entry, void* ptr) {
		*entry &= ~0x000ffffffffff000;
		*entry |= (((size_t)ptr << 12) & 0x000ffffffffff000);
	}

	__attribute__((always_inline)) inline u64 page_flags(page_entry entry) {
		return entry & (0x8ff | ((u64)1 << (u64)63));
	}

	__attribute__((always_inline)) inline void page_set_flags(page_entry* entry, u64 flags) {
		*entry &= ~(0x8ff | ((u64)1 << (u64)63));
		*entry |= flags & (0x8ff | ((u64)1 << (u64)63));
	}

	__attribute__((always_inline)) inline bool page_exec(page_entry entry) {
		return (entry & no_exec) != 0;
	}

	__attribute__((always_inline)) inline bool page_present(page_entry entry) {
		return (entry & present);
	}

	typedef void* virtual_addr;

	__attribute__((always_inline)) inline size_t va_offset(virtual_addr addr) {
		return ((size_t)addr & 0xfff);
	}

	__attribute__((always_inline)) inline size_t va_index(virtual_addr addr, u8 level) {
		//	level starts at 0 (0 = pt)
		return ((size_t)addr >> (12 + (level * 9))) & 0x1ff;
	}



	/*
	 *	static paging layout:
	 *	pml4[0]
	 *	pdpt[0]
	 *	pd:
	 *		[0 .. x]	->	ring0 (kernel, stack (in this order) )
	 *		[x .. y]	->	heap
	 *
	 *
	 *	bad, reserved and acpi memory is not mapped yet
	 *		acpi is not supported yet so I dont see point of mapping it
	 */

#else
	#warning memory/paging.h already included
#endif