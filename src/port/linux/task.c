#include "../../../inc/task.h"
#include "../../../inc/errno.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------ */

int
os_task_init(os_task_t *p, const char *name, os_task_func_f p_func, void *p_func_arg)
{
	if (NULL == p || NULL == name)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Clear out new task memory */
	memset(p, 0, sizeof(os_task_t));

	/* Copy task name into task control block memory (NULL terminated by snprintf) */
	snprintf(p->name, sizeof(p->name), "%s", name);

	/* Initialize mutex memory */
	p->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	/* Store address of task function and function argument */
	p->p_func	  = p_func;
	p->p_func_arg = p_func_arg;

	/* Create the new task thread */
	if (0 != pthread_create(&p->thread, NULL, p->p_func, p->p_func_arg))
	{
		/* Set os_errno to generic error */
		os_errno = OS_EERROR;

		return -1;
	}

	return 0;
}

int
os_task_destroy(os_task_t *p)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain the task's control block mutex */
	if (0 != pthread_mutex_lock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Set the exiting flag to inform task thread to exit */
	p->f_exiting = 1U;

	/* Release the task's control block mutex */
	if (0 != pthread_mutex_unlock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Join the task's pthread to cleanup resources */
	if (0 != pthread_join(p->thread, NULL))
	{
		/* Set os_errno to indicate internal error */
		os_errno = OS_EERROR;

		return -1;
	}

	/* Clear memory */
	memset(p, 0, sizeof(*p));

	return 0;
}

bool
os_task_check_stop(os_task_t *p)
{
	bool retval = false;

	if (NULL == p)
		return false;

	/* Obtain the task's control block mutex */
	if (0 != pthread_mutex_lock(&p->mutex))
		return false;

	/* Get the task exiting flag */
	retval = p->f_exiting;

	/* Release the task's control block mutex */
	if (0 != pthread_mutex_unlock(&p->mutex))
		return false;

	return retval;
}
