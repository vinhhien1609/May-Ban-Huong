#ifndef __ITL_TYPES_H
#define __ITL_TYPES_H

#include <stdint.h>

typedef signed char INT8;
typedef unsigned char UINT8;
typedef volatile signed char VINT8;
typedef volatile unsigned char VUINT8;

typedef signed short INT16;
typedef unsigned short UINT16;
typedef volatile signed short VINT16;
typedef volatile unsigned short VUINT16;

typedef signed long INT32;
typedef uint32_t UINT32;
typedef volatile signed long VINT32;
typedef volatile uint32_t VUINT32;

typedef signed long long INT64;
typedef long long UINT64;
typedef volatile signed long long VINT64;
typedef volatile long long VUINT64;

typedef int32_t SSP_PORT;
#endif
