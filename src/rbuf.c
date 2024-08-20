#include "../inc/rbuf.h"
#include "../inc/errno.h"
#include "../inc/bytes.h"

#include "private.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* ------------------------------------------------------------ */

int
os_rbuf_init(os_rbuf_t *p, uint8_t *p_pool, uint32_t pool_size)
{
	if (NULL == p || NULL == p_pool)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Byte pool size must be power of 2 for cursor calculation logic */
	if (0U != (pool_size & (pool_size - 1U)))
	{
		OS_PRV_ERR("os_rbuf_init(): (0U != (pool_size & (pool_size - 1U)))");

		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Initialize byte storage pool */
	p->pool = p_pool;
	p->size = pool_size - 1U;
	p->head = 0U;
	p->tail = 0U;

	return 0;
}

int
os_rbuf_change_size(os_rbuf_t *p, uint32_t pool_size)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Byte pool size must be power of 2 for cursor calculation logic */
	if (0U != (pool_size & (pool_size - 1U)))
	{
		OS_PRV_ERR("os_rbuf_change_size(): (0U != (pool_size & (pool_size - 1U)))");

		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Update byte storage pool */
	p->size = pool_size - 1U;
	p->head = (p->head & p->size);
	p->tail = (p->tail & p->size);

	return 0;
}

int
os_rbuf_flush(os_rbuf_t *p)
{
	if (NULL == p)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Reset byte pool cursor values */
	p->head = 0U;
	p->tail = 0U;

	return 0;
}

uint32_t
os_rbuf_used(os_rbuf_t *p)
{
	if (NULL == p)
		return 0U;

	return (p->tail - p->head) & p->size;
}

uint32_t
os_rbuf_free(os_rbuf_t *p)
{
	if (NULL == p)
		return 0U;

	return ((p->head - p->tail) - 1U) & p->size;
}

int
os_rbuf_push(os_rbuf_t *p, const uint8_t *bytes, uint32_t nbytes)
{
	if (NULL == p || NULL == bytes || 0U == nbytes)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Prevent byte pool overflow */
	if (os_rbuf_free(p) < nbytes)
	{
		/* Indicate buffer will overflow */
		os_errno = OS_EOVERFLOW;

		return -1;
	}

	/* Add bytes to the byte pool */
	for (uint32_t i = 0U; i < nbytes; i++)
	{
		uint32_t ix = (p->tail + i) & p->size;

		p->pool[ix] = bytes[i];
	}

	/* Update the pool tail curser after inserting */
	p->tail = (p->tail + nbytes) & p->size;

	return 0;
}

int
os_rbuf_push_u8(os_rbuf_t *p, uint8_t value)
{
	uint8_t tmp[1U] = {value};

	return os_rbuf_push(p, tmp, sizeof(tmp));
}

int
os_rbuf_push_u16(os_rbuf_t *p, uint16_t value)
{
	uint8_t tmp[2U] = {
		(uint8_t)(value >> 8U),
		(uint8_t)(value)
	};

	return os_rbuf_push(p, tmp, sizeof(tmp));
}

int
os_rbuf_push_u32(os_rbuf_t *p, uint32_t value)
{
	uint8_t tmp[4U] = {
		(uint8_t)(value >> 24U),
		(uint8_t)(value >> 16U),
		(uint8_t)(value >> 8U),
		(uint8_t)(value)
	};

	return os_rbuf_push(p, tmp, sizeof(tmp));
}

#ifdef __LP64__

int
os_rbuf_push_u64(os_rbuf_t *p, uint64_t value)
{
	uint8_t tmp[8U] = {
		(uint8_t)(value >> 56U),
		(uint8_t)(value >> 48U),
		(uint8_t)(value >> 40U),
		(uint8_t)(value >> 32U),
		(uint8_t)(value >> 24U),
		(uint8_t)(value >> 16U),
		(uint8_t)(value >> 8U),
		(uint8_t)(value)
	};

	return os_rbuf_push(p, tmp, sizeof(tmp));
}

#endif

int
os_rbuf_peek(os_rbuf_t *p, uint32_t offset, uint8_t *p_buffer, uint32_t count)
{
	if (NULL == p || NULL == p_buffer || 0U == count)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Ensure the byte pool has enough data to read (from the desired offset) */
	if (os_rbuf_used(p) + offset < count)
	{
		/* Set os_errno to indicate not enough data in pool */
		os_errno = OS_ENOENT;

		return -1;
	}

	/* Copy bytes to caller's buffer */
	for (uint32_t i = 0U; i < count; i++)
		p_buffer[i] = p->pool[(p->head + i + offset) & p->size];

	return 0;
}

int
os_rbuf_peek_u8(os_rbuf_t *p, uint32_t offset, uint8_t *p_value)
{
	if (NULL == p_value)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	return os_rbuf_peek(p, offset, p_value, 1U);
}

int
os_rbuf_peek_u16(os_rbuf_t *p, uint32_t offset, uint16_t *p_value)
{
	uint8_t tmp[2U];

	if (NULL == p_value)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (-1 == os_rbuf_peek(p, offset, tmp, sizeof(tmp)))
		return -1;

	*p_value = WORD_BE16(tmp);

	return 0;
}

int
os_rbuf_peek_u32(os_rbuf_t *p, uint32_t offset, uint32_t *p_value)
{
	uint8_t tmp[4U];

	if (NULL == p_value)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (-1 == os_rbuf_peek(p, offset, tmp, sizeof(tmp)))
		return -1;

	*p_value = WORD_BE32(tmp);

	return 0;
}

#ifdef __LP64__

int
os_rbuf_peek_u64(os_rbuf_t *p, uint32_t offset, uint64_t *p_value)
{
	uint8_t tmp[8U];

	if (NULL == p_value)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (-1 == os_rbuf_peek(p, offset, tmp, sizeof(tmp)))
		return -1;

	*p_value = WORD_BE64(tmp);

	return 0;
}

#endif

int
os_rbuf_pop(os_rbuf_t *p, uint8_t *p_buffer, uint32_t count)
{
	if (NULL == p || NULL == p_buffer || 0U == count)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	/* Ensure the byte pool has enough data to read */
	if (os_rbuf_used(p) < count)
	{
		/* Set os_errno to indicate not enough data in pool */
		os_errno = OS_ENOENT;

		return -1;
	}

	/* Copy bytes to caller's buffer */
	for (uint32_t i = 0U; i < count; i++)
		p_buffer[i] = p->pool[(p->head + i) & p->size];

	/* Update the byte pool head cursor */
	p->head = (p->head + count) & p->size;

	return 0;
}

int
os_rbuf_pop_u8(os_rbuf_t *p, uint8_t *p_value)
{
	return os_rbuf_pop(p, p_value, 1U);
}

int
os_rbuf_pop_u16(os_rbuf_t *p, uint16_t *p_value)
{
	uint8_t tmp[2U];

	if (NULL == p_value)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (-1 == os_rbuf_pop(p, tmp, sizeof(tmp)))
		return -1;

	*p_value = WORD_BE16(tmp);

	return 0;
}

int
os_rbuf_pop_u32(os_rbuf_t *p, uint32_t *p_value)
{
	uint8_t tmp[4U];

	if (NULL == p_value)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (-1 == os_rbuf_pop(p, tmp, sizeof(tmp)))
		return -1;

	*p_value = WORD_BE32(tmp);

	return 0;
}

#ifdef __LP64__

int
os_rbuf_pop_u64(os_rbuf_t *p, uint64_t *p_value)
{
	uint8_t tmp[8U];

	if (NULL == p_value)
	{
		/* Set os_errno to indicate invalid arguments */
		os_errno = OS_EINVAL;

		return -1;
	}

	if (-1 == os_rbuf_pop(p, tmp, sizeof(tmp)))
		return -1;

	*p_value = WORD_BE64(tmp);

	return 0;
}

#endif
