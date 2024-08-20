#include "../../inc/log.h"
#include "../../inc/mutex.h"
#include "../../inc/errno.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define LOG_OPTS_DEFAULT (OS_LOG_OPT_TIME_DATE | OS_LOG_OPT_TAG | OS_LOG_OPT_SEVERITY)

static const char *LOG_SEV_STR_TABLE[] = {
	"CRT",
	"ERR",
	"WRN",
	"NTC",
	"DBG",
	"TRC"
};

/* ---------------------------------------------------------- */

int
os_log_init(os_log_t *p, const char *tag, OS_LOG_SEV severity, OS_LOG_OPT options)
{
	if (NULL == p || NULL == tag)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Clear out memory of new log control block */
	memset(p, 0, sizeof(os_log_t));

	/* Copy tag to the log tag cache (snprintf will NULL terminate for us) */
	snprintf(p->tag, sizeof(p->tag), "%s", tag);

	/* Set initial log severity level */
	p->level = severity;

	/* Initialize the log control block mutex */
	if (-1 == os_mutex_init(&(p->mutex)))
	{
		/* Set os_errno to indicate failure to initialize mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Write options to log control block */
	if (OS_LOG_OPT_DEFAULT == options)
		p->options = OS_LOG_OPT_DEFAULT;
	else
		p->options = options;

	return 0;
}

int
os_log_destroy(os_log_t *p)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Destroy mutex used to protect log buffer/output */
	os_mutex_destroy(&(p->mutex));

	/* Clear memory */
	memset(p, 0, sizeof(*p));

	return 0;
}

int
os_log_get_opts(os_log_t *p, OS_LOG_OPT *p_options)
{
	if (NULL == p || NULL == p_options)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain log context mutex */
	if (0 != os_mutex_lock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Copy current option to caller-provided buffer */
	*p_options = p->options;

	/* Release log context mutex */
	if (0 != os_mutex_unlock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

int
os_log_set_opts(os_log_t *p, OS_LOG_OPT options)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain log context mutex */
	if (0 != os_mutex_lock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Overwrite old options */
	p->options = options;

	/* Release log context mutex */
	if (0 != os_mutex_unlock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

int
os_log_get_level(os_log_t *p, OS_LOG_SEV *p_level)
{
	if (NULL == p || NULL == p_level)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain log context mutex */
	if (0 != os_mutex_lock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Copy current severity level to caller-provided buffer */
	*p_level = p->level;

	/* Release log context mutex */
	if (0 != os_mutex_unlock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

int
os_log_set_level(os_log_t *p, OS_LOG_SEV level)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Obtain log context mutex */
	if (0 != os_mutex_lock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	/* Overwrite old severity level */
	p->level = level;

	/* Release log context mutex */
	if (0 != os_mutex_unlock(&p->mutex))
	{
		/* Set os_errno to indicate unable to obtain/release resource mutex */
		os_errno = OS_EMUTEX;

		return -1;
	}

	return 0;
}

void
os_log(os_log_t *p, OS_LOG_SEV severity, const char *file, int line, const char *fmt, ...)
{
	va_list argp;

	if (NULL == p || NULL == file || NULL == fmt)
		return;

	if (severity < OS_LOG_SEV_CRT || severity > OS_LOG_SEV_TRC)
		return;

	va_start(argp, fmt);
	os_vlog(p, severity, file, line, fmt, argp);
	va_end(argp);
}

void
os_vlog(os_log_t *p, OS_LOG_SEV severity, const char *file, int line, const char *fmt, va_list argp)
{
	bool add_col = false;

	/* Obtain log context mutex */
	if (0 != os_mutex_lock(&p->mutex))
	{
		return;
	}

	/* Prepend time & date string */
	if (0 != (p->options & OS_LOG_OPT_TIME))
	{
		struct tm tm_now;
		char 	  tmp[100U];
		time_t 	  now;

		/* Get current UNIX time */
		now = time(NULL);

		/* Get localized date, then format for readability */

		if (NULL == localtime_r(&now, &tm_now))
			fprintf(stderr, "[TIME/DATE ERR]");
		else if (0U == strftime(tmp, sizeof(tmp), "%F %T", &tm_now))
			fprintf(stderr, "[TIME/DATE ERR]");
		else
			fprintf(stderr, "[%s]", tmp);

		add_col = true;
	}

	/* Prepend context tag name string */
	if (0 != (p->options & OS_LOG_OPT_TAG))
	{
		fprintf(stderr, "[%s]", p->tag);

		add_col = true;
	}

	/* Prepend severity level string */
	if (0 != (p->options & OS_LOG_OPT_SEVERITY))
	{
		if (severity < OS_LOG_SEV_CRT || severity >= (sizeof(LOG_SEV_STR_TABLE)/sizeof(char *)))
			fprintf(stderr, "[SEV ERR: %d]", severity);
		else
			fprintf(stderr, "[%s]", LOG_SEV_STR_TABLE[severity]);

		add_col = true;
	}

	/* Prepend filename string */
	if (0 != (p->options & OS_LOG_OPT_FILE))
	{
		fprintf(stderr, " %s:%d", file, line);

		add_col = true;
	}

	/* Prepend colon to separate log message info from the actual log message */
	if (add_col)
	{
		fprintf(stderr, ": ");
	}

	/* Add formatted log message */
	vfprintf(stderr, fmt, argp);

	/* Append newline character */
	fprintf(stderr, "\n");

	/* Release log context mutex */
	if (0 != os_mutex_unlock(&p->mutex))
	{
		return;
	}
}
