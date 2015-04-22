#ifndef _VCORE_H
#define _VCORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <arch/vcore.h>
#include <arch/atomic.h>
#include <sys/param.h>
#include <string.h>
#include <timing.h>

/*****************************************************************************/
/* TODO: This is a complete hack, but necessary for vcore stuff to work for now
 * The issue is that exit sometimes calls sys_yield(), and we can't recover from
 * that properly under our vcore model (we shouldn't though).  We really need to
 * rethink what sys_yield 'should' do when in multicore mode, or else come up 
 * with a different syscall entirely. */
#include <stdlib.h>
#include <unistd.h>
#undef exit
#define exit(status) _exit(status)
/*****************************************************************************/

#define LOG2_MAX_VCORES 6
#define MAX_VCORES (1 << LOG2_MAX_VCORES)

#define TRANSITION_STACK_PAGES 2
#define TRANSITION_STACK_SIZE (TRANSITION_STACK_PAGES*PGSIZE)

/* Defined by glibc; Must be implemented by a user level threading library */
extern void vcore_entry();
/* Defined in glibc's start.c */
extern __thread bool __vcore_context;
extern __thread int __vcoreid;
/* Defined in vcore.c */
extern __thread struct syscall __vcore_one_sysc;	/* see sys_change_vcore */

/* Vcore API functions */
static inline uint32_t max_vcores(void);
static inline uint32_t num_vcores(void);
static inline int vcore_id(void);
static inline bool in_vcore_context(void);
static inline bool in_multi_mode(void);
static inline void __enable_notifs(uint32_t vcoreid);
static inline void __disable_notifs(uint32_t vcoreid);
static inline bool notif_is_enabled(uint32_t vcoreid);
static inline bool vcore_is_mapped(uint32_t vcoreid);
static inline bool vcore_is_preempted(uint32_t vcoreid);
static inline struct preempt_data *vcpd_of(uint32_t vcoreid);
static inline bool preempt_is_pending(uint32_t vcoreid);
static inline bool __preempt_is_pending(uint32_t vcoreid);
static inline void *get_vcpd_tls_desc(uint32_t vcoreid);
static inline void set_vcpd_tls_desc(uint32_t vcoreid, void *tls_desc);
static inline uint64_t vcore_account_resume_nsec(uint32_t vcoreid);
static inline uint64_t vcore_account_total_nsec(uint32_t vcoreid);
void vcore_init(void);
void vcore_event_init(void);
void vcore_change_to_m(void);
int vcore_request(long nr_new_vcores);
void vcore_yield(bool preempt_pending);
void vcore_reenter(void (*entry_func)(void));
void enable_notifs(uint32_t vcoreid);
void disable_notifs(uint32_t vcoreid);
void vcore_idle(void);
void ensure_vcore_runs(uint32_t vcoreid);
void cpu_relax_vc(uint32_t vcoreid);
uint32_t get_vcoreid(void);
bool check_vcoreid(const char *str, uint32_t vcoreid);

/* Static inlines */
static inline uint32_t max_vcores(void)
{
	return MIN(__procinfo.max_vcores, MAX_VCORES);
}

static inline uint32_t num_vcores(void)
{
	return __procinfo.num_vcores;
}

static inline int vcore_id(void)
{
	return __vcoreid;
}

static inline bool in_vcore_context(void)
{
	return __vcore_context;
}

static inline bool in_multi_mode(void)
{
	return __procinfo.is_mcp;
}

/* Only call this if you know what you are doing. */
static inline void __enable_notifs(uint32_t vcoreid)
{
	vcpd_of(vcoreid)->notif_disabled = FALSE;
}

static inline void __disable_notifs(uint32_t vcoreid)
{
	vcpd_of(vcoreid)->notif_disabled = TRUE;
}

static inline bool notif_is_enabled(uint32_t vcoreid)
{
	return !vcpd_of(vcoreid)->notif_disabled;
}

static inline bool vcore_is_mapped(uint32_t vcoreid)
{
	return __procinfo.vcoremap[vcoreid].valid;
}

/* We could also check for VC_K_LOCK, but that's a bit much. */
static inline bool vcore_is_preempted(uint32_t vcoreid)
{
	struct preempt_data *vcpd = vcpd_of(vcoreid);
	return atomic_read(&vcpd->flags) & VC_PREEMPTED;
}

static inline struct preempt_data *vcpd_of(uint32_t vcoreid)
{
	return &__procdata.vcore_preempt_data[vcoreid];
}

/* Uthread's can call this in case they care if a preemption is coming.  If a
 * preempt is incoming, this will return TRUE, if you are in uthread context.  A
 * reasonable response for a uthread is to yield, and vcore_entry will deal with
 * the preempt pending.
 *
 * If you call this from vcore context, it will do nothing.  In general, it's
 * not safe to just yield (or do whatever you plan on doing) from arbitrary
 * places in vcore context.  So we just lie about PP. */
static inline bool preempt_is_pending(uint32_t vcoreid)
{
	if (in_vcore_context())
		return FALSE;
	return __preempt_is_pending(vcoreid);
}

static inline bool __preempt_is_pending(uint32_t vcoreid)
{
	return __procinfo.vcoremap[vcoreid].preempt_pending;
}

/* The kernel interface uses uintptr_t, but we have a lot of older code that
 * uses void *, hence the casting. */
static inline void *get_vcpd_tls_desc(uint32_t vcoreid)
{
	return (void*)__procdata.vcore_preempt_data[vcoreid].vcore_tls_desc;
}

static inline void set_vcpd_tls_desc(uint32_t vcoreid, void *tls_desc)
{
	__procdata.vcore_preempt_data[vcoreid].vcore_tls_desc = (uintptr_t)tls_desc;
}

static inline uint64_t vcore_account_resume_ticks(uint32_t vcoreid)
{
	return __procinfo.vcoremap[vcoreid].resume_ticks;
}

static inline uint64_t vcore_account_resume_nsec(uint32_t vcoreid)
{
	return tsc2nsec(vcore_account_resume_ticks(vcoreid));
}

static inline uint64_t vcore_account_total_ticks(uint32_t vcoreid)
{
	return __procinfo.vcoremap[vcoreid].total_ticks;
}

static inline uint64_t vcore_account_total_nsec(uint32_t vcoreid)
{
	return tsc2nsec(vcore_account_total_ticks(vcoreid));
}

static inline uint64_t vcore_account_uptime_ticks(uint32_t vcoreid)
{
	uint64_t resume = __procinfo.vcoremap[vcoreid].resume_ticks; 
	uint64_t total = __procinfo.vcoremap[vcoreid].total_ticks; 
	uint64_t now = read_tsc();
	return now - resume + total;
}

static inline uint64_t vcore_account_uptime_nsec(uint32_t vcoreid)
{
	return tsc2nsec(vcore_account_uptime_ticks(vcoreid));
}

#ifdef __cplusplus
}
#endif

#endif
