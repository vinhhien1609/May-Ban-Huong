#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

typedef enum
{
	FLASH_PIN_SS,
	FLASH_PIN_CLK,
	FLASH_PIN_DATA,
} flash_pin_t;

extern SPI_HandleTypeDef hspi1;

typedef struct
{
    uint16_t money;
		uint16_t number;
} __attribute__((packed)) revenue_t;		// doanh thu

typedef struct
{
    unsigned long money;
		unsigned long number;
} __attribute__((packed)) revenue_total;		// doanh thu

void flash_init(void);
uint32_t Read_SRAM_ID(void);
uint8_t Read_Byte_Sram(uint16_t add);
void Write_Byte_Sram(uint16_t add, uint8_t data);
uint8_t Read_SRAM_STT(void);
void Read_config(void);
void Write_config(void);
void save_device_config(void);
void read_device_config(void);

void add_revenue_day(uint8_t day, uint8_t month, unsigned int year, revenue_t revenue_day);
revenue_total get_revenue_total(void);
revenue_t get_revenue_day(uint8_t day, uint8_t month, unsigned int year);
revenue_t get_revenue_month(uint8_t month, unsigned int year);
revenue_t get_revenue_year(unsigned int year);
void sync_number_celled(uint16_t number_cell);

#endif	/* flash_DRIVER_H */
