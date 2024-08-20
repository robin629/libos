#include "../../private.h"

#include "../../../inc/assert.h"
#include "../../../inc/errno.h"
#include "../../../inc/mutex.h"
#include "../../../inc/queue.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if 0 != (OS_QUEUE_MSGID_MAX & (OS_QUEUE_MSGID_MAX - 1U))
#error "OS_QUEUE_MSGID_MAX is not power of 2"
#endif

/* Initialized in runtime.c */
os_mutex_t g_queue_mutex;

static os_queue_t *g_queue_list;

/* ------------------------------------------------------------ */

int
os_queue_init(os_queue_t *p, os_msg_t *p_msg_pool, uint32_t pool_size)
{
	if (NULL == p || NULL == p_msg_pool)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Message pool size must be power of 2 for cursor calculation logic */
	if (0U != (pool_size & (pool_size - 1U)))
	{
		OS_PRV_ERR("os_queue_init(): (0U != (pool_size & (pool_size - 1U)))");

		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Clear queue memory */
	memset(p, 0, sizeof(os_queue_t));

	/* Initialize the queue's message pool */
	p->buffer = p_msg_pool;
	p->size	  = pool_size - 1U;

	/* Lock the global queue list mutex */
	os_assert(0 == os_mutex_lock(&g_queue_mutex));

	/* Assign the next queue as the old head (NULL if list was empty) */
	p->next = g_queue_list;

	/* Reassign the list head to the new item */
	g_queue_list = p;

	/* Unlock the global queue list mutex */
	os_assert(0 == os_mutex_unlock(&g_queue_mutex));

	return 0;
}

int
os_queue_destroy(os_queue_t *p)
{
	os_queue_t *prv = NULL;
	os_queue_t *tmp;

	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Lock the global queue list mutex */
	os_assert(0 == os_mutex_lock(&g_queue_mutex));

	/* Grab address of first queue in global queue list */
	tmp = g_queue_list;

	/* Loop through queue list and find our queue */
	while (NULL != tmp)
	{
		if (p == tmp)
		{
			/* Check if this queue is currently the queue list head */
			if (p == g_queue_list)
				g_queue_list = tmp->next;

			/* Update the address of previous node (if possible) */
			if (NULL != prv)
				prv->next = tmp->next;

			/* Clear memory */
			memset(p, 0, sizeof(*p));

			/* Found queue; stop traversing queue list */
			break;
		}

		/* Update the previous node address */
		prv = tmp;

		/* Move to next queue in the list */
		tmp = tmp->next;
	}

	/* Unlock the global queue list mutex */
	os_assert(0 == os_mutex_unlock(&g_queue_mutex));

	return 0;
}

int
os_queue_sub(os_queue_t *p, uint32_t id)
{
	uint32_t off = id / 32U;
	uint32_t bit = id & 7U;

	if (NULL == p || id > (OS_QUEUE_MSGID_MAX-1U))
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Lock the global queue list mutex */
	os_assert(0 == os_mutex_lock(&g_queue_mutex));

	/* Enable notifications for this message ID */
	p->subscriptions[off] |= (1U << bit);

	/* Unlock the global queue list mutex */
	os_assert(0 == os_mutex_unlock(&g_queue_mutex));

	return 0;
}

int
os_queue_unsub(os_queue_t *p, uint32_t id)
{
	uint32_t off = id / 32U;
	uint32_t bit = id & 7U;

	if (NULL == p || id > (OS_QUEUE_MSGID_MAX-1U))
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Lock the global queue list mutex */
	os_assert(0 == os_mutex_lock(&g_queue_mutex));

	/* Disable notifications for this message ID */
	p->subscriptions[off] &= ~(1U << bit);

	/* Unlock the global queue list mutex */
	os_assert(0 == os_mutex_unlock(&g_queue_mutex));

	return 0;
}

int
os_queue_recv(os_queue_t *p, os_msg_t *p_msg)
{
	int err = 0;

	if (NULL == p || NULL == p_msg)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Lock the global queue list mutex */
	os_assert(0 == os_mutex_lock(&g_queue_mutex));

	/* No messages in the queue */
	if (p->head == p->tail)
	{
		/* Set os_errno to indicate no messages waiting */
		os_errno = OS_EAGAIN;

		err = -1;
	}
	else
	{
		/* Copy the message from queue to caller */
		memcpy(p_msg, &p->buffer[p->head], sizeof(*p_msg));

		/* Update the queue's read index */
		p->head = (p->head + 1U) & p->size;
	}

	/* Unlock the global queue list mutex */
	os_assert(0 == os_mutex_unlock(&g_queue_mutex));

	return err;
}

int
os_queue_send(os_queue_t *p, os_msg_t *msg)
{
	os_queue_t *tmp;
	int err = -2;	// Local to this func, -2 = no queue found

	if (NULL == p || NULL == msg)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Lock the global queue list mutex */
	os_assert(0 == os_mutex_lock(&g_queue_mutex));

	/* Ensure the 'source' field is pointing to the correct queue */
	msg->source = p;

	/* Grab address of first queue in global queue list */
	tmp = g_queue_list;

	/* Traverse the queue list, find the queue by address */
	while (NULL != tmp)
	{
		if (msg->target == tmp)
		{
			/* Copy message to the target's buffer */
			memcpy(&tmp->buffer[tmp->tail], msg, sizeof(*msg));

			/* Update the target queue's write index */
			tmp->tail = (tmp->tail + 1U) & tmp->size;

			/* Found target queue; set error to 0 indicating queue found */
			err = 0;

			break;
		}

		tmp = tmp->next;
	}

	/* Unlock the global queue list mutex */
	os_assert(0 == os_mutex_unlock(&g_queue_mutex));

	/* Condition when no queue was found in list */
	if (-2 == err)
	{
		/* Set os_errno to indicate no queue found in list */
		os_errno = OS_ENOENT;

		return -1;
	}

	return err;
}

int
os_queue_sendv(os_queue_t *p, os_queue_t *dst, uint32_t userdata, uint32_t id, uint32_t param_count, ...)
{
	os_msg_t qmsg;
	va_list  argp;

	if (NULL == p || NULL == dst || param_count > OS_QUEUE_PARAM_COUNT)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	memset(&qmsg, 0, sizeof(qmsg));

	/* Save the message params */
	qmsg.userdata = userdata;
	qmsg.source	  = p;
	qmsg.target   = dst;
	qmsg.id 	  = id;

	/* Start the stack varargs list */
	va_start(argp, param_count);

	/* Copy params from stack to the message buffer */
	for (uint32_t i = 0U; i < param_count; i++)
		qmsg.params[i] = va_arg(argp, uint32_t);

	/* End the stack list */
	va_end(argp);

	/* Post the actual message */
	return os_queue_send(p, &qmsg);
}

int
os_queue_post(os_queue_t *p, os_msg_t *msg)
{
	os_queue_t *tmp;
	uint32_t off;
	uint32_t bit;

	if (NULL == p || NULL == msg || msg->id > (OS_QUEUE_MSGID_MAX-1U))
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Calculate subscriptions table index and bit */
	off = msg->id / 32U;
	bit = msg->id & 7U;

	/* Lock the global queue list mutex */
	os_assert(0 == os_mutex_lock(&g_queue_mutex));

	/* Ensure the 'source' field is pointing to the correct queue */
	msg->source = p;

	/* Grab address of first queue in global queue list */
	tmp = g_queue_list;

	/* Traverse the queue list, find the queue by address */
	while (NULL != tmp)
	{
		/* Only notify if the queue is subscribed to this event */
		if (0U != ((tmp->subscriptions[off] >> bit) & 1U))
		{
			/* Copy notification to the target's buffer */
			memcpy(&tmp->buffer[tmp->tail], msg, sizeof(*msg));

			/* Update the target queue's write index */
			tmp->tail = (tmp->tail + 1U) & tmp->size;
		}

		tmp = tmp->next;
	}

	/* Unlock the global queue list mutex */
	os_assert(0 == os_mutex_unlock(&g_queue_mutex));

	return 0;
}

int
os_queue_postv(os_queue_t *p, uint32_t id, uint32_t param_count, ...)
{
	os_msg_t qmsg;
	va_list  argp;

	if (NULL == p || param_count > OS_QUEUE_PARAM_COUNT)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	memset(&qmsg, 0, sizeof(qmsg));

	/* Save the message information */
	qmsg.source = p;
	qmsg.target = NULL;
	qmsg.id = id;

	/* Start the stack varargs list */
	va_start(argp, param_count);

	/* Copy params from stack to the message buffer */
	for (uint32_t i = 0U; i < param_count; i++)
		qmsg.params[i] = va_arg(argp, uint32_t);

	/* End the stack list */
	va_end(argp);

	/* Post the actual message */
	return os_queue_post(p, &qmsg);
}
