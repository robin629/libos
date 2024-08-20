#ifndef OS_MUTEX_H
#define OS_MUTEX_H
#include <pthread.h>
#include <stdint.h>

/* Mutex control block */
typedef struct os_mutex_s
{
	pthread_mutex_t mutex;
} os_mutex_t;

/**
 * Initialize a new mutex. The mutex is recursive.
 * 
 * @param[in] p
 * 		Pointer to os_mutex_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL	-	Invalid arguments
 * 		OS_ENOMEM	-	Failure to allocate memory for serial port context
 * 		OS_EERROR	-	Unspecified internal error
*/
int os_mutex_init(os_mutex_t *p);

/**
 * Destroy a mutex previously created using the os_mutex_init() function.
 * 
 * @param[in] p
 * 		Pointer to os_mutex_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL	-	Invalid arguments
*/
int os_mutex_destroy(os_mutex_t *p);

/**
 * Lock a mutex (previously created using the os_mutex_init() function).
 * 
 * @param[in] p
 * 		Pointer to os_mutex_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL	-	Invalid arguments
 * 		OS_EERROR	-	Unspecified internal error
*/
int os_mutex_lock(os_mutex_t *p);

/**
 * Unlock a mutex (previously locked using the os_mutex_lock() function).
 * 
 * @param[in] p
 * 		Pointer to os_mutex_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL	-	Invalid arguments
 * 		OS_EERROR	-	Unspecified internal error
*/
int os_mutex_unlock(os_mutex_t *p);

#endif
