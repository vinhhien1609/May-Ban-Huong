#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include <stdint.h>

typedef enum
{
	FLASH_PIN_SS,
	FLASH_PIN_CLK,
	FLASH_PIN_DATA,
} flash_pin_t;


void flash_init(void);
uint32_t Read_SRAM_ID(void);
uint8_t Read_Byte_Sram(uint16_t add);
void Write_Byte_Sram(uint16_t add, uint8_t data);
uint8_t Read_SRAM_STT(void);
void Read_config(void);
void Write_config(void);

#endif	/* flash_DRIVER_H */
