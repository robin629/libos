#ifndef OS_OVERFLOW_H
#define OS_OVERFLOW_H
#include <stdint.h>
#include <limits.h>

/**
 * Check for overflow when adding two signed numbers.
 * 
 * @param[in] max
 * 		Signed number maximum value (e.g IOS_MAX, found in <limits.h>)
 * 
 * @param[in] min
 * 		Signed number minimum value (e.g IOS_MIN, found in <limits.h>)
 * 
 * @param[in] lhs
 * 		First addend value
 * 
 * @param[in] rhs
 * 		Second addend value
 * 
 * @return true/false
*/
#define OS_OVERFLOW_ADD(max, min, lhs, rhs) (bool)(		\
		(												\
			((rhs) > 0) && ((lhs) > ((max) - (rhs)))	\
		) || (											\
			((rhs) < 0) && ((lhs) < ((min) - (rhs)))	\
		)												\
	)

/**
 * Check for overflow when subtracting two signed numbers.
 * 
 * @param[in] max
 * 		Signed number maximum value (e.g IOS_MAX, found in <limits.h>)
 * 
 * @param[in] min
 * 		Signed number minimum value (e.g IOS_MIN, found in <limits.h>)
 * 
 * @param[in] lhs
 * 		Minuend value
 * 
 * @param[in] rhs
 * 		Subtrahend value
 * 
 * @return true/false
*/
#define OS_OVERFLOW_SUB(max, min, lhs, rhs) (bool)(		\
		(												\
			((rhs) > 0) && ((lhs) < ((min) + (rhs)))	\
		) || (											\
			((rhs) < 0) && ((lhs) > ((max) + (rhs)))	\
		)												\
	)

/**
 * Check for overflow when multiplying signed numbers.
 * 
 * @param[in] max
 * 		Signed number maximum value (e.g IOS_MAX, found in <limits.h>)
 * 
 * @param[in] min
 * 		Signed number minimum value (e.g IOS_MIN, found in <limits.h>)
 * 
 * @param[in] multiplicand
 * 		Input value to be multiplied
 * 
 * @param[in] multiplier
 * 		Value to multiply by
 * 
 * @return true/false
*/
#define OS_OVERFLOW_MUL(max, min, multiplicand, multiplier) (bool)(								\
		(																						\
			((multiplicand) > 0 && (multiplier) > 0) && (multiplicand) > ((max) / (multiplier))	\
		) || (																					\
			((multiplicand) > 0 && (multiplier) < 0) && (multiplier) < ((min) / (multiplicand))	\
		) || (																					\
			((multiplicand) < 0 && (multiplier) > 0) && (multiplicand) < ((min) / (multiplier))	\
		) || (																					\
			((multiplicand) < 0 && (multiplier) < 0) && (multiplier) < ((max) / (multiplicand))	\
		)																						\
	)

/**
 * Check for overflow when converting unsigned number to signed number.
 * 
 * @param[in] max
 * 		Signed number maximum value (e.g IOS_MAX, found in <limits.h>)
 * 
 * @param[in] min
 * 		Signed number minimum value (e.g IOS_MIN, found in <limits.h>)
 * 
 * @param[in] value
 * 		Input value to be converted
 * 
 * @return true/false
*/
#define OS_OVERFLOW_CAST(max, min, value) (bool)((value > max) || (-value < min))

#endif
