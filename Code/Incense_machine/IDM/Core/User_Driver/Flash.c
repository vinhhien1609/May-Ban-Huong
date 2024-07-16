#include "flash.h"
#include "main.h"
#include "IDM.h"
#include "vdm_device_config.h"


#define Read_SRAM	0x03
#define Write_SRAM	0x02
#define Read_ID	0x9F
#define WREN	0x06
#define WRDI	0x04
#define WRSR		0x01

#define Machine_Para_ADDR_BASE	0x00
#define Buy_Para_ADDR_BASE	0x30
#define Device_config_ADDR_BASE	0x400
#define Money_ADDR_BASE		0x5D0


extern IDM_PARA IDM;

extern vdm_device_config_t m_device_config;

void Flash_write_pin(flash_pin_t pin, uint32_t output)
{
	GPIO_PinState outpin;
    if (output)
    {
        outpin = GPIO_PIN_SET;
    }
    else
    {
        outpin = GPIO_PIN_RESET;
    }

    switch (pin)
    {
    case FLASH_PIN_SS:
				HAL_GPIO_WritePin(Flash_SS_GPIO_Port, Flash_SS_Pin, outpin);
        break;

    case FLASH_PIN_CLK:
        HAL_GPIO_WritePin(Flash_SCK_GPIO_Port, Flash_SCK_Pin, outpin);
        break;

    case FLASH_PIN_DATA:
        HAL_GPIO_WritePin(Flash_MOSI_GPIO_Port, Flash_MOSI_Pin, outpin);
        break;

    default:
        break;
    }
}


void SRAM_writeSTT(void)
{
	uint8_t n=0, code=WREN;
	Flash_write_pin(FLASH_PIN_SS, 0);
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}
	code = WRSR;
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}	
	code = 0x82;
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}	
	Flash_write_pin(FLASH_PIN_SS, 1);
}	

uint32_t Read_SRAM_ID(void)
{
	uint32_t Result=0;
	uint8_t n=0, code=Read_ID;
	Flash_write_pin(FLASH_PIN_SS, 0);
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}
	//read ID
	Result=0;
	for(n=0; n<32; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		Result= Result<<1;		
		Flash_write_pin(FLASH_PIN_CLK, 1);
		if(HAL_GPIO_ReadPin(Flash_MISO_GPIO_Port, Flash_MISO_Pin))
			Result |=0x01;
	}
	Flash_write_pin(FLASH_PIN_SS, 1);
	return Result;
	
}

uint8_t Read_Byte_Sram(uint16_t add)
{
	uint16_t addr=add;
	uint8_t n=0, code=Read_SRAM, d_out=0;
	Flash_write_pin(FLASH_PIN_SS, 0);
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}
	//send addr
	for(n=0; n<16; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (addr & (0x8000>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}	
	//read data
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		d_out= d_out<<1;		
		Flash_write_pin(FLASH_PIN_CLK, 1);
		if(HAL_GPIO_ReadPin(Flash_MISO_GPIO_Port, Flash_MISO_Pin))
			d_out |=0x01;
	}
	Flash_write_pin(FLASH_PIN_SS, 1);
	return d_out;
	
}

void Write_Byte_Sram(uint16_t add, uint8_t data)
{
	uint16_t addr=add;
	uint8_t n=0, code=Write_SRAM, d_in=data;
	
	Flash_write_pin(FLASH_PIN_SS, 0);
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}
	//send addr
	for(n=0; n<16; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (addr & (0x8000>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}	
	//read data
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (d_in & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}
	Flash_write_pin(FLASH_PIN_SS, 1);
}

uint8_t Read_SRAM_STT(void)
{
	uint32_t Result=0;
	uint8_t n=0, code=0x05;
	Flash_write_pin(FLASH_PIN_SS, 0);
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
		else	Flash_write_pin(FLASH_PIN_DATA, 0);
		Flash_write_pin(FLASH_PIN_CLK, 1);
	}
	//read STT
	Result=0;
	for(n=0; n<8; n++)
	{
		Flash_write_pin(FLASH_PIN_CLK, 0);
		Result= Result<<1;		
		Flash_write_pin(FLASH_PIN_CLK, 1);
		if(HAL_GPIO_ReadPin(Flash_MISO_GPIO_Port, Flash_MISO_Pin))
			Result |=0x01;
	}
	Flash_write_pin(FLASH_PIN_SS, 1);
	return Result;
	
}

void Write_config(void)
{
	uint16_t addr = Machine_Para_ADDR_BASE;
	uint8_t temp;
	Buzz_On(100);
	Write_Byte_Sram(addr, IDM.EnableHumidity);
	
	addr++;
	Write_Byte_Sram(addr, IDM.HumidityMAX);
	
	addr++;	
	temp = IDM.NumberBuyMore/256;
	Write_Byte_Sram(addr, temp);
	addr++;
	temp = IDM.NumberBuyMore%256;
	Write_Byte_Sram(addr, temp);
	
	addr++;
	Write_Byte_Sram(addr, IDM.NumberInsenseBuy);
	
	addr++;
	Write_Byte_Sram(addr, IDM.TimeConveyerRun);
	
	addr++;
	temp = IDM.TotalInsenseBuy/256;
	Write_Byte_Sram(addr, temp);
	addr++;
	temp = IDM.TotalInsenseBuy%256;
	Write_Byte_Sram(addr, temp);
	
	addr++;
	Write_Byte_Sram(addr, IDM.TimeSwapIsense);
}
void Read_config(void)
{
	uint16_t addr = Machine_Para_ADDR_BASE;
	uint8_t temp[2];
	
	IDM.EnableHumidity =	Read_Byte_Sram(addr);
	
	addr++;
	IDM.HumidityMAX = Read_Byte_Sram(addr);
	
	addr++;
	temp[0] = Read_Byte_Sram(addr);
	addr++;
	temp[1] = Read_Byte_Sram(addr);
	IDM.NumberBuyMore = temp[0] *256 + temp[1];
	
	addr++;
	IDM.NumberInsenseBuy = Read_Byte_Sram(addr);
	
	addr++;
	IDM.TimeConveyerRun = Read_Byte_Sram(addr);

	addr++;
	temp[0] = Read_Byte_Sram(addr);
	addr++;
	temp[1] = Read_Byte_Sram(addr);
	IDM.TotalInsenseBuy = temp[0] *256 + temp[1];
	
	addr++;
	IDM.TimeSwapIsense = Read_Byte_Sram(addr);
	read_device_config();
}

vdm_device_config_accept_cash_t accept_denominations;

void save_device_config(void)
{
// Device_config_ADDR_BASE
//	m_device_config.accept_cash_max = 0xFAFF;
	for(int n=0; n<sizeof(m_device_config); n++)
	{
		Write_Byte_Sram(Device_config_ADDR_BASE + n, ((uint8_t *)&m_device_config)[n]);
	}
}
void read_device_config(void)
{
// Device_config_ADDR_BASE	
	for(int n=0; n<sizeof(m_device_config); n++)
	{
		((uint8_t *)&m_device_config)[n] = Read_Byte_Sram(Device_config_ADDR_BASE +n);
	}
	accept_denominations = m_device_config.accept_cash_max;
}

revenue_t get_revenue_day(uint8_t day, uint8_t month, uint8_t year)
{
	revenue_t money;
	uint16_t addr = Money_ADDR_BASE + (day + month*31 + (year-2024)*372)*4;
	for(int n=0; n<sizeof(money); n++)
	((uint8_t *)&money)[n] = Read_Byte_Sram(addr + n);
	return (money);
}

revenue_t get_revenue_month(uint8_t month, uint8_t year)
{
	revenue_t money, temp;
	money.money =0;
	money.number =0;
	for(int n=0; n<31; n++)
	{
		temp = get_revenue_day(n+1, month, year);
		money.money += temp.money;
		money.number += temp.number;
	}
	return (money);
}

revenue_t get_revenue_year(uint8_t year)
{
	revenue_t money, temp;
	money.money =0;
	money.number =0;
	for(int n=0; n<12; n++)
	{
		temp = get_revenue_month(n+1, year);
		money.money += temp.money;
		money.number += temp.number;
	}
	return (money);
}

void add_revenue_day(uint8_t day, uint8_t month, uint8_t year, revenue_t revenue_day)
{
	revenue_t money = get_revenue_day(day, month, year);
	money.money += revenue_day.money;
	money.number += revenue_day.number;
	
	uint16_t addr = Money_ADDR_BASE + (day + month*31 + (year-2024)*372)*4;
	for(int n=0; n<sizeof(money); n++)
	{
		Write_Byte_Sram(addr + n, ((uint8_t *)&money)[n]);
	}	
}


void flash_init(void)
{
	SRAM_writeSTT();
}


