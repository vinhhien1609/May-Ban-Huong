#include "flash.h"
#include "main.h"
#include "IDM.h"



#define Read_SRAM	0x03
#define Write_SRAM	0x02
#define Read_ID	0x9F
#define WREN	0x06
#define WRDI	0x04
#define WRSR		0x01

#define Machine_Para_ADDR_BASE	0x00
#define Buy_Para_ADDR_BASE	0x30
#define Money_ADDR_BASE	0x500

extern IDM_PARA IDM;

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
}

void flash_init(void)
{
	SRAM_writeSTT()	;
}


