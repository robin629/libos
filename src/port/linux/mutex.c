#include "../../../inc/mutex.h"
#include "../../../inc/errno.h"

#include "../../private.h"

#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------ */

int
os_mutex_init(os_mutex_t *p)
{
	pthread_mutexattr_t attr;

	/* Clear out new mutex memory */
	memset(p, 0, sizeof(os_mutex_t));

	/* Initialize the mutex attribute values */
	pthread_mutexattr_init(&attr);

	/* Make the mutex recursive */
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	/* Initialize the underlying mutex type (pthread_mutex_t) */
	if (0 != pthread_mutex_init(&p->mutex, &attr))
	{
		OS_PRV_ERR("pthread_mutex_init() error");

		/* Set os_errno to indicate unspecified error */
		os_errno = OS_EERROR;

		return -1;
	}

	return 0;
}

int
os_mutex_destroy(os_mutex_t *p)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Clear memory */
	memset(p, 0, sizeof(*p));

	return 0;
}

int
os_mutex_lock(os_mutex_t *p)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (0 != pthread_mutex_lock(&(p->mutex)))
	{
		/* Set os_errno to indicate unspecified error */
		os_errno = OS_EERROR;

		return -1;
	}

	return 0;
}

int
os_mutex_unlock(os_mutex_t *p)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (0 != pthread_mutex_unlock(&(p->mutex)))
	{
		/* Set os_errno to indicate unspecified error */
		os_errno = OS_EERROR;

		return -1;
	}

	return 0;
}
