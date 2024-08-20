#ifndef OS_LOG_H
#define OS_LOG_H
#include "mutex.h"

#include <stdarg.h>

#define LOG_TAG_SIZE 64U

/* In accordance to [IA 1021][Logging Levels]. */
typedef enum
{
	OS_LOG_SEV_CRT = 0,
	OS_LOG_SEV_ERR = 1,
	OS_LOG_SEV_WRN = 2,
	OS_LOG_SEV_NTC = 3,
	OS_LOG_SEV_DBG = 4,
	OS_LOG_SEV_TRC = 5
} OS_LOG_SEV;

/* In accordance to [IA 1021][Logging Options]. */
typedef enum
{
	/* Use default log options */
	OS_LOG_OPT_DEFAULT = -1,

	/* Prepend logging context tag to log output */
	OS_LOG_OPT_TAG = (1 << 0),

	/* Prepend severity level name to log output */
	OS_LOG_OPT_SEVERITY	= (1 << 1),

	/* Prepend time and date to log output */
	OS_LOG_OPT_TIME = (1 << 2),

	/* Prepend file name and line number to log output */
	OS_LOG_OPT_FILE	= (1 << 3)
} OS_LOG_OPT;

typedef struct
{
	char tag[LOG_TAG_SIZE + 1U];

	os_mutex_t mutex;

	OS_LOG_SEV level;
	OS_LOG_OPT options;
} os_log_t;

#define os_log_crt(p_log, fmt, ...) os_log(p_log, OS_LOG_SEV_CRT, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define os_log_err(p_log, fmt, ...) os_log(p_log, OS_LOG_SEV_ERR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define os_log_wrn(p_log, fmt, ...) os_log(p_log, OS_LOG_SEV_WRN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define os_log_ntc(p_log, fmt, ...) os_log(p_log, OS_LOG_SEV_NTC, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define os_log_dbg(p_log, fmt, ...) os_log(p_log, OS_LOG_SEV_DBG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define os_log_trc(p_log, fmt, ...) os_log(p_log, OS_LOG_SEV_TRC, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define os_vlog_crt(p_log, fmt, argp) os_vlog(p_log, OS_LOG_SEV_CRT, __FILE__, __LINE__, fmt, argp)
#define os_vlog_err(p_log, fmt, argp) os_vlog(p_log, OS_LOG_SEV_ERR, __FILE__, __LINE__, fmt, argp)
#define os_vlog_wrn(p_log, fmt, argp) os_vlog(p_log, OS_LOG_SEV_WRN, __FILE__, __LINE__, fmt, argp)
#define os_vlog_ntc(p_log, fmt, argp) os_vlog(p_log, OS_LOG_SEV_NTC, __FILE__, __LINE__, fmt, argp)
#define os_vlog_dbg(p_log, fmt, argp) os_vlog(p_log, OS_LOG_SEV_DBG, __FILE__, __LINE__, fmt, argp)
#define os_vlog_trc(p_log, fmt, argp) os_vlog(p_log, OS_LOG_SEV_TRC, __FILE__, __LINE__, fmt, argp)

/**
 * Create a new logging context.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @param[in] tag
 * 		Logging context name.
 * 
 * @param[in] severity
 * 		Initial logging severity level (OS_LOG_xxx enum value).
 * 
 * @param[in] options
 * 		Bitwise OR of logging options (OS_LOG_OPT_xxx enum value bits).
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_ENOMEM
*/
int os_log_init(os_log_t *p, const char *tag, OS_LOG_SEV severity, OS_LOG_OPT options);

/**
 * Destroy a logging context create using the os_log_init() function.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
*/
int os_log_destroy(os_log_t *p);

/**
 * Get the current options for a logging context.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @param[out] p_options
 * 		Pointer to caller-provided memory to write the current options to.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
*/
int os_log_get_opts(os_log_t *p, OS_LOG_OPT *p_options);

/**
 * Overwrite the current options for a logging context.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @param[in] options
 * 		New bitwise OR of logging option bits (OS_LOG_OPT_xxx enum value).
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
*/
int os_log_set_opts(os_log_t *p, OS_LOG_OPT options);

/**
 * Get the current severity level for a logging context.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @param[out] p_level
 * 		Pointer to caller-provided memory to write the current severity level to.
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
*/
int os_log_get_level(os_log_t *p, OS_LOG_SEV *p_level);

/**
 * Overwrite the current severity level for a logging context.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @param[in] level
 * 		New logging severity level (OS_LOG_xxx enum value).
 * 
 * @return 0
 * 		Success
 * 
 * @return -1 (os_errno set)
 * 		OS_EINVAL
 * 		OS_EMUTEX
*/
int os_log_set_level(os_log_t *p, OS_LOG_SEV level);

/**
 * Submit log message to logging context.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @param[in] severity
 * 		Severity level of log message (OS_LOG_xxx enum value).
 * 
 * @param[in] file
 * 		Filename of originating log message.
 * 
 * @param[in] line
 * 		File line number of originating log message.
 * 
 * @param[in] fmt
 * 		Log message format string.
*/
void os_log(os_log_t *p, OS_LOG_SEV severity, const char *file, int line, const char *fmt, ...);

/**
 * Submit log message to logging context.
 * 
 * @param[in] p
 * 		Pointer os_log_t object.
 * 
 * @param[in] severity
 * 		Severity level of log message (OS_LOG_xxx enum value).
 * 
 * @param[in] file
 * 		Filename of originating log message.
 * 
 * @param[in] line
 * 		File line number of originating log message.
 * 
 * @param[in] fmt
 * 		Log message format string.
 * 
 * @param[in] argp
 * 		Pointer to varadic arguments for the formatted message string.
*/
void os_vlog(os_log_t *p, OS_LOG_SEV severity, const char *file, int line, const char *fmt, va_list argp);

#endif
