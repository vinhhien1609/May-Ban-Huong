#include <stdint.h>
#include <stdio.h>

#define DEBUG_BY_UART 1

#ifdef DEBUG_BY_UART
#define DEBUG_INFO(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#ifdef DEBUG_BY_UART
#define DEBUG_ERROR(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#ifdef DEBUG_BY_UART
#define DEBUG_WARN(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#ifdef DEBUG_BY_UART
#define DEBUG_VERBOSE(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif
