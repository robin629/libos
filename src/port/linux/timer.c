#include "../../../inc/overflow.h"
#include "../../../inc/runtime.h"
#include "../../../inc/timer.h"
#include "../../../inc/errno.h"
#include "../../../inc/time.h"

#include "../../private.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define TIMER_MAGIC 0x345E887D

/* ------------------------------------------------------------ */

int
os_timer_init(os_timer_t *p)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Clear out new timer memory */
	memset(p, 0, sizeof(*p));

	/* Initialize the timer's magic value */
	p->magic = TIMER_MAGIC;

	/* Obtain the timer's control block mutex */
	if (-1 == os_mutex_init(&(p->mutex)))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

int
os_timer_destroy(os_timer_t *p)
{
	if (NULL == p || TIMER_MAGIC != __atomic_load_n(&(p->magic), __ATOMIC_SEQ_CST))
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Destroy timer's mutex */
	os_mutex_destroy(&(p->mutex));

	/* Clear out all data (including magic value) */
	memset(p, 0, sizeof(*p));

	return 0;
}

int
os_timer_start(os_timer_t *p, long interval)
{
	if (NULL == p || TIMER_MAGIC != __atomic_load_n(&(p->magic), __ATOMIC_SEQ_CST))
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain the timer's control block mutex */
	if (-1 == os_mutex_lock(&(p->mutex)))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Write new interval values to timer control block memory */
	p->interval = interval;

	/* Mark this timer as an active timer */
	p->enabled  = 1U;

	/* Mark this timer as running */
	p->running  = 1U;

	/* Reset the timer expired flag */
	p->expired  = 0U;

	/* Update the wakeup time */
	p->wake_time = os_time_add_ms(os_time_monotonic(), interval);

	/* Release the timer's control block mutex */
	if (-1 == os_mutex_unlock(&(p->mutex)))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

int
os_timer_stop(os_timer_t *p)
{
	if (NULL == p || TIMER_MAGIC != __atomic_load_n(&(p->magic), __ATOMIC_SEQ_CST))
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain the timer's control block mutex */
	if (-1 == os_mutex_lock(&(p->mutex)))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Mark the timer as disabled; calling os_timer_expired() will return false event when (1U == tmp->expired) */
	p->enabled = 0U;

	/* Don't change the interval value, only clear the running flag */
	p->running = 0U;

	/* Clear the expired flag; timer is stopped manually, therfore not expired */
	p->expired = 0U;

	/* Release the timer's control block mutex */
	if (-1 == os_mutex_unlock(&(p->mutex)))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

int
os_timer_restart(os_timer_t *p)
{
	if (NULL == p || TIMER_MAGIC != __atomic_load_n(&(p->magic), __ATOMIC_SEQ_CST))
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain the timer's control block mutex */
	if (-1 == os_mutex_lock(&(p->mutex)))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Mark this timer as enabled */
	p->enabled = 1U;

	/* Set running flag in case the timer was stopped */
	p->running = 1U;

	/* Clear timer expired flag */
	p->expired = 0U;

	/* Update the wakeup time  (using the previous interval) */
	p->wake_time = os_time_add_ms(os_time_monotonic(), p->interval);

	/* Release the timer's control block mutex */
	if (-1 == os_mutex_unlock(&(p->mutex)))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

bool
os_timer_running(os_timer_t *p)
{
	bool retval = false;

	if (NULL == p || TIMER_MAGIC != __atomic_load_n(&(p->magic), __ATOMIC_SEQ_CST))
	{
		return false;
	}

	/* Obtain the timer's control block mutex */
	if (-1 == os_mutex_lock(&(p->mutex)))
	{
		return false;
	}

	/* First, check if the timer is expired */
	if (os_time_cmp(p->wake_time, <, os_time_monotonic()))
	{
		/* Set the expired flag (for our retval) */
		p->expired = 1U;
	}

	/* Get timer running flag (check for expired) */
	retval = (0U != p->enabled) && (0U != p->running) && (0U == p->expired);

	/* Release the timer's control block mutex */
	if (-1 == os_mutex_unlock(&(p->mutex)))
	{
		return false;
	}

	return retval;
}

bool
os_timer_expired(os_timer_t *p)
{
	bool retval = false;

	if (NULL == p || TIMER_MAGIC != __atomic_load_n(&(p->magic), __ATOMIC_SEQ_CST))
	{
		return false;
	}

	/* Obtain the timer's control block mutex */
	if (-1 == os_mutex_lock(&(p->mutex)))
	{
		return false;
	}

	/* Check if the timer is expiring now */
	if (os_time_cmp(p->wake_time, <, os_time_monotonic()))
	{
		/* Set the expired flag (for our retval) */
		p->expired = 1U;
	}

	/* Get timer expired flag (timer must be enabled) */
	retval = (0U != p->enabled) && (0U != p->expired);

	/* Disable the timer if we have expired (prevents further calls to this function to return true) */
	if (0U != p->expired)
		p->enabled = 0U;

	/* Release the timer's control block mutex */
	if (-1 == os_mutex_unlock(&(p->mutex)))
	{
		return false;
	}

	return retval;
}
