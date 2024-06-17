/*
 * platform.h
 *
 *  Created on: Jul 23, 2021
 *      Author: huybk213
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdint.h>
#include <stdbool.h>

/* Macro to set a bit in a register */
#ifndef SETBIT
#define SETBIT(addr, bit) 		((addr) |= (1 << (bit)))
#endif

/* Macro to clear a bit in a register */
#ifndef CLEARBIT
#define CLEARBIT(addr, bit)		((addr) &= ~(1 << (bit)))
#endif

/* Macro to check a bit in a register is 1 or 0 */
#ifndef CHECKBIT
#define CHECKBIT(addr, bit) 	(((addr) & (1 << (bit))) == (1 << (bit)))
#endif


#ifndef READBIT
#define READBIT(value, bit) (((value) >> (bit)) & 0x01)
#endif


#endif /* PLATFORM_H_ */
