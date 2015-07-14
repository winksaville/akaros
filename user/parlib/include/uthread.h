#ifndef PARLIB_UTHREAD_H
#define PARLIB_UTHREAD_H

#include <parlib/vcore.h>
#include <ros/syscall.h>

__BEGIN_DECLS

#define UTHREAD_DONT_MIGRATE		0x001 /* don't move to another vcore */
#define UTHREAD_SAVED				0x002 /* uthread's state is in utf */
#define UTHREAD_FPSAVED				0x004 /* uthread's FP state is in uth->as */
#define UTHREAD_IS_THREAD0			0x008 /* thread0: glibc's main() thread */

/* Thread States */
#define UT_RUNNING		1
#define UT_NOT_RUNNING	2

/* Externally blocked thread reasons (for uthread_has_blocked()) */
#define UTH_EXT_BLK_MUTEX			1
#define UTH_EXT_BLK_JUSTICE			2	/* whatever.  might need more options */

/* Bare necessities of a user thread.  2LSs should allocate a bigger struct and
 * cast their threads to uthreads when talking with vcore code.  Vcore/default
 * 2LS code won't touch udata or beyond. */
struct uthread {
	struct user_context u_ctx;
	struct ancillary_state as;
	void *tls_desc;
	int flags;
	int state;
	int notif_disabled_depth;
	struct syscall *sysc;	/* syscall we're blocking on, if any */
	struct syscall local_sysc;	/* for when we don't want to use the stack */
	void (*yield_func)(struct uthread*, void*);
	void *yield_arg;
	int err_no;
	char err_str[MAX_ERRSTR_LEN];
};
typedef struct uthread uthread_t;
extern __thread struct uthread *current_uthread;
extern bool parlib_wants_to_be_mcp;

/* 2L-Scheduler operations.  Examples in pthread.c. */
struct schedule_ops {
	/* Functions supporting thread ops */
	void (*sched_entry)(void);
	void (*thread_runnable)(struct uthread *);
	void (*thread_paused)(struct uthread *);
	void (*thread_blockon_sysc)(struct uthread *, void *);
	void (*thread_has_blocked)(struct uthread *, int);
	void (*thread_refl_fault)(struct uthread *, unsigned int, unsigned int,
							  unsigned long);
	/* Functions event handling wants */
	void (*preempt_pending)(void);
	void (*spawn_thread)(uintptr_t pc_start, void *data);	/* don't run yet */
};
extern struct schedule_ops *sched_ops;

/* Low-level _S code calls this for basic uthreading without a 2LS */
void uthread_lib_init(void);
/* Call this, passing it a uthread representing thread0, from your 2LS init
 * routines.  When it returns, you're in _M mode (thread0 on vcore0) */
void uthread_2ls_init(struct uthread *uthread);
/* Call this to become an mcp capable of worling with uthreads. */
void uthread_mcp_init(void);

/* Functions to make/manage uthreads.  Can be called by functions such as
 * pthread_create(), which can wrap these with their own stuff (like attrs,
 * retvals, etc). */

/* uthread_init() does the uthread initialization of a uthread that the caller
 * created.  Call this whenever you are "starting over" with a thread.  Pass in
 * attr, if you want to override any defaults. */
struct uth_thread_attr {
	bool want_tls;		/* default, no */
};
void uthread_init(struct uthread *new_thread, struct uth_thread_attr *attr);
/* Call this when you are done with a uthread, forever, but before you free it */
void uthread_cleanup(struct uthread *uthread);
void uthread_runnable(struct uthread *uthread);
void uthread_yield(bool save_state, void (*yield_func)(struct uthread*, void*),
                   void *yield_arg);
void uthread_sleep(unsigned int seconds);
void uthread_usleep(unsigned int usecs);

/* Utility functions */
bool __check_preempt_pending(uint32_t vcoreid);	/* careful: check the code */
void uth_disable_notifs(void);
void uth_enable_notifs(void);

bool register_evq(struct syscall *sysc, struct event_queue *ev_q);
void deregister_evq(struct syscall *sysc);

/* Helpers, which sched_entry() can call */
void highjack_current_uthread(struct uthread *uthread);
void run_current_uthread(void);
void run_uthread(struct uthread *uthread);

/* Asking for trouble with this API, when we just want stacktop (or whatever
 * the SP will be). */
static inline void init_uthread_ctx(struct uthread *uth, void (*entry)(void),
                                    void *stack_bottom, uint32_t size)
{
	init_user_ctx(&uth->u_ctx, (long)entry, (long)(stack_bottom) + size);
}

#define uthread_set_tls_var(uth, name, val)                                    \
({                                                                             \
	typeof(val) __val = val;                                                   \
	begin_access_tls_vars(((struct uthread*)(uth))->tls_desc);                 \
	name = __val;                                                              \
	end_access_tls_vars();                                                     \
})

#define uthread_get_tls_var(uth, name)                                         \
({                                                                             \
	typeof(name) val;                                                          \
	begin_access_tls_vars(((struct uthread*)(uth))->tls_desc);                 \
	val = name;                                                                \
	end_access_tls_vars();                                                     \
	val;                                                                       \
})

__END_DECLS

#endif /* PARLIB_UTHREAD_H */
