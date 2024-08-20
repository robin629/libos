#include "../../../inc/assert.h"
#include "../../../inc/errno.h"
#include "../../../inc/mutex.h"

#include "../../private.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>

/* User defined program enter/setup */
extern int os_runtime_enter();

/* User defined program exit/teardown */
extern int os_runtime_exit();

/* Defined in queue.c */
extern os_mutex_t g_queue_mutex;

static pthread_mutex_t 	g_runtime_mutex = PTHREAD_MUTEX_INITIALIZER;
static sigset_t			g_sig_set;
static bool				g_exit_flag;

/* ------------------------------------------------------------ */

bool
os_runtime_exiting()
{
	bool retval;

	/* Obtain runtime mutex */
	if (0 != pthread_mutex_lock(&g_runtime_mutex))
	{
		OS_PRV_ABORT("Failed to lock mutex!");
	}

	/* Read current exit flag while holding mutex */
	retval = g_exit_flag;

	/* Release runtime mutex */
	if (0 != pthread_mutex_unlock(&g_runtime_mutex))
	{
		OS_PRV_ABORT("Failed to unlock mutex!");
	}

	return retval;
}

static int
os_runtime_init()
{
	OS_PRV_DBG("Initializing runtime");

	/* Initialize exit flag */
	g_exit_flag = false;

	/* Initialize signal handling */
	sigemptyset(&g_sig_set);

	/* Fill the signal set with the ones we want to block */
	sigaddset(&g_sig_set, SIGINT);
	sigaddset(&g_sig_set, SIGTERM);

	/* Block the signals we want to handle BEFORE creating any threads */
	if (0 != pthread_sigmask(SIG_BLOCK, &g_sig_set, NULL))
	{
		OS_PRV_CRT("Failed to set signal mask for main thread");

		/* Set os_errno to the generic error value */
		os_errno = OS_EERROR;

		return -1;
	}

	/* Initialize the global queue list mutex */
	os_assert(0 == os_mutex_init(&g_queue_mutex));

	return 0;
}

static int
os_runtime_destroy()
{
	OS_PRV_DBG("Destroying runtime");

	/* Destroy the global queue list mutex */
	os_assert(0 == os_mutex_destroy(&g_queue_mutex));

	return 0;
}

static void
os_runtime_loop()
{
	int sig;

	OS_PRV_DBG("Enter runtime loop");

	/* Spin until signal received */
	while (1)
	{
		/* Wait for pending signals */
		if (0 != sigwait(&g_sig_set, &sig))
		{
			OS_PRV_CRT("sigwait() returned non-zero! Exiting");

			break;
		}

		/* Handle program termination signals */
		if (SIGINT == sig || SIGTERM == sig)
		{
			OS_PRV_CRT("Caught SIGINT or SIGTERM");

			break;
		}
	}

	/* Obtain runtime mutex */
	if (0 != pthread_mutex_lock(&g_runtime_mutex))
	{
		OS_PRV_ABORT("Failed to lock mutex!");
	}

	/* Update exit flag for all other tasks */
	g_exit_flag = true;

	/* Release runtime mutex */
	if (0 != pthread_mutex_unlock(&g_runtime_mutex))
	{
		OS_PRV_ABORT("Failed to unlock mutex!");
	}

	OS_PRV_DBG("Exit runtime loop");
}

int
main()
{
	OS_PRV_CRT("main: Entering");

	/* Call internal runtime init */
	os_runtime_init();

	/* Call the user-defined runtime program setup */
	os_runtime_enter();

	/* BLOCKING: Loop until program shutdown */
	os_runtime_loop();

	/* Call the user-defined runtime program shutdown */
	os_runtime_exit();

	/* Call internal runtime shutdown */
	os_runtime_destroy();

	OS_PRV_CRT("main: Exiting");
	return 0;
}
