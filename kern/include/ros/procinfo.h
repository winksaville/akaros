/* See COPYRIGHT for copyright information. */

#ifndef ROS_PROCINFO_H
#define ROS_PROCINFO_H

#include <ros/memlayout.h>
#include <ros/common.h>
#include <ros/resource.h>
#include <ros/atomic.h>
#include <ros/arch/arch.h>
#include <string.h>

/* Process creation flags */
#define PROC_DUP_FGRP			1

#define PROCINFO_MAX_ARGP 32
#define PROCINFO_ARGBUF_SIZE 3072

#ifdef ROS_KERNEL
#include <sys/queue.h>
#endif /* ROS_KERNEL */

/* Not necessary to expose all of this, but it doesn't hurt, and is convenient
 * for the kernel.  Need to do some acrobatics for the TAILQ_ENTRY. */
struct vcore;
struct vcore {
#ifdef ROS_KERNEL
	TAILQ_ENTRY(vcore)	list;
#else /* userspace */
	void				*dummy_ptr1;
	void				*dummy_ptr2;
#endif /* ROS_KERNEL */
	uint32_t			pcoreid;
	bool				valid;
	uint32_t			nr_preempts_sent;	/* these two differ when a preempt*/
	uint32_t			nr_preempts_done;	/* is in flight. */
	uint64_t			preempt_pending;
	/* A process can see cumulative runtime as of the last resume, and can also
	 * calculate runtime in this interval, by adding (ns - resume) + total. */
	uint64_t			resume_ticks;		/* TSC at resume time */
	uint64_t			total_ticks;		/* ticks up to last offlining */
};

struct pcore {
	uint32_t			vcoreid;
	bool 				valid;
};

typedef struct procinfo {
	pid_t pid;
	pid_t ppid;
	size_t max_vcores;	/* TODO: change to a uint32_t */
	uint64_t tsc_freq;
	uint64_t timing_overhead;
	void *heap_bottom;
	/* glibc relies on stuff above this point.  if you change it, you need to
	 * rebuild glibc. */
	bool is_mcp;			/* is in multi mode */
	unsigned long 		res_grant[MAX_NUM_RESOURCES];
	struct vcore		vcoremap[MAX_NUM_CPUS];
	uint32_t			num_vcores;
	struct pcore		pcoremap[MAX_NUM_CPUS];
	seq_ctr_t			coremap_seqctr;
} procinfo_t;
#define PROCINFO_NUM_PAGES  ((sizeof(procinfo_t)-1)/PGSIZE + 1)	


// this is how user programs access the procinfo page
#ifndef ROS_KERNEL
# define __procinfo (*(procinfo_t*)UINFO)

#include <ros/common.h>
#include <ros/atomic.h>
#include <ros/syscall.h>

/* Figure out what your vcoreid is from your pcoreid and procinfo.  Only low
 * level or debugging code should call this. */
static inline uint32_t __get_vcoreid_from_procinfo(void)
{
	/* The assumption is that any IPIs/KMSGs would knock userspace into the
	 * kernel before it could read the closing of the seqctr.  Put another way,
	 * there is a 'memory barrier' between the IPI write and the seqctr write.
	 * I think this is true. */
	uint32_t kpcoreid, kvcoreid;
	seq_ctr_t old_seq;
	do {
		cmb();
		old_seq = __procinfo.coremap_seqctr;
		kpcoreid = __ros_syscall_noerrno(SYS_getpcoreid, 0, 0, 0, 0, 0, 0);
		if (!__procinfo.pcoremap[kpcoreid].valid)
			continue;
		kvcoreid = __procinfo.pcoremap[kpcoreid].vcoreid;
	} while (seqctr_retry(old_seq, __procinfo.coremap_seqctr));
	return kvcoreid;
}

static inline uint32_t __get_vcoreid(void)
{
	/* since sys_getvcoreid could lie (and might never change) */
	return __get_vcoreid_from_procinfo();
}

#endif /* ifndef ROS_KERNEL */

#endif // !ROS_PROCDATA_H
