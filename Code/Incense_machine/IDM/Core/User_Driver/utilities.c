/*
 * utilities.c
 *
 *  Created on: Aug 1, 2021
 *      Author: huybk
 */
#include "utilities.h"

uint16_t utilities_calculate_crc16(const uint8_t* data, uint32_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--)
    {
        x = crc >> 8 ^ *data++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }

    return crc;
}
