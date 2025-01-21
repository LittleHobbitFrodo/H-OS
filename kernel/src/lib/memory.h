//
//	memory.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

#define KB 1024
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)

typedef struct membase_t {
	void* virtual;
	void* physical;
	void* hhdm;
} membase_t;

static membase_t base = {0};


static u8 stack[7][8*KB];

static void memory_init();

#define align(val, algn) (((size_t)(val) + (algn) - 1) & ~((algn) - 1))



typedef struct meminfo_t {
	size_t total;
	size_t usable;
	size_t used;

	size_t reserved; //	reserved memory
	size_t system; //	ring 0 memory (kheap, stacks, acpi, fb, ...)

	size_t unmapped; //	bad memory
} meminfo_t;

static meminfo_t meminfo = {0};


enum memmap_types {
	memmap_usable,
	memmap_kernel,
	memmap_heap,
	memmap_reserved,
	memmap_reclaimable,
	memmap_acpi,
	memmap_bad,
	memmap_stack,
	memmap_other,
	memmap_paging,
	memmap_undefined
} memmap_types;

typedef struct memmap_entry {
	size_t base;
	size_t len;
	enum memmap_types type;
} memmap_entry;

static void memmap_parse();

[[maybe_unused]] static void memmap_analyze();

[[maybe_unused]] static memmap_entry* memmap_find(enum memmap_types type);

enum memmap_types memmap_entry_type(u64 constant);


[[maybe_unused]] static void memmap_display();
[[maybe_unused]] static void memmap_display_original();

void memmap_ent_construct(memmap_entry* ent);
