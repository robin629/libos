#ifndef OS_CLOCK_H
#define OS_CLOCK_H
#include <stdint.h>

typedef struct
{
	/* Seconds [0-60] (1 leap second)	*/
	uint8_t sec;

	/* Minutes [0-59]					*/
	uint8_t min;

	/* Hours [0-23]						*/
	uint8_t hour;

	/* Day of month [1-31]				*/
	uint8_t mday;

	/* Day of week [0-6]	0 = sunday	*/
	uint8_t wday;

	/* Day of year [0-365]				*/
	uint16_t yday;

	/* Month of year [0-11]				*/
	uint8_t month;

	/* Years since 1900 				*/
	uint32_t year;

	/* Daylight savings time (0 = N/a, 1 = No, 2 = Yes) */
	uint8_t dst;

} os_clock_t;

/**
 * Get local time/date information from the system.
 * 
 * @param[in] p
 * 		Pointer to caller-provided os_clock_t struct.
 * 
 * @retval 0
 * 		Success
 * 
 * @retval -1 (os_errno set)
 * 		NT_EINVAL	-	Invalid arguments
 * 		NT_EERROR	-	Failed to get time/date
*/
int os_clock_get(os_clock_t *p);

/**
 * Write local time/date information to the system.
 * 
 * @param[in] p
 * 		Pointer to caller-provided os_clock_t struct used to set system time/date.
 * 
 * @retval 0
 * 		Success
 * 
 * @retval -1 (os_errno set)
 * 		NT_EINVAL	-	Invalid arguments
 * 		NT_EERROR	-	Failed to set time/date
*/
int os_clock_set(os_clock_t *p);

#endif
