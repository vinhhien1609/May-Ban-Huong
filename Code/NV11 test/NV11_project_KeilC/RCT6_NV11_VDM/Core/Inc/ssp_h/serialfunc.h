
#ifndef SSP_SERIAL_FUNC_H
#define SSP_SERIAL_FUNC_H

#include "main.h"
#include <stdlib.h>


#define SERIAL_FUNCTION_USE_DMA         0

#if SERIAL_FUNCTION_USE_DMA == 0
#define SERIAL_FUNCTION_DMA_RX          0
#else
#define SERIAL_FUNCTION_DMA_RX          0
#endif 


/**
 * @brief           Initialize serial driver for NV11 communication 
 */
void ssp_serial_initialize(void);
void ssp_serial_deinitialize(void);
void ssp_serial_write_data(const uint8_t* data, uint32_t length);
uint32_t ssp_serial_rx_availble(void);
uint32_t ssp_serial_read(uint8_t* buffer, uint32_t bytes_to_read);
void ssp_serial_flush(void);

#endif  /* SSP_SERIAL_FUNC_H */

