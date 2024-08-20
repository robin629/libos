#ifndef OS_BYTES_H
#define OS_BYTES_H
#include <stdbool.h>
#include <stdint.h>

#if !defined(__BYTE_ORDER__)
#error "Compiler macro __BYTE_ORDER__ is not defined!"
#endif

/**
 * Create bitfield variable. To access individual bits, simply
 * access the desired bit variable (f_example_flag). To read/write
 * the entire bitfield, use the _raw_ variable in the union.
 *
 * @param type[in]		Data type (uint8_t, uint32_t, etc.)
 * @param name[in]		Name of bitfield
 *
 * @example BITFIELD(uint8_t, example_bits, f_one_bit:1, f_two_bits:2);
 *
 * @returns Bitfield union of _raw_ word and struct of named bits.
*/
#define BITFIELD(type, name, ...) union { type _raw_; struct { type __VA_ARGS__; }; } name

/**
 * Get index of the lowest bit set (1).
 *
 * @param value[in]		Value
 *
 * @retval 0	-	All bits are low (0)
 * @retval >= 1	-	One-based index of lowest bit set (1)
*/
#define BIT_LOWEST(value) __builtin_ffsll(value)

/**
 * Get index of the lowest bit set (1).
 *
 * @param value[in]		Value
 *
 * @retval 0	-	All bits are low (0)
 * @retval >= 1	-	One-based index of lowest bit set (1)
*/
#define BIT_LOWEST_SET(value) BIT_LOWEST(value)

/**
 * Get index of the lowest bit clear (0).
 *
 * @param value[in]		Value
 *
 * @retval 0	-	All bits are high (1)
 * @retval >= 1	-	One-based index of lowest bit clear (0)
*/
#define BIT_LOWEST_CLR(value) BIT_LOWEST(~value)

/**
 * Get number of bits set (1).
 *
 * @param value[in]		Value
 *
 * @retval Number of bits set (1)
*/
#define BIT_COUNT(value) __builtin_popcountll(value)

/**
 * Get number of bits set (1).
 *
 * @param value[in]		Value
 *
 * @retval Number of bits set (1)
*/
#define BIT_COUNT_SET(value) BIT_COUNT(value)

/**
 * Get number of bits clear (0).
 *
 * @param value[in]		Value
 *
 * @retval Number of bits clear (0)
*/
#define BIT_COUNT_CLR(value) BIT_COUNT(~value)


/************************************************************************/
/* 16-bit Word Conversions												*/
/************************************************************************/

#if __ORDER_LITTLE_ENDIAN__ == __BYTE_ORDER__

/**
 * Concatenate two octets into one 16 bit word (big endian).
 *
 * @param bytes[in]
 * 		Pointer to array of 2 octets (e.g. uint8_t *)
 *
 * @return 16-bit big endian representation
*/
#define WORD_BE16(bytes) (uint16_t)( ((uint16_t)(bytes)[0U]) << 8U | ((uint16_t)(bytes)[1U]) )

/**
 * Concatenate two octets into one 16 bit word (little endian).
 *
 * @param bytes[in]
 * 		Pointer to array of 2 octets (e.g. uint8_t *)
 *
 * @return 16-bit little endian representation
*/
#define WORD_LE16(bytes) (uint16_t)( *( (uint16_t *) bytes) )

#elif __ORDER_BIG_ENDIAN__ == __BYTE_ORDER__

/**
 * Concatenate two octets into one 16 bit word (big endian).
 *
 * @param bytes[in]
 * 		Pointer to array of 2 octets (e.g. uint8_t *)
 *
 * @return 16-bit big endian representation
*/
#define WORD_BE16(bytes) (uint16_t)( *( (uint16_t *) bytes) )

/**
 * Concatenate two octets into one 16 bit word (little endian).
 *
 * @param bytes[in]
 * 		Pointer to array of 2 octets (e.g. uint8_t *)
 *
 * @return 16-bit little endian representation
*/
#define WORD_LE16(bytes) (uint16_t)( ((uint16_t)(bytes)[1U]) << 8U | ((uint16_t)(bytes)[0U]) )

#endif


/************************************************************************/
/* 32-bit Word Conversions												*/
/************************************************************************/

#if __ORDER_LITTLE_ENDIAN__ == __BYTE_ORDER__

/**
 * Concatenate four octets into one 32 bit word (big endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 4 octets (e.g. uint8_t *)
 *
 * @return 32-bit big endian representation
*/
#define WORD_BE32(bytes) (uint32_t)(							\
							((uint32_t)(bytes)[0U]) << 24U |	\
							((uint32_t)(bytes)[1U]) << 16U |	\
							((uint32_t)(bytes)[2U]) << 8U  |	\
							((uint32_t)(bytes)[3U]) )

/**
 * Concatenate four octets into one 32 bit word (little endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 4 octets (e.g. uint8_t *)
 *
 * @return 32-bit little endian representation
*/
#define WORD_LE32(bytes) (uint32_t)( *( (uint32_t *) bytes) )

#elif __ORDER_BIG_ENDIAN__ == __BYTE_ORDER__

/**
 * Concatenate four octets into one 32 bit word (big endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 4 octets (e.g. uint8_t *)
 *
 * @return 32-bit big endian representation
*/
#define WORD_BE32(bytes) (uint32_t)( *( (uint32_t *) bytes) )

/**
 * Concatenate four octets into one 32 bit word (little endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 4 octets (e.g. uint8_t *)
 *
 * @return 32-bit little endian representation
*/
#define WORD_LE32(bytes) (uint32_t)(							\
							((uint32_t)(bytes)[3U]) << 24U |	\
							((uint32_t)(bytes)[2U]) << 16U |	\
							((uint32_t)(bytes)[1U]) << 8U  |	\
							((uint32_t)(bytes)[0U]) )

#endif

#if __LP64__

/************************************************************************/
/* 64-bit Word Conversions												*/
/************************************************************************/

#if __ORDER_LITTLE_ENDIAN__ == __BYTE_ORDER__

/**
 * Concatenate eight octets into one 64 bit word (big endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 8 octets (e.g. uint8_t *)
 *
 * @return 64-bit big endian representation
*/
#define WORD_BE64(bytes) (uint64_t)(							\
							((uint64_t)(bytes)[0U]) << 56U |	\
							((uint64_t)(bytes)[1U]) << 48U |	\
							((uint64_t)(bytes)[2U]) << 40U |	\
							((uint64_t)(bytes)[3U]) << 32U |	\
							((uint64_t)(bytes)[4U]) << 24U |	\
							((uint64_t)(bytes)[5U]) << 16U |	\
							((uint64_t)(bytes)[6U]) << 8U  |	\
							((uint64_t)(bytes)[7U]) )

/**
 * Concatenate eight octets into one 64 bit word (little endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 8 octets (e.g. uint8_t *)
 *
 * @return 64-bit little endian representation
*/
#define WORD_LE64(bytes) (uint64_t)( *( (uint64_t *) bytes) )

#elif __ORDER_BIG_ENDIAN__ == __BYTE_ORDER__

/**
 * Concatenate eight octets into one 64 bit word (big endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 8 octets (e.g. uint8_t *)
 *
 * @return 64-bit big endian representation
*/
#define WORD_BE64(bytes) (uint64_t)( *( (uint64_t *) bytes) )

/**
 * Concatenate eight octets into one 64 bit word (little endian)
 *
 * @param bytes[in]
 * 		Pointer to array of 8 octets (e.g. uint8_t *)
 *
 * @return 64-bit little endian representation
*/
#define WORD_LE64(bytes) (uint64_t)(							\
							((uint64_t)(bytes)[7U]) << 56U |	\
							((uint64_t)(bytes)[6U]) << 48U |	\
							((uint64_t)(bytes)[5U]) << 40U |	\
							((uint64_t)(bytes)[4U]) << 32U |	\
							((uint64_t)(bytes)[3U]) << 24U |	\
							((uint64_t)(bytes)[2U]) << 16U |	\
							((uint64_t)(bytes)[1U]) << 8U  |	\
							((uint64_t)(bytes)[0U]) )

#endif

#endif

/************************************************************************/
/* 16-bit Word Unpacking to Byte Array									*/
/************************************************************************/

/**
 * Unpack a 16-bit word into 2 octets
 *
 * @param buffer[in]
 * 		Pointer to array of at least 2 bytes (e.g. uint8_t *)
 *
 * @return Buffer with 2 octets representing the 16-bit word in big endian format
*/
#define PUT_BE16(buffer, value) do { \
			((uint8_t *) buffer)[0U] = (uint8_t)(((uint16_t)value) >> 8U);	\
			((uint8_t *) buffer)[1U] = (uint8_t)(((uint16_t)value));		\
		} while (0)

/**
 * Unpack a 16-bit word into 2 octets
 *
 * @param buffer[in]
 * 		Pointer to array of at least 2 bytes (e.g. uint8_t *)
 *
 * @return Buffer with 2 octets representing the 16-bit word in little endian format
*/
#define PUT_LE16(buffer, value) do { \
			((uint8_t *) buffer)[1U] = (uint8_t)(((uint16_t)value) >> 8U);	\
			((uint8_t *) buffer)[0U] = (uint8_t)(((uint16_t)value)); 		\
		} while (0)

/************************************************************************/
/* 32-bit Word Unpacking to Byte Array									*/
/************************************************************************/

/**
 * Unpack a 32-bit word into 4 octets
 *
 * @param buffer[in]
 * 		Pointer to array of at least 4 bytes (e.g. uint8_t *)
 *
 * @return Buffer with 4 octets representing the 32-bit word in big endian format
*/
#define PUT_BE32(buffer, value) do { \
			((uint8_t *) buffer)[0U] = (uint8_t)(((uint32_t)value) >> 24U);	\
			((uint8_t *) buffer)[1U] = (uint8_t)(((uint32_t)value) >> 16U);	\
			((uint8_t *) buffer)[2U] = (uint8_t)(((uint32_t)value) >> 8U);		\
			((uint8_t *) buffer)[3U] = (uint8_t)(((uint32_t)value));			\
		} while (0)

/**
 * Unpack a 32-bit word into 4 octets
 *
 * @param buffer[in]
 * 		Pointer to array of at least 4 bytes (e.g. uint8_t *)
 *
 * @return Buffer with 4 octets representing the 32-bit word in big endian format
*/
#define PUT_LE32(buffer, value) do { \
			((uint8_t *) buffer)[3U] = (uint8_t)(((uint32_t)value) >> 24U);	\
			((uint8_t *) buffer)[2U] = (uint8_t)(((uint32_t)value) >> 16U);	\
			((uint8_t *) buffer)[1U] = (uint8_t)(((uint32_t)value) >> 8U); 	\
			((uint8_t *) buffer)[0U] = (uint8_t)(((uint32_t)value));			\
		} while (0)

#if __LP64__

/************************************************************************/
/* 64-bit Word Unpacking to Byte Array									*/
/************************************************************************/

/**
 * Unpack a 64-bit word into 8 octets
 *
 * @param buffer[in]
 * 		Pointer to array of at least 8 bytes (e.g. uint8_t *)
 *
 * @return Buffer with 8 octets representing the 64-bit word in big endian format
*/
#define PUT_BE64(buffer, value) do { \
			((uint8_t *) buffer)[0U] = (uint8_t)(((uint64_t)value) >> 56U);	\
			((uint8_t *) buffer)[1U] = (uint8_t)(((uint64_t)value) >> 48U);	\
			((uint8_t *) buffer)[2U] = (uint8_t)(((uint64_t)value) >> 40U);	\
			((uint8_t *) buffer)[3U] = (uint8_t)(((uint64_t)value) >> 32U);	\
			((uint8_t *) buffer)[4U] = (uint8_t)(((uint64_t)value) >> 24U);	\
			((uint8_t *) buffer)[5U] = (uint8_t)(((uint64_t)value) >> 16U);	\
			((uint8_t *) buffer)[6U] = (uint8_t)(((uint64_t)value) >> 8U); 	\
			((uint8_t *) buffer)[7U] = (uint8_t)(((uint64_t)value));			\
		} while (0)

/**
 * Unpack a 64-bit word into 8 octets
 *
 * @param buffer[in]
 * 		Pointer to array of at least 8 bytes (e.g. uint8_t *)
 *
 * @return Buffer with 8 octets representing the 64-bit word in little endian format
*/
#define PUT_LE64(buffer, value) do { \
			((uint8_t *) buffer)[7U] = (uint8_t)(((uint64_t)value) >> 56U);	\
			((uint8_t *) buffer)[6U] = (uint8_t)(((uint64_t)value) >> 48U);	\
			((uint8_t *) buffer)[5U] = (uint8_t)(((uint64_t)value) >> 40U);	\
			((uint8_t *) buffer)[4U] = (uint8_t)(((uint64_t)value) >> 32U);	\
			((uint8_t *) buffer)[3U] = (uint8_t)(((uint64_t)value) >> 24U);	\
			((uint8_t *) buffer)[2U] = (uint8_t)(((uint64_t)value) >> 16U);	\
			((uint8_t *) buffer)[1U] = (uint8_t)(((uint64_t)value) >> 8U); 	\
			((uint8_t *) buffer)[0U] = (uint8_t)(((uint64_t)value));			\
		} while (0)

#endif

/************************************************************************/
/* Word Bit Reversing													*/
/************************************************************************/

/**
 * Reverse Bits in 8-bit word
 *
 * @param value[in]
 * 		8-bit word to reverse
 *
 * @return
 * 		Reversed 8-bit word
*/
#define REVERSE_8(value) __builtin_bitreverse8((uint8_t)(value))

/**
 * Reverse Bits in 16-bit word
 *
 * @param value[in]
 * 		16-bit word to reverse
 *
 * @return
 * 		Reversed 16-bit word
*/
#define REVERSE_16(value) __builtin_bitreverse16((uint16_t)(value))

/**
 * Reverse Bits in 32-bit word
 *
 * @param value[in]
 * 		32-bit word to reverse
 *
 * @return
 * 		Reversed 32-bit word
*/
#define REVERSE_32(value) __builtin_bitreverse32((uint32_t)(value))

#if __LP64__

/**
 * Reverse Bits in 64-bit word
 *
 * @param value[in]
 * 		64-bit word to reverse
 *
 * @return
 * 		Reversed 64-bit word
*/
#define REVERSE_64(value) __builtin_bitreverse64((uint64_t)(value))

#endif

/**
 * Return the big endian representation of a 16 bit value
 *
 * @param value     16 bit value to convert to big endian
 *
 * @return Big endian 16 bit value
*/
uint16_t os_bytes_BE16(uint16_t value);

/**
 * Return the little endian representation of a 16 bit value
 *
 * @param value     16 bit value to convert to little endian
 *
 * @return Little endian 16 bit value
*/
uint16_t os_bytes_LE16(uint16_t value);

/**
 * Return the big endian representation of a 32 bit value
 *
 * @param value     32 bit value to convert to big endian
 *
 * @return Big endian 32 bit value
*/
uint32_t os_bytes_BE32(uint32_t value);

/**
 * Return the little endian representation of a 32 bit value
 *
 * @param value     32 bit value to convert to little endian
 *
 * @return Little endian 32 bit value
*/
uint32_t os_bytes_LE32(uint32_t value);

#ifdef __LP64__

/**
 * Return the big endian representation of a 64 bit value
 *
 * @param value     64 bit value to convert to big endian
 *
 * @return Big endian 64 bit value
*/
uint64_t os_bytes_BE64(uint64_t value);

/**
 * Return the little endian representation of a 64 bit value
 *
 * @param value     64 bit value to convert to little endian
 *
 * @return Little endian 64 bit value
*/
uint64_t os_bytes_LE64(uint64_t value);

#endif

#ifdef __LP64__

/**
 * Pack a variable length of bytes into a word (Big Endian)
 *
 * @param nbytes    Byte array length
 * @param bytes     Array of bytes
 *
 * @return The packed bytes word
*/
uint64_t os_bytes_pack(uint8_t nbytes, const uint8_t *bytes);

/**
 * Pack a variable length of bytes into a word
 *
 * @param nbytes        Byte array length
 * @param bytes         Array of bytes
 * @param big_endian    True = big endian, False = little endian
 *
 * @return The packed bytes word
*/
uint64_t os_bytes_pack_ex(uint8_t nbytes, const uint8_t *bytes, bool big_endian);

/**
 * Unpack a word into variable length array (Big Endian)
 *
 * @param value		Value to unpack
 * @param p_buffer	Buffer to unpack to
 * @param bufsize	Size of buffer
 *
 * @return The length of the unpacked value array
*/
uint8_t os_bytes_unpack(uint64_t value, uint8_t *p_buffer, uint64_t bufsize);

/**
 * Unpack a word into variable length array
 *
 * @param value			Value to unpack
 * @param big_endian	True = big endian, False = little endian
 * @param p_buffer		Buffer to unpack to
 * @param bufsize		Size of buffer
 *
 * @return The length of the unpacked value array
*/
uint8_t os_bytes_unpack_ex(uint64_t value, bool big_endian, uint8_t *p_buffer, uint64_t bufsize);

/**
 * Convert array of bytes to null-terminated ASCII string
 *
 * @param bytes		Array of bytes
 * @param nbytes	Byte array size
 * @param p_buffer	User-provided buffer to write ASCII into
 * @param bufsize	Size of user-provided buffer (including NULL byte)
 *
 * @return True: success, False: provided buffer too small
*/
bool os_bytes_string(const uint8_t *bytes, uint64_t nbytes, char *p_buffer, uint64_t bufsize);

#else

/**
 * Pack a variable length of bytes into a word (Big Endian)
 *
 * @param nbytes    Byte array length
 * @param bytes     Array of bytes
 *
 * @return The packed bytes word
*/
uint32_t os_bytes_pack(uint8_t nbytes, const uint8_t *bytes);

/**
 * Pack a variable length of bytes into a word
 *
 * @param nbytes        Byte array length
 * @param bytes         Array of bytes
 * @param big_endian    True = big endian, False = little endian
 *
 * @return The packed bytes word
*/
uint32_t os_bytes_pack_ex(uint8_t nbytes, const uint8_t *bytes, bool big_endian);

/**
 * Unpack a word into variable length array (Big Endian)
 *
 * @param value		Value to unpack
 * @param p_buffer	Buffer to unpack to
 * @param bufsize	Size of buffer
 *
 * @return The length of the unpacked value array
*/
uint8_t os_bytes_unpack(uint32_t value, uint8_t *p_buffer, uint32_t bufsize);

/**
 * Unpack a word into variable length array
 *
 * @param value			Value to unpack
 * @param big_endian	True = big endian, False = little endian
 * @param p_buffer		Buffer to unpack to
 * @param bufsize		Size of buffer
 * @param allow_zero	If the value is 0, allow the variable amount to be zero (00 vs 01 00)
 *
 * @return The length of the unpacked value array
*/
uint8_t os_bytes_unpack_ex(uint32_t value, bool big_endian, uint8_t *p_buffer, uint32_t bufsize, bool allow_zero);

/**
 * Convert array of bytes to null-terminated ASCII string
 *
 * @param bytes		Array of bytes
 * @param nbytes	Byte array size
 * @param p_buffer	User-provided buffer to write ASCII into
 * @param bufsize	Size of user-provided buffer (including NULL byte)
 *
 * @return True: success, False: provided buffer too small
*/
bool os_bytes_string(const uint8_t *bytes, uint32_t nbytes, char *p_buffer, uint32_t bufsize);

#endif

#endif
