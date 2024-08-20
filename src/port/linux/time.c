#include "../../../inc/time.h"
#include "../../../inc/overflow.h"

#include <stdbool.h>
#include <limits.h>
#include <time.h>

#define MILLION 1000000
#define BILLION 1000000000

/* Seconds in 1s */
#define SECOND 1L

/* Milliseconds in 1s */
#define MILLISECOND 1000L

/* Microseconds in 1s */
#define MICROSECOND 1000000L

/* Nanoseconds in 1s */
#define NANOSECOND  1000000000L

/* Convert time units... Example: Convert 5,000ms to seconds: UNIT_CONV(MILLISECOND, SECOND, 5000) == 5s */
#define UNIT_CONV(from, to, val) (long)((from > to) ? ((val) / (from/to)) : ((to/from) * (val)))

#define stoms(n)  UNIT_CONV(SECOND, MILLISECOND, n)
#define stous(n)  UNIT_CONV(SECOND, MICROSECOND, n)
#define stons(n)  UNIT_CONV(SECOND, NANOSECOND, n)

#define mstos(n)  UNIT_CONV(MILLISECOND, SECOND, n)
#define mstous(n) UNIT_CONV(MILLISECOND, MICROSECOND, n)
#define mstons(n) UNIT_CONV(MILLISECOND, NANOSECOND, n)

#define ustos(n)  UNIT_CONV(MICROSECOND, SECOND, n)
#define ustoms(n) UNIT_CONV(MICROSECOND, MILLISECOND, n)
#define ustons(n) UNIT_CONV(MICROSECOND, NANOSECOND, n)

#define nstos(n)  UNIT_CONV(NANOSECOND, SECOND, n)
#define nstoms(n) UNIT_CONV(NANOSECOND, MILLISECOND, n)
#define nstous(n) UNIT_CONV(NANOSECOND, MICROSECOND, n)

/* ------------------------------------------------------------ */

void
os_sleep(long sec)
{
	struct timespec tspec;

	if (sec < 1L)
		return;

	tspec.tv_sec  = sec;
	tspec.tv_nsec = 0L;

	nanosleep(&tspec, NULL);
}

void
os_sleep_ms(long msec)
{
	struct timespec tspec;

	if (msec < 1L)
		return;

	tspec.tv_sec  = mstos(msec);
	tspec.tv_nsec = mstons(msec % MILLISECOND) % BILLION;

	nanosleep(&tspec, NULL);
}

void
os_sleep_us(long usec)
{
	struct timespec tspec;

	if (usec < 1L)
		return;

	tspec.tv_sec  = ustos(usec);
	tspec.tv_nsec = ustons(usec % MICROSECOND) % BILLION;

	nanosleep(&tspec, NULL);
}

void
os_sleep_ns(long nsec)
{
	struct timespec tspec;

	if (nsec < 1L)
		return;

	tspec.tv_sec  = 0L;
	tspec.tv_nsec = nsec % BILLION;

	nanosleep(&tspec, NULL);
}

os_time_t
os_time_monotonic()
{
	os_time_t retval;

	if (-1 == clock_gettime(CLOCK_MONOTONIC, &retval))
		return OS_TIME_INIT;

	return retval;
}

os_time_t
os_time_realtime()
{
	os_time_t retval;

	if (-1 == clock_gettime(CLOCK_REALTIME, &retval))
		return OS_TIME_INIT;

	return retval;
}

os_time_t
os_time_add(const os_time_t old, const os_time_t new)
{
	long ss;
	long ns;

	if (OS_OVERFLOW_ADD(LONG_MAX, LONG_MIN, old.tv_sec + 1L, new.tv_sec))
		return OS_TIME_INIT;

	if (OS_OVERFLOW_ADD(LONG_MAX, LONG_MIN, old.tv_nsec, new.tv_nsec))
		return OS_TIME_INIT;

	ss = old.tv_sec  + new.tv_sec;
	ns = old.tv_nsec + new.tv_nsec;

	/* Time nanoseconds can't hold more than 9,999,999,999 */
	if (ns > BILLION)
	{
		ss += 1L;
		ns -= BILLION;
	}

	return (os_time_t){.tv_sec = ss, .tv_nsec = ns};
}

os_time_t
os_time_diff(const os_time_t old, const os_time_t new)
{
	if (new.tv_nsec < old.tv_nsec)
	{
		if (OS_OVERFLOW_SUB(LONG_MAX, LONG_MIN, new.tv_sec - 1L, old.tv_sec))
			return OS_TIME_INIT;

		if (OS_OVERFLOW_SUB(LONG_MAX, LONG_MIN, new.tv_nsec, old.tv_nsec))
			return OS_TIME_INIT;

		return (os_time_t) {
			.tv_sec  = (new.tv_sec - 1L) - old.tv_sec,
			.tv_nsec = ((new.tv_nsec - old.tv_nsec) + BILLION) % BILLION
		};
	}
	else
	{
		if (OS_OVERFLOW_SUB(LONG_MAX, LONG_MIN, new.tv_sec, old.tv_sec))
			return OS_TIME_INIT;

		if (OS_OVERFLOW_SUB(LONG_MAX, LONG_MIN, new.tv_nsec, old.tv_nsec))
			return OS_TIME_INIT;

		return (os_time_t) {
			.tv_sec  = new.tv_sec  - old.tv_sec,
			.tv_nsec = new.tv_nsec - old.tv_nsec
		};
	}
}

os_time_t
os_time_from_s(long sec)
{
	if (sec < 1L)
		return (os_time_t) OS_TIME_INIT;

	return (os_time_t){ .tv_sec = sec, .tv_nsec = 0L };
}

os_time_t
os_time_from_ms(long msec)
{
	if (msec < 1L)
		return (os_time_t) OS_TIME_INIT;

	return (os_time_t){ .tv_sec = mstos(msec), .tv_nsec = mstons(msec % MILLISECOND) % BILLION };
}

os_time_t
os_time_from_us(long usec)
{
	if (usec < 1L)
		return (os_time_t) OS_TIME_INIT;

	return (os_time_t){ .tv_sec = ustos(usec), .tv_nsec = ustons(usec % MICROSECOND) % BILLION };
}

os_time_t
os_time_from_ns(long nsec)
{
	if (nsec < 1L)
		return (os_time_t) OS_TIME_INIT;

	return (os_time_t){ .tv_sec = 0L, .tv_nsec = nsec % BILLION };
}

long
os_time_s(const os_time_t tval)
{
	if (OS_OVERFLOW_ADD(LONG_MAX, LONG_MIN, tval.tv_sec, (tval.tv_nsec / NANOSECOND)))
		return 0L;

	return (tval.tv_sec + (tval.tv_nsec / NANOSECOND));
}

long
os_time_ms(const os_time_t tval)
{
	if (OS_OVERFLOW_ADD(LONG_MAX, LONG_MIN, stoms(tval.tv_sec), nstoms(tval.tv_nsec)))
		return 0L;

	return stoms(tval.tv_sec) + nstoms(tval.tv_nsec);
}

long
os_time_us(const os_time_t tval)
{
	if (OS_OVERFLOW_ADD(LONG_MAX, LONG_MIN, stous(tval.tv_sec), nstous(tval.tv_nsec)))
		return 0L;

	return stous(tval.tv_sec) + nstous(tval.tv_nsec);
}

long
os_time_ns(const os_time_t tval)
{
	if (OS_OVERFLOW_ADD(LONG_MAX, LONG_MIN, stons(tval.tv_sec), tval.tv_nsec))
		return 0L;

	return stons(tval.tv_sec) + tval.tv_nsec;
}
