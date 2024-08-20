#include "../inc/bytes.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* ---------------------------------------------------------- */

uint16_t
os_bytes_BE16(uint16_t value)
{
#if __ORDER_BIG_ENDIAN__ == __BYTE_ORDER__
	return value;
#endif

	/**
	 * Value: 0x1234
	 *
	 * BE: 0x1234
	 * LE: 0x3412
	*/

	return (uint16_t)((value & 0xFF00U) >> 8U) | ((value & 0x00FFU) << 8U);
}

uint16_t
os_bytes_LE16(uint16_t value)
{
#if __ORDER_LITTLE_ENDIAN__ == __BYTE_ORDER__
	return value;
#endif

	/**
	 * Value: 0x1234
	 *
	 * BE: 0x1234
	 * LE: 0x3412
	*/

	return (uint16_t)((value & 0xFF00U) >> 8U) | ((value & 0x00FFU) << 8U);
}

uint32_t
os_bytes_BE32(uint32_t value)
{
#if __ORDER_BIG_ENDIAN__ == __BYTE_ORDER__
	return value;
#endif

	/**
	 * Value: 0x12345678
	 *
	 * BE: 0x12345678
	 * LE: 0x78563412
	*/

	return (uint32_t)((value & 0xFF000000U) >> 24U) |
					((value & 0x00FF0000U) >> 8U) |
					((value & 0x0000FF00U) << 8U) |
					((value & 0x000000FFU) << 24U);
}

uint32_t
os_bytes_LE32(uint32_t value)
{
#if __ORDER_LITTLE_ENDIAN__ == __BYTE_ORDER__
	return value;
#endif

	/**
	 * Value: 0x12345678
	 *
	 * BE: 0x12345678
	 * LE: 0x78563412
	*/

	return (uint32_t)((value & 0xFF000000U) >> 24U) |
					((value & 0x00FF0000U) >> 8U) |
					((value & 0x0000FF00U) << 8U) |
					((value & 0x000000FFU) << 24U);
}

#ifdef __LP64__

uint64_t
os_bytes_BE64(uint64_t value)
{
#if __ORDER_BIG_ENDIAN__ == __BYTE_ORDER__
	return value;
#endif

	/**
	 * Value: 0x123456789ABCDEF1
	 *
	 * BE: 0x123456789ABCDEF1
	 * LE: 0xF1DEBC9A78563412
	*/

	return (uint64_t) ((value & 0xFF00000000000000ULL) >> 56U) |
					((value & 0x00FF000000000000ULL) >> 40U) |
					((value & 0x0000FF0000000000ULL) >> 24U) |
					((value & 0x000000FF00000000ULL) >> 8U) |
					((value & 0x00000000FF000000ULL) << 8U) |
					((value & 0x0000000000FF0000ULL) << 24U) |
					((value & 0x000000000000FF00ULL) << 40U) |
					((value & 0x00000000000000FFULL) << 56U);
}

uint64_t
os_bytes_LE64(uint64_t value)
{
#if __ORDER_LITTLE_ENDIAN__ == __BYTE_ORDER__
	return value;
#endif

	/**
	 * Value: 0x123456789ABCDEF1
	 *
	 * BE: 0x123456789ABCDEF1
	 * LE: 0xF1DEBC9A78563412
	*/

	return (uint64_t) ((value & 0xFF00000000000000ULL) >> 56U) |
					((value & 0x00FF000000000000ULL) >> 40U) |
					((value & 0x0000FF0000000000ULL) >> 24U) |
					((value & 0x000000FF00000000ULL) >> 8U) |
					((value & 0x00000000FF000000ULL) << 8U) |
					((value & 0x0000000000FF0000ULL) << 24U) |
					((value & 0x000000000000FF00ULL) << 40U) |
					((value & 0x00000000000000FFULL) << 56U);
}

uint64_t
os_bytes_pack(uint8_t nbytes, const uint8_t *bytes)
{
	uint64_t retval = 0U;

	if (NULL == bytes || nbytes < 1U || nbytes > 8U)
		return 0U;

	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		// 256: 02 00 01

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint64_t) bytes[nbytes - 1U - i]) << (i * 8U);
	#else
		// 256: 02 01 00

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint64_t) bytes[i]) << (i * 8U);
	#endif

	return retval;
}

uint64_t
os_bytes_pack_ex(uint8_t nbytes, const uint8_t *bytes, bool big_endian)
{
	uint64_t retval = 0U;

	if (NULL == bytes || nbytes < 1U || nbytes > 8U)
		return 0U;

	if (big_endian && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	{
		// 256: 02 00 01

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint64_t) bytes[nbytes - 1U - i]) << (i * 8U);
	}
	else
	{
		// 256: 02 01 00

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint64_t) bytes[i]) << (i * 8U);
	}

	return retval;
}

uint8_t
os_bytes_unpack(uint64_t value, uint8_t *p_buffer, uint64_t bufsize)
{
	if (NULL == p_buffer)
		return 0U;

	/*
		LE: 256 = 00 01
		BE: 256 = 01 00
	*/

	if (value <= 0xFF)
	{
		if (bufsize < 2U)
			return 0U;

		p_buffer[0U] = 1U;
		p_buffer[1U] = value;

		return 2U;
	}
	else if (value <= 0xFFFF)
	{
		if (bufsize < 3U)
			return 0U;

		p_buffer[0U] = 2U;
		p_buffer[1U] = value >> 8U;
		p_buffer[2U] = value;

		return 3U;
	}
	else if (value <= 0xFFFFFF)
	{
		if (bufsize < 4U)
			return 0U;

		p_buffer[0U] = 3U;
		p_buffer[1U] = value >> 16U;
		p_buffer[2U] = value >> 8U;
		p_buffer[3U] = value;

		return 4U;
	}
	else if (value <= 0xFFFFFFFF)
	{
		if (bufsize < 5U)
			return 0U;

		p_buffer[0U] = 4U;
		p_buffer[1U] = value >> 24U;
		p_buffer[2U] = value >> 16U;
		p_buffer[3U] = value >> 8U;
		p_buffer[4U] = value;

		return 5U;
	}
	else if (value <= 0xFFFFFFFFFF)
	{
		if (bufsize < 6U)
			return 0U;

		p_buffer[0U] = 5U;
		p_buffer[1U] = value >> 32U;
		p_buffer[2U] = value >> 24U;
		p_buffer[3U] = value >> 16U;
		p_buffer[4U] = value >> 8U;
		p_buffer[5U] = value;

		return 6U;
	}
	else if (value <= 0xFFFFFFFFFFFF)
	{
		if (bufsize < 7U)
			return 0U;

		p_buffer[0U] = 6U;
		p_buffer[1U] = value >> 40U;
		p_buffer[1U] = value >> 32U;
		p_buffer[2U] = value >> 24U;
		p_buffer[3U] = value >> 16U;
		p_buffer[4U] = value >> 8U;
		p_buffer[5U] = value;

		return 7U;
	}
	else if (value <= 0xFFFFFFFFFFFFFF)
	{
		if (bufsize < 8U)
			return 0U;

		p_buffer[0U] = 7U;
		p_buffer[1U] = value >> 48U;
		p_buffer[2U] = value >> 40U;
		p_buffer[3U] = value >> 32U;
		p_buffer[4U] = value >> 24U;
		p_buffer[5U] = value >> 16U;
		p_buffer[6U] = value >> 8U;
		p_buffer[7U] = value;

		return 8U;
	}
	else
	{
		if (bufsize < 9U)
			return 0U;

		p_buffer[0U] = 8U;
		p_buffer[1U] = value >> 56U;
		p_buffer[2U] = value >> 48U;
		p_buffer[3U] = value >> 40U;
		p_buffer[4U] = value >> 32U;
		p_buffer[5U] = value >> 24U;
		p_buffer[6U] = value >> 16U;
		p_buffer[7U] = value >> 8U;
		p_buffer[8U] = value;

		return 9U;
	}
}

uint8_t
os_bytes_unpack_ex(uint64_t value, bool big_endian, uint8_t *p_buffer, uint64_t bufsize)
{
	if (NULL == p_buffer)
		return 0U;

	/*
		LE: 256 = 00 01
		BE: 256 = 01 00
	*/

	if (value <= 0xFF)
	{
		if (bufsize < 2U)
			return 0U;

		p_buffer[0U] = 1U;
		p_buffer[1U] = value;

		return 2U;
	}
	else if (value <= 0xFFFF)
	{
		if (bufsize < 3U)
			return 0U;

		p_buffer[0U] = 2U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 8U;
			p_buffer[2U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
		}

		return 3U;
	}
	else if (value <= 0xFFFFFF)
	{
		if (bufsize < 4U)
			return 0U;

		p_buffer[0U] = 3U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 16U;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
		}

		return 4U;
	}
	else if (value <= 0xFFFFFFFF)
	{
		if (bufsize < 5U)
			return 0U;

		p_buffer[0U] = 4U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 24U;
			p_buffer[2U] = value >> 16U;
			p_buffer[3U] = value >> 8U;
			p_buffer[4U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
			p_buffer[4U] = value >> 24U;
		}

		return 5U;
	}
	else if (value <= 0xFFFFFFFFFF)
	{
		if (bufsize < 6U)
			return 0U;

		p_buffer[0U] = 5U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 32U;
			p_buffer[2U] = value >> 24U;
			p_buffer[3U] = value >> 16U;
			p_buffer[4U] = value >> 8U;
			p_buffer[5U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
			p_buffer[4U] = value >> 24U;
			p_buffer[6U] = value >> 32U;
		}

		return 6U;
	}
	else if (value <= 0xFFFFFFFFFFFF)
	{
		if (bufsize < 7U)
			return 0U;

		p_buffer[0U] = 6U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 40U;
			p_buffer[2U] = value >> 32U;
			p_buffer[3U] = value >> 24U;
			p_buffer[4U] = value >> 16U;
			p_buffer[5U] = value >> 8U;
			p_buffer[6U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
			p_buffer[4U] = value >> 24U;
			p_buffer[6U] = value >> 32U;
			p_buffer[6U] = value >> 40U;
		}

		return 7U;
	}
	else if (value <= 0xFFFFFFFFFFFFFF)
	{
		if (bufsize < 8U)
			return 0U;

		p_buffer[0U] = 7U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 48U;
			p_buffer[2U] = value >> 40U;
			p_buffer[3U] = value >> 32U;
			p_buffer[4U] = value >> 24U;
			p_buffer[5U] = value >> 16U;
			p_buffer[6U] = value >> 8U;
			p_buffer[7U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
			p_buffer[4U] = value >> 24U;
			p_buffer[6U] = value >> 32U;
			p_buffer[6U] = value >> 40U;
			p_buffer[6U] = value >> 48U;
		}

		return 8U;
	}
	else
	{
		if (bufsize < 9U)
			return 0U;

		p_buffer[0U] = 8U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 56U;
			p_buffer[2U] = value >> 48U;
			p_buffer[3U] = value >> 40U;
			p_buffer[4U] = value >> 32U;
			p_buffer[5U] = value >> 24U;
			p_buffer[6U] = value >> 16U;
			p_buffer[7U] = value >> 8U;
			p_buffer[8U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
			p_buffer[4U] = value >> 24U;
			p_buffer[6U] = value >> 32U;
			p_buffer[6U] = value >> 40U;
			p_buffer[6U] = value >> 48U;
			p_buffer[6U] = value >> 56U;
		}

		return 9U;
	}
}

bool
os_bytes_string(const uint8_t *bytes, uint64_t nbytes, char *p_buffer, uint64_t bufsize)
{
	uint64_t min_size = (nbytes * 2U) + 1U;
	uint64_t bufpos = 0U;

	if (NULL == bytes || NULL == p_buffer || bufsize < min_size)
		return false;

	for (uint64_t i = 0U; i < nbytes; i++)
	{
		p_buffer[bufpos] = "0123456789ABCDEF"[(bytes[i] >> 4U) & 0xF];
		p_buffer[bufpos + 1U] = "0123456789ABCDEF"[bytes[i] & 0xF];
		bufpos += 2U;
	}

	/* Add NULL terminator */
	p_buffer[bufpos] = 0;

	return true;
}

#else

uint32_t
os_bytes_pack(uint8_t nbytes, const uint8_t *bytes)
{
	uint32_t retval = 0U;

	if (NULL == bytes || nbytes < 1U || nbytes > 4U)
		return 0U;

	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		// 256: 02 00 01

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint32_t) bytes[nbytes - 1U - i]) << (i * 8U);
	#else
		// 256: 02 01 00

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint32_t) bytes[i]) << (i * 8U);
	#endif

	return retval;
}

uint32_t
os_bytes_pack_ex(uint8_t nbytes, const uint8_t *bytes, bool big_endian)
{
	uint32_t retval = 0U;

	if (NULL == bytes || nbytes < 1U || nbytes > 4U)
		return 0U;

	if (big_endian && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	{
		// 256: 02 00 01

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint32_t) bytes[nbytes - 1U - i]) << (i * 8U);
	}
	else
	{
		// 256: 02 01 00

		for (uint8_t i = 0U; i < nbytes; i++)
			retval += ((uint32_t) bytes[i]) << (i * 8U);
	}

	return retval;
}

uint8_t
os_bytes_unpack(uint32_t value, uint8_t *p_buffer, uint32_t bufsize)
{
	if (NULL == p_buffer)
		return 0U;

	/*
		LE: 256 = 00 01
		BE: 256 = 01 00
	*/

   	if (value <= 0xFF)
	{
		if (bufsize < 2U)
			return 0U;

		p_buffer[0U] = 1U;
		p_buffer[1U] = value;

		return 2U;
	}
	else if (value <= 0xFFFF)
	{
		if (bufsize < 3U)
			return 0U;

		p_buffer[0U] = 2U;
		p_buffer[1U] = value >> 8U;
		p_buffer[2U] = value;

		return 3U;
	}
	else if (value <= 0xFFFFFF)
	{
		if (bufsize < 4U)
			return 0U;

		p_buffer[0U] = 3U;
		p_buffer[1U] = value >> 16U;
		p_buffer[2U] = value >> 8U;
		p_buffer[3U] = value;

		return 4U;
	}
	else
	{
		if (bufsize < 5U)
			return 0U;

		p_buffer[0U] = 4U;
		p_buffer[1U] = value >> 24U;
		p_buffer[2U] = value >> 16U;
		p_buffer[3U] = value >> 8U;
		p_buffer[4U] = value;

		return 5U;
	}
}

uint8_t
os_bytes_unpack_ex(uint32_t value, bool big_endian, uint8_t *p_buffer, uint32_t bufsize, bool allow_zero)
{
	if (NULL == p_buffer)
		return 0U;

	/*
		LE: 256 = 00 01
		BE: 256 = 01 00
	*/

	if (0U == value && allow_zero)
	{
		if (bufsize < 1U)
			return 0U;

		p_buffer[0U] = 0U;

		return 1U;
	}
   	else if (value <= 0xFF)
	{
		if (bufsize < 2U)
			return 0U;

		p_buffer[0U] = 1U;
		p_buffer[1U] = value;

		return 2U;
	}
	else if (value <= 0xFFFF)
	{
		if (bufsize < 3U)
			return 0U;

		p_buffer[0U] = 2U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 8U;
			p_buffer[2U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
		}

		return 3U;
	}
	else if (value <= 0xFFFFFF)
	{
		if (bufsize < 4U)
			return 0U;

		p_buffer[0U] = 3U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 16U;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
		}

		return 4U;
	}
	else
	{
		if (bufsize < 5U)
			return 0U;

		p_buffer[0U] = 4U;

		if (big_endian) /* Big Endian */
		{
			p_buffer[1U] = value >> 24U;
			p_buffer[2U] = value >> 16U;
			p_buffer[3U] = value >> 8U;
			p_buffer[4U] = value;
		}
		else            /* Little Endian */
		{
			p_buffer[1U] = value;
			p_buffer[2U] = value >> 8U;
			p_buffer[3U] = value >> 16U;
			p_buffer[4U] = value >> 24U;
		}

		return 5U;
	}
}

bool
os_bytes_string(const uint8_t *bytes, uint32_t nbytes, char *p_buffer, uint32_t bufsize)
{
	uint32_t min_size = (nbytes * 2U) + 1U;
	uint32_t bufpos = 0U;

	if (NULL == bytes || NULL == p_buffer || bufsize < min_size)
		return false;

	for (uint32_t i = 0U; i < nbytes; i++)
	{
		p_buffer[bufpos] = "0123456789ABCDEF"[(bytes[i] >> 4U) & 0xF];
		p_buffer[bufpos + 1U] = "0123456789ABCDEF"[bytes[i] & 0xF];
		bufpos += 2U;
	}

	/* Add NULL terminator */
	p_buffer[bufpos] = 0;

	return true;
}

#endif
