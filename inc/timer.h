#ifndef OS_TIMER_H
#define OS_TIMER_H
#include "mutex.h"
#include "time.h"

#include <stdbool.h>
#include <stdint.h>

/* Timer control block */
typedef struct os_timer_s
{
	uint32_t magic;

	uint32_t enabled:1U,
			 running:1U,
			 expired:1U;

	os_mutex_t mutex;
	os_time_t  wake_time;

	long interval;
} os_timer_t;

/**
 * Create a new os_timer_t object. The timer is controlled using the following functions:
 * 		- os_timer_start()	-	Start the timer and set duration.
 * 		- os_timer_stop()	-	Stop an active timer.
 * 		- os_timer_restart()	-	Restart a timer using the value passed in the last call to os_timer_start().
 * 		- os_timer_running()	-	Check if a timer is currently running.
 * 		- os_timer_expired()	-	Check if a timer has expired.
 * 
 * @param[in] p
 * 		Pointer to os_timer_t object.
 * 
 * @param[in] type
 * 		Type of the timer (OS_TIMER_xxx)
 * 
 * @param[in] name
 * 		Name of the timer. This value can be left NULL if no name is needed.
 * 
 * @param[in] p_func
 * 		Callback function to invoke when timer expires. This value can be left NULL and timer
 * 		status can be read using os_timer_running() and os_timer_expired() functions.
 * 
 * @param[in] p_func_arg
 * 		Pointer to caller provided argument to be passed to the timer handler function when
 * 		invoked on expiration. This value can be left NULL.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_E`INVAL
 * 		OS_ENOMEM
*/
int os_timer_init(os_timer_t *p);

/**
 * Destroy a timer previously created using the os_timer_init() function. If the timer
 * is currently running, the timer will be stopped without invoking timer callback function.
 * 
 * @param[in] p
 * 		Pointer to os_timer_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
*/
int os_timer_destroy(os_timer_t *p);

/**
 * Start running a timer previously created using the os_timer_init() function.
 * 
 * @param[in] p
 * 		Pointer to os_timer_t object.
 * 
 * @param[in] interval
 * 		Number of milliseconds until timer should fire callback and/or expire.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
*/
int os_timer_start(os_timer_t *p, long interval);

/**
 * Stop an existing timer from running.
 * 
 * @param[in] p
 * 		Pointer to os_timer_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
*/
int os_timer_stop(os_timer_t *p);

/**
 * Restart an existing timer with the same interval used in the
 * last call to os_timer_start().
 * 
 * @param[in] p
 * 		Pointer to os_timer_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
*/
int os_timer_restart(os_timer_t *p);

/**
 * Check if a timer is currently running.
 * 
 * @param[in] p
 * 		Pointer to os_timer_t object.
 * 
 * @return true/false
 * 		True when timer is active and not expired, false when timer is inactive or expired.
*/
bool os_timer_running(os_timer_t *p);

/**
 * Check if a timer has expired. Typically used to check for expiration on
 * OS_TIMER_NO_CALLBACK type timers that will never asynchronously fire a callback.
 * Once this function is called, the 'expired' flag of the timer is cleared. Subsequent
 * calls will return false.
 * 
 * @param[in] p
 * 		Pointer to os_timer_t object.
 * 
 * @return true/false
 * 		True when timer has expired, false when still running or timer is inactive
*/
bool os_timer_expired(os_timer_t *p);

#endif
