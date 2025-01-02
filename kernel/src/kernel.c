//
//	kernel.c
//		part of the CORE kernel belonging to the H-OS project
//

#include "./include.h"
#include "./lib/builtin-shell.h"


void kernel() {
	//	kernel() -> starts the OS

	output.color = col.green;
	print("starting ");
	printl(KERNEL_NAME);
	output.color = col.white;

	pages.heap.global.data->bitmap[0] = 0b110000110101;
	print("testing table allocation:\t"); printb(pages.heap.global.data->bitmap[0]); endl();
	wait(2000);

	any_page_table* table = table_alloc(&pages.heap.global, 4);
	print("table:\t"); printp(table); endl();

	table_heap_debug(&pages.heap.global);

	table_free(&pages.heap.global, table);
	printl("\nfreed:");
	table_heap_debug(&pages.heap.global);

	shell();

}
