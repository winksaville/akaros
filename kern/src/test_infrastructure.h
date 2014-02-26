/*
 * Header file for test structures
 */

/* Postboot kernel tests: tests ran after boot in kernel mode */

struct pb_kernel_test {
	char name[256]; // Name of the test function
	bool (*func)(void); // Name of the test function, should be equal to 'name'.
	bool enabled; // Whether to run or not the test.
};

extern struct pb_kernel_test pb_kernel_tests[];
extern int num_pb_kernel_tests;