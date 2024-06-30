
#ifndef CRC_CALC_H
#define CRC_CALC_H

#include <stdint.h>

/**
 * @brief		Calculate CRC16 of data
 * @param[in]	data Pointer to buffer data
 * @param[in]	length Data length
 */
uint16_t crc_calculator_xmodem(uint8_t* data, uint16_t length);

/**
 * @brief		Calculate CRC8 of data
 * @param[in]	data Pointer to buffer data
 * @param[in]	length Data length
 */
uint8_t crc_calculator_get_crc8 (uint8_t *data, uint8_t length);

#endif  /* CRC_CALC_H */
