#ifndef ROS_INC_TEST_INFRASTRUCTURE_H
#define ROS_INC_TEST_INFRASTRUCTURE_H

/*
 * Header file with infrastructure needed for kernel unit tests:
 *  - Assertion functions.
 *  - Test structures.
 */

#include <stdio.h>
#include <kmalloc.h>

/* Macros for assertions. 
 * They depend on <stdbool.h> and printk() to be included in the source file. 
 */
#define KT_ASSERT_M(message, test)                                               \
	do {                                                                         \
		if (!(test)) {                                                           \
			extern char *kern_test_msg;                                          \
			char prefix[] = "Assertion failed: ";                                \
			int msg_size = sizeof(prefix) + sizeof(message) - 1;                 \
			kern_test_msg = (char*) kmalloc(msg_size, 0);                        \
			snprintf(kern_test_msg, msg_size, "%s%s", prefix, message);          \
			return false;                                                        \
		}                                                                        \
	} while (0)

#define KT_ASSERT(test)                                                          \
	do {                                                                         \
		if (!(test)) {                                                           \
			return false;                                                        \
		}                                                                        \
	} while (0)


/* Postboot kernel tests: tests ran after boot in kernel mode. */

struct pb_kernel_test {
	char name[256]; // Name of the test function.
	bool (*func)(void); // Name of the test function, should be equal to 'name'.
	bool enabled; // Whether to run or not the test.
};

/* Macro for registering a postboot kernel test. */
#define PB_K_TEST_REG(name, enabled)                                            \
	{"test_" #name, test_##name, enabled}

extern struct pb_kernel_test pb_kernel_tests[];
extern int num_pb_kernel_tests;

#endif // ROS_INC_TEST_INFRASTRUCTURE_H