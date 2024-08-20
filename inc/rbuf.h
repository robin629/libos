#ifndef OS_RBUF_H
#define OS_RBUF_H
#include <stdint.h>

typedef struct
{
	uint8_t *pool;
	uint32_t size;
	uint32_t head;
	uint32_t tail;
} os_rbuf_t;

int os_rbuf_init(os_rbuf_t *p, uint8_t *p_pool, uint32_t pool_size);
int os_rbuf_change_size(os_rbuf_t *p, uint32_t pool_size);
int os_rbuf_flush(os_rbuf_t *p);
uint32_t os_rbuf_used(os_rbuf_t *p);
uint32_t os_rbuf_free(os_rbuf_t *p);
int os_rbuf_push(os_rbuf_t *p, const uint8_t *bytes, uint32_t nbytes);
int os_rbuf_push_u8(os_rbuf_t *p, uint8_t value);
int os_rbuf_push_u16(os_rbuf_t *p, uint16_t value);
int os_rbuf_push_u32(os_rbuf_t *p, uint32_t value);
#ifdef __LP64__
int os_rbuf_push_u64(os_rbuf_t *p, uint64_t value);
#endif
int os_rbuf_peek(os_rbuf_t *p, uint32_t offset, uint8_t *p_buffer, uint32_t count);
int os_rbuf_peek_u8(os_rbuf_t *p, uint32_t offset, uint8_t *p_value);
int os_rbuf_peek_u16(os_rbuf_t *p, uint32_t offset, uint16_t *p_value);
int os_rbuf_peek_u32(os_rbuf_t *p, uint32_t offset, uint32_t *p_value);
#ifdef __LP64__
int os_rbuf_peek_u64(os_rbuf_t *p, uint32_t offset, uint64_t *p_value);
#endif

/**
 * @param[in] p_buffer
 * 		User provided buffer to copy data to. If this parameter is NULL, the
 * 		data will be popped from the buffer without copying.
*/
int os_rbuf_pop(os_rbuf_t *p, uint8_t *p_buffer, uint32_t count);
int os_rbuf_pop_u8(os_rbuf_t *p, uint8_t *p_value);
int os_rbuf_pop_u16(os_rbuf_t *p, uint16_t *p_value);
int os_rbuf_pop_u32(os_rbuf_t *p, uint32_t *p_value);
#ifdef __LP64__
int os_rbuf_pop_u64(os_rbuf_t *p, uint64_t *p_value);
#endif

#endif
