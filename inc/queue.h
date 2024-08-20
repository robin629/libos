#ifndef LIBOS_QUEUE_H
#define LIBOS_QUEUE_H
#include "mutex.h"

#include <stdint.h>

#define OS_QUEUE_MSGID_MAX (8192U)

/* Subscription table holds one bit for each possible event in 32-bit words */
#define OS_QUEUE_SUB_TABLE_SIZE (OS_QUEUE_MSGID_MAX/32U)

#define OS_QUEUE_PARAM_COUNT 128U

typedef struct os_queue_s os_queue_t;

typedef struct
{
	os_queue_t *source;
	os_queue_t *target;
	uint32_t 	userdata;

	uint32_t id;

	union
	{
		uint32_t params[OS_QUEUE_PARAM_COUNT];
		uint8_t  data[OS_QUEUE_PARAM_COUNT*4U];
	};
} os_msg_t;

struct os_queue_s
{
	os_queue_t *next;

	os_msg_t *buffer;
	uint32_t  head;
	uint32_t  tail;
	uint32_t  size;

	uint32_t  subscriptions[OS_QUEUE_SUB_TABLE_SIZE];
};

int os_queue_init(os_queue_t *p, os_msg_t *p_msg_pool, uint32_t pool_size);
int os_queue_destroy(os_queue_t *p);
int os_queue_sub(os_queue_t *p, uint32_t id);
int os_queue_unsub(os_queue_t *p, uint32_t id);
int os_queue_send(os_queue_t *p, os_msg_t *msg);
int os_queue_sendv(os_queue_t *p, os_queue_t *dst, uint32_t userdata, uint32_t id, uint32_t param_count, ...);
int os_queue_recv(os_queue_t *p, os_msg_t *p_msg);
int os_queue_post(os_queue_t *p, os_msg_t *msg);
int os_queue_postv(os_queue_t *p, uint32_t id, uint32_t param_count, ...);

#endif
