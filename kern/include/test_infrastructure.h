/*
 * Header file with infrastructure needed for kernel unit tests:
 *  - Assertion functions.
 *  - Test structures.
 */

/* Macros for assertions. 
 * They depend on <stdbool.h> and printk() to be included in the source file. 
 */
#define KT_ASSERT_M(message, test)                                               \
	do {                                                                         \
		if (!(test)) {                                                           \
			printk("Assertion failed: %s\n", message);                           \
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