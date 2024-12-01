/*
 * utilities.h
 *
 *  Created on: Aug 1, 2021
 *      Author: huybk
 */
#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

/**
 * @brief		Caculate CRC16
 * @param[in]	data Data to calculate CRC16 value
 * @param[in]	length Length of data
 * @retval		CRC16 value
 */
uint16_t utilities_calculate_crc16(const uint8_t* data, uint32_t length);

#endif /* UTILITIES_H */


