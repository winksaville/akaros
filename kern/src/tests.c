/*
 * Declaration of all the tests to be ran.
 */

#include <stdbool.h>
#include "test_infrastructure.h"
#include "tests_pb_kernel.c"


/* Postboot kernel tests declarations. */

struct pb_kernel_test pb_kernel_tests[] = {
#ifdef CONFIG_X86	
	{"test_ipi_sending", test_ipi_sending, true},
	{"test_pic_reception", test_pic_reception, true},
	{"test_ioapic_pit_reroute", test_ioapic_pit_reroute, true},
	{"test_lapic_status_bit", test_lapic_status_bit, true},
	{"test_pit", test_pit, true},
	{"test_circ_buffer", test_circ_buffer, true},
	{"test_kernel_messages", test_kernel_messages, true},
#endif // CONFIG_X86
	{"test_print_info", test_print_info, true},
	{"test_page_coloring", test_page_coloring, true},
	{"test_color_alloc", test_color_alloc, true},
	{"test_barrier", test_barrier, true},
	{"test_interrupts_irqsave", test_interrupts_irqsave, true},
	{"test_bitmasks", test_bitmasks, true},
	{"test_checklists", test_checklists, true},
	{"test_slab", test_slab, true},
	{"test_kmalloc", test_kmalloc, true},
	{"test_hashtable", test_hashtable, true},
	{"test_bcq", test_bcq, true},
	{"test_ucq", test_ucq, true},
	{"test_vm_regions", test_vm_regions, true},
	{"test_radix_tree", test_radix_tree, true},
	{"test_random_fs", test_random_fs, true},
	{"test_kthreads", test_kthreads, true},
	{"test_kref", test_kref, true},
	{"test_atomics", test_atomics, true},
	{"test_abort_halt", test_abort_halt, true},
	{"test_cv", test_cv, true},
	{"test_memset", test_memset, true},
	{"test_setjmp", test_setjmp, true},
	{"test_apipe", test_apipe, true},
	{"test_rwlock", test_rwlock, true},
	{"test_rv", test_rv, true},
	{"test_alarm", test_alarm, true}
};

int num_pb_kernel_tests = sizeof(pb_kernel_tests) / 
                          sizeof(struct pb_kernel_test);
