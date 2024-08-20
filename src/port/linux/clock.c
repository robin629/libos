#include "../../../inc/clock.h"
#include "../../../inc/errno.h"

#include <sys/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

/* ------------------------------------------------------------ */

int
os_clock_get(os_clock_t *p)
{
	time_t now = time(NULL);
	struct tm tm;

	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (NULL == localtime_r(&now, &tm))
	{
		os_errno = OS_EERROR;
		return -1;
	}

	/* Copy linux time/date to os_clock_t */
	p->sec	 = tm.tm_sec;
	p->min	 = tm.tm_min;
	p->hour  = tm.tm_hour;
	p->mday  = tm.tm_mday;
	p->wday	 = tm.tm_wday;
	p->yday  = tm.tm_yday;
	p->month = tm.tm_mon;
	p->year	 = tm.tm_year;

	/* DST value (0 = N/a, 1 = No, 2 = Yes) */
	if (0 == tm.tm_isdst)
		p->dst = 1U;
	else if (1 == tm.tm_isdst)
		p->dst = 2U;
	else
		p->dst = 0U;

	return 0;
}

int
os_clock_set(os_clock_t *p)
{
	struct timeval tv;
	struct tm tm;
	time_t t;

	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Set linux time/date using os_clock_t struct */
	tm.tm_sec	= p->sec;
	tm.tm_min	= p->min;
	tm.tm_hour	= p->hour;
	tm.tm_mday	= p->mday;
	tm.tm_mon	= p->month;
	tm.tm_year	= p->year;

	/* Configure DST (0 = N/a, 1 = No, 2 = Yes) */
	if (1U == p->dst)
		tm.tm_isdst	= 0;
	else if (2U == p->dst)
		tm.tm_isdst = 1;
	else
		tm.tm_isdst = -1;

	t = mktime(&tm);
	if (-1 == t)
	{
		printf("mktime: %s\n", strerror(errno));

		if (EINVAL == errno)
			os_errno = OS_EINVAL;
		else if (EPERM == errno)
			os_errno = OS_EPERM;
		else
			os_errno = OS_EERROR;

		return -1;
	}

	tv.tv_sec  = t;
	tv.tv_usec = 0;

	if (-1 == settimeofday(&tv, NULL))
	{
		printf("settimeofday: %s\n", strerror(errno));

		if (EINVAL == errno)
			os_errno = OS_EINVAL;
		else if (EPERM == errno)
			os_errno = OS_EPERM;
		else
			os_errno = OS_EERROR;

		return -1;
	}

	return 0;
}
