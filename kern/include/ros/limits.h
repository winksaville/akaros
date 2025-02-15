#ifndef ROS_INC_LIMITS_H
#define ROS_INC_LIMITS_H

/* Keep this 255 to stay in sync with glibc (expects d_name[256]) */
#define MAX_FILENAME_SZ 255
/* POSIX / glibc name: */
#define NAME_MAX MAX_FILENAME_SZ

#define PATH_MAX 4096 /* includes null-termination */

/* # bytes of args + environ for exec()  (i.e. max size of argenv) */
#define ARG_MAX (32 * 4096) /* Good chunk of our 256 page stack */

#endif /* ROS_INC_LIMITS_H */
