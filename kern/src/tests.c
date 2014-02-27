/*
 * Declaration of all the tests to be ran.
 */

#include <stdbool.h>
#include <test_infrastructure.h>
#include "tests_pb_kernel.c"


/* Postboot kernel tests declarations. */

struct pb_kernel_test pb_kernel_tests[] = {
#ifdef CONFIG_X86
	PB_K_TEST_REG(ipi_sending, false),
	PB_K_TEST_REG(pic_reception, false),
	PB_K_TEST_REG(ioapic_pit_reroute, false),
	PB_K_TEST_REG(lapic_status_bit, false),
	PB_K_TEST_REG(pit, false),
	PB_K_TEST_REG(circ_buffer, false),
	PB_K_TEST_REG(kernel_messages, false),
#endif // CONFIG_X86
	PB_K_TEST_REG(print_info, false), 
	PB_K_TEST_REG(page_coloring, false),
	PB_K_TEST_REG(color_alloc, false),
	PB_K_TEST_REG(barrier, false),
	PB_K_TEST_REG(interrupts_irqsave, true),
	PB_K_TEST_REG(bitmasks, true),
	PB_K_TEST_REG(checklists, false),
	PB_K_TEST_REG(smp_call_functions, false),
	PB_K_TEST_REG(slab, false),
	PB_K_TEST_REG(kmalloc, false),
	PB_K_TEST_REG(hashtable, true),
	PB_K_TEST_REG(bcq, false),
	PB_K_TEST_REG(ucq, false),
	PB_K_TEST_REG(vm_regions, true),
	PB_K_TEST_REG(radix_tree, true),
	PB_K_TEST_REG(random_fs, false),
	PB_K_TEST_REG(kthreads, false),
	PB_K_TEST_REG(kref, false),
	PB_K_TEST_REG(atomics, false),
	PB_K_TEST_REG(abort_halt, false),
	PB_K_TEST_REG(cv, false),
	PB_K_TEST_REG(memset, false),
	PB_K_TEST_REG(setjmp, false),
	PB_K_TEST_REG(apipe, false),
	PB_K_TEST_REG(rwlock, false),
	PB_K_TEST_REG(rv, false),
	PB_K_TEST_REG(alarm, false)
};

int num_pb_kernel_tests = sizeof(pb_kernel_tests) /
                          sizeof(struct pb_kernel_test);
