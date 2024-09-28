//
//	memory.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "./integers.h"

#define KB 1024
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)


static u8 KERNEL_STACK[32*KB];

static u8 INTERRUPT_STACK[(8*7)*KB];


static bool kaslr = false;
//	enable/disable kaslr
//		enabled: places kernel stack and heap to "random" place
//		disabled: always places stack and heap to the same location

typedef struct stack_holder {
	void* kernel;
	void* interrupt[7];
} stack_holder;

static stack_holder stack;

static void memory_init();

__attribute__((always_inline)) inline void *align(void *ptr, size_t align) {
	return (void *) (((u64) ptr + align - 1) & ~(align - 1));
}

static struct meminfo {
	size_t total;
	size_t usable;
	size_t used;

	size_t reserved; //	reserved memory
	size_t ring0; //	ring 0 memory (kheap, stacks, acpi, fb, ...)

	size_t unmapped; //	bad memory
} meminfo;


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

static void memmap_reclaim();

static void memmap_analyze();

enum memmap_types memmap_entry_type(u64 constant);


[[maybe_unused]] static void memmap_display();
[[maybe_unused]] static void memmap_display_original();


//	memmap vector is declared in vector.h
