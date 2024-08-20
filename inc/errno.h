#ifndef OS_ERRNO_H
#define OS_ERRNO_H

#define OS_EOK	 		((int) 0)		/* No error condition								*/
#define OS_EERROR 		((int) 1)		/* Generic error condition							*/
#define OS_EINVAL 		((int) 2)		/* Invalid argument(s)								*/
#define OS_ENOMEM 		((int) 3)		/* Memory invalid or unable to allocate memory 		*/
#define OS_EMUTEX 		((int) 4)		/* Failed to obtain/init resource mutex				*/
#define OS_EOVERFLOW	((int) 5)		/* Value too large for defined data type			*/
#define OS_ENOENT		((int) 6)		/* No entry found (object, file, directory, etc.)	*/
#define OS_EAGAIN		((int) 7)		/* Resource is busy. Try operation again.			*/
#define OS_ENODEV		((int) 8)		/* No such device.									*/
#define OS_ENOSUP		((int) 9)		/* Not supported.									*/
#define OS_EPERM		((int) 10)		/* Operation not permitted.							*/

/* Thread-local NT error variable */
extern __thread int os_errno;

const char *os_errstr(int value);

#endif
