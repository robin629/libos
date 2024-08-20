#include "../../../inc/assert.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* ---------------------------------------------------------- */

void
os_assert_f(const char *commit, const char *file, int line, const char *expr)
{
	/*
		ASSERT[1277c355dde3][src/rtdata/rt_nova_status.c:49]: ((void *)0) == p_value
	*/

	fprintf(stderr, "ASSERT[%s][%s:%d]: %s\n", commit, file, line, expr);
	exit(1);
}
