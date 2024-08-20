#ifndef OS_TIME_H
#define OS_TIME_H
#include <stdbool.h>
#include <time.h>

#define OS_TIME_INIT ((os_time_t){0, 0})

/* OS time (timespec) type */
typedef struct timespec os_time_t;

/**
 * Compare two os_time_t values
 * 
 * @param ts0[in]
 * 		First os_time_t value
 * 
 * @param operator[in]
 * 		<, >, ==, <=, >=
 * 
 * @param ts1[in]
 * 		Second os_time_t value
 * 
 * @return compare true/false
*/
#define os_time_cmp(ts0, operator, ts1) (bool)(			\
		((ts0).tv_sec == (ts1).tv_sec) ?				\
			((ts0).tv_nsec operator (ts1).tv_nsec) :	\
			((ts0).tv_sec  operator (ts1).tv_sec)		\
	)

/**
 * Sleep for n amount of time
 * 
 * @param sec[in]
 * 		Sleep duration (seconds)
*/
void os_sleep(long sec);

/**
 * Sleep for n amount of time
 * 
 * @param msec[in]
 * 		Sleep duration (milliseconds)
*/
void os_sleep_ms(long msec);

/**
 * Sleep for n amount of time
 * 
 * @param usec[in]
 * 		Sleep duration (microseconds)
*/
void os_sleep_us(long usec);

/**
 * Sleep for n amount of time
 * 
 * @param nsec[in]
 * 		Sleep duration (nanoseconds)
*/
void os_sleep_ns(long nsec);

/**
 * Get current clock ticks since boot as os_time_t value
 * 
 * @return os_time_t value for MONOTONIC clock
*/
os_time_t os_time_monotonic();

/**
 * Get current wall time (UTC) as os_time_t value.
 * 
 * @return os_time_t value for REALTIME clock
*/
os_time_t os_time_realtime();

#define os_time_utc() os_time_realtime()

/**
 * Add one os_time_t value to another.
 * 
 * @param old[in]
 * 		Older os_time_t value
 * 
 * @param new[in]
 * 		Newer os_time_t value to add
 * 
 * @return os_time_t old + os_time_t new
*/
os_time_t os_time_add(const os_time_t old, const os_time_t new);

/**
 * Add seconds to a os_time_t value
 * 
 * @param tval[in]
 * 		os_time_t value
 * 
 * @param sec[in]
 * 		Number of seconds to add
 * 
 * @return os_time_t tval + sec
*/
#define os_time_add_s(tval, sec) os_time_add(tval, os_time_from_s(sec))

/**
 * Add milliseconds to a os_time_t value
 * 
 * @param tval[in]
 * 		os_time_t value
 * 
 * @param msec[in]
 * 		Number of milliseconds to add
 * 
 * @return os_time_t tval + msec
*/
#define os_time_add_ms(tval, msec) os_time_add(tval, os_time_from_ms(msec))

/**
 * Add microseconds to a os_time_t value
 * 
 * @param tval[in]
 * 		os_time_t value
 * 
 * @param usec[in]
 * 		Number of microseconds to add
 * 
 * @return os_time_t tval + usec
*/
#define os_time_add_us(tval, usec) os_time_add(tval, os_time_from_us(usec))

/**
 * Add nanoseconds to a os_time_t value
 * 
 * @param tval[in]
 * 		os_time_t value
 * 
 * @param nsec[in]
 * 		Number of nanoseconds to add
 * 
 * @return os_time_t tval + nsec
*/
#define os_time_add_ns(tval, nsec) os_time_add(tval, os_time_from_ns(nsec))

/**
 * Calculate the difference between two os_time_t values
 * 
 * @param old[in]
 * 		Older os_time_t value
 * 
 * @param new[in]
 * 		Newer os_time_t value
 * 
 * @return Difference between os_time_t old and os_time_t new
*/
os_time_t os_time_diff(const os_time_t old, const os_time_t new);

/**
 * Calculate the difference between two os_time_t value in seconds.
 * 
 * @param old[in]
 * 		Older os_time_t value
 * 
 * @param new[in]
 * 		Newer os_time_t value
 * 
 * @return Difference between os_time_t old and os_time_t new in seconds
*/
#define os_time_diff_s(old, new) os_time_s(os_time_diff(old, new))

/**
 * Calculate the difference between two os_time_t value in milliseconds.
 * 
 * @param old[in]
 * 		Older os_time_t value
 * 
 * @param new[in]
 * 		Newer os_time_t value
 * 
 * @return Difference between os_time_t old and os_time_t new in milliseconds
*/
#define os_time_diff_ms(old, new) os_time_ms(os_time_diff(old, new))

/**
 * Calculate the difference between two os_time_t value in microseconds.
 * 
 * @param old[in]
 * 		Older os_time_t value
 * 
 * @param new[in]
 * 		Newer os_time_t value
 * 
 * @return Difference between os_time_t old and os_time_t new in microseconds
*/
#define os_time_diff_us(old, new) os_time_us(os_time_diff(old, new))

/**
 * Calculate the difference between two os_time_t value in nanoseconds.
 * 
 * @param old[in]
 * 		Older os_time_t value
 * 
 * @param new[in]
 * 		Newer os_time_t value
 * 
 * @return Difference between os_time_t old and os_time_t new in nanoseconds
*/
#define os_time_diff_ns(old, new) os_time_ns(os_time_diff(old, new))

/**
 * Convert seconds to os_time_t value
 * 
 * @param sec[in]
 * 		Number of seconds
 * 
 * @return os_time_t value
*/
os_time_t os_time_from_s(long sec);

/**
 * Convert milliseconds to os_time_t value
 * 
 * @param msec[in]
 * 		Number of milliseconds
 * 
 * @return os_time_t value
*/
os_time_t os_time_from_ms(long msec);

/**
 * Convert microseconds to os_time_t value
 * 
 * @param usec[in]
 * 		Number of microseconds
 * 
 * @return os_time_t value
*/
os_time_t os_time_from_us(long usec);

/**
 * Convert nanoseconds to os_time_t value
 * 
 * @param nsec[in]
 * 		Number of nanoseconds
 * 
 * @return os_time_t value
*/
os_time_t os_time_from_ns(long nsec);

/**
 * Convert os_time_t value to seconds
 * 
 * @param tval[in]
 * 		os_time_t value to convert
 * 
 * @return seconds
*/
long os_time_s(const os_time_t tval);

/**
 * Convert os_time_t value to milliseconds
 * 
 * @param tval[in]
 * 		os_time_t value to convert
 * 
 * @return milliseconds
*/
long os_time_ms(const os_time_t tval);

/**
 * Convert os_time_t value to microseconds
 * 
 * @param tval[in]
 * 		os_time_t value to convert
 * 
 * @return microseconds
*/
long os_time_us(const os_time_t tval);

/**
 * Convert os_time_t value to nanoseconds
 * 
 * @param tval[in]
 * 		os_time_t value to convert
 * 
 * @return nanoseconds
*/
long os_time_ns(const os_time_t tval);


#endif
