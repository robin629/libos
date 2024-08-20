#ifndef OS_ASSERT_H
#define OS_ASSERT_H
#include <stdbool.h>

#define os_assert(expr) do {	\
		if (false == (bool)(expr))	\
			os_assert_f(NULL, __FILE__, __LINE__, #expr);	\
	} while (0)

/*
Example:
	ASSERT[src/rtdata/rt_nova_status.c:49]: ((void *)0) == p_value
*/

void os_assert_f(const char *commit, const char *file, int line, const char *expr);

#endif
