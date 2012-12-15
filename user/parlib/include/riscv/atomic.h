#ifndef PARLIB_ARCH_ATOMIC_H
#define PARLIB_ARCH_ATOMIC_H

/* Unlike in x86, we need to include spinlocks in the user atomic ops file.
 * Since compare and swap isn't truely non-blocking, and we can't disable
 * interrupts in userspace, there is a slight chance of deadlock. */

#include <ros/common.h>
#include <ros/atomic.h>
#include <ros/arch/membar.h>

#define SPINLOCK_INITIALIZER {0}

static inline void atomic_init(atomic_t *number, long val);
static inline long atomic_read(atomic_t *number);
static inline void atomic_set(atomic_t *number, long val);
static inline void atomic_inc(atomic_t *number);
static inline void atomic_dec(atomic_t *number);
static inline long atomic_fetch_and_add(atomic_t *number, long val);
static inline long atomic_swap(atomic_t *addr, long val);
static inline void *atomic_swap_ptr(void **addr, void *val);
static inline uint32_t atomic_swap_u32(uint32_t *addr, uint32_t val);
static inline bool atomic_cas(atomic_t *addr, long exp_val, long new_val);
static inline bool atomic_cas_ptr(void **addr, void *exp_val, void *new_val);
static inline bool atomic_cas_u32(uint32_t *addr, uint32_t exp_val,
                                  uint32_t new_val);
static inline void atomic_or_int(volatile int *number, int mask);

/* Inlined functions declared above */

static inline void atomic_init(atomic_t *number, long val)
{
	*(volatile long*)number = val;
}

static inline long atomic_read(atomic_t *number)
{
	return *(volatile long*)number;
}

/* Sparc needs atomic add, but the regular ROS atomic add conflicts with
 * glibc's internal one. */
static inline void ros_atomic_add(atomic_t *number, long inc)
{
	atomic_fetch_and_add(number, inc);
}

static inline void atomic_set(atomic_t *number, long val)
{
	atomic_init(number, val);
}

static inline void atomic_inc(atomic_t *number)
{
	ros_atomic_add(number, 1);
}

static inline void atomic_dec(atomic_t *number)
{
	ros_atomic_add(number, -1);
}

/* Adds val to number, returning number's original value */
static inline long atomic_fetch_and_add(atomic_t *number, long val)
{
	return __sync_fetch_and_add((long*)number, val);
}

static inline long atomic_swap(atomic_t *addr, long val)
{
	return __sync_lock_test_and_set((long*)addr, val);
}

static inline void *atomic_swap_ptr(void **addr, void *val)
{
	return __sync_lock_test_and_set(addr, val);
}

static inline uint32_t atomic_swap_u32(uint32_t *addr, uint32_t val)
{
	return __sync_lock_test_and_set(addr, val);
}

// RISC-V has atomic word ops, not byte ops, so we must manipulate addresses
static inline void atomic_andb(volatile uint8_t* number, uint8_t mask)
{
	uintptr_t offset = (uintptr_t)number & 3;
	uint32_t wmask = (1<<(8*offset+8)) - (1<<(8*offset));
	wmask = ~wmask | ((uint32_t)mask << (8*offset));

	__sync_fetch_and_and((uint32_t*)((uintptr_t)number & ~3), wmask);
}

static inline void atomic_orb(volatile uint8_t* number, uint8_t mask)
{
	uintptr_t offset = (uintptr_t)number & 3;
	uint32_t wmask = (uint32_t)mask << (8*offset);

	__sync_fetch_and_or((uint32_t*)((uintptr_t)number & ~3), wmask);
}

asm (".section .gnu.linkonce.b.__riscv_ros_atomic_lock, \"aw\", %nobits\n"
     "\t.previous");

atomic_t __riscv_ros_atomic_lock
  __attribute__ ((nocommon, section(".gnu.linkonce.b.__riscv_ros_atomic_lock\n\t#"),
                  visibility ("hidden")));

static inline bool atomic_cas(atomic_t *addr, long exp_val, long new_val)
{
	bool retval = 0;
	long temp;

	if ((long)*addr != exp_val)
		return 0;
	if (atomic_swap(&__riscv_ros_atomic_lock, 1))
		return 0;
	if ((long)*addr == exp_val) {
		atomic_swap(addr, new_val);
		retval = 1;
	}
	atomic_set(&__riscv_ros_atomic_lock, 0);
	return retval;
}

static inline bool atomic_cas_ptr(void **addr, void *exp_val, void *new_val)
{
	return atomic_cas((atomic_t*)addr, (long)exp_val, (long)new_val);
}

static inline bool atomic_cas_u32(uint32_t *addr, uint32_t exp_val,
                                  uint32_t new_val)
{
	return atomic_cas((atomic_t*)addr, (long)exp_val, (long)new_val);
}

static inline void atomic_or_int(volatile int *number, int mask)
{
	__sync_fetch_and_or(number, mask);
}

#endif /* !PARLIB_ARCH_ATOMIC_H */
