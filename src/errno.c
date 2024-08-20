#include "../inc/errno.h"

#include <stddef.h>

#define TABLE_SIZE(t) (sizeof(t) / sizeof(char *))

static const char *OS_ERRNO_STR_TABLE[] = {
	"OS_EOK",
	"OS_EERROR",
	"OS_EINVAL",
	"OS_ENOMEM",
	"OS_EMUTEX",
	"OS_EOVERFLOW",
	"OS_ENOENT",
	"OS_EAGAIN",
	"OS_ENODEV",
	"OS_ENOSUP",
	"OS_EPERM"
};

__thread int os_errno;

/* ------------------------------------------------------------ */

const char *
os_errstr(int value)
{
	if (value < 0 || (size_t)value >= TABLE_SIZE(OS_ERRNO_STR_TABLE))
		return "OS_E???";

	return OS_ERRNO_STR_TABLE[value];
}
