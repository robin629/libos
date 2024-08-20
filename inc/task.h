#ifndef OS_TASK_H
#define OS_TASK_H
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

typedef void* (*os_task_func_f)(void *param);

#define OS_TASK_NAME_SIZE 64U

/* Task control block */
typedef struct os_task_s
{
	pthread_mutex_t mutex;

	pthread_t thread;

	uint32_t f_exiting:1U;

	char name[OS_TASK_NAME_SIZE + 1U];

	os_task_func_f p_func;

	void *p_func_arg;
} os_task_t;

/**
 * Create a new os_task_t object. The task is started immediately.
 * 
 * @param[in] p
 * 		Pointer to os_task_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
 * 		OS_EERROR
*/
int os_task_init(os_task_t *p, const char *name, os_task_func_f p_func, void *p_func_arg);

/**
 * Destroy a os_task_t initialized using the os_task_init() function.
 * The task will be stopped if currently running.
 * 
 * @param[in] p
 * 		Pointer to os_task_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
*/
int os_task_destroy(os_task_t *p);

/**
 * Check if the task should exit.
 * 
 * @param[in] p
 * 		Pointer to os_task_t object.
 * 
 * @return bool
 * 		True if task should exit, false if task should keep running.
*/
bool os_task_check_stop(os_task_t *p);

#endif
