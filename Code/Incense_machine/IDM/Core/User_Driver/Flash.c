#include "flash.h"
#include "main.h"
#include "IDM.h"
#include "vdm_device_config.h"

extern IDM_HARDWARE IDM_Status;

#define Read_SRAM	0x03
#define Write_SRAM	0x02
#define Read_ID	0x9F
#define WREN	0x06
#define WRDI	0x04
#define WRSR		0x01

#define Machine_Para_ADDR_BASE	0x00
//#define Buy_Para_ADDR_BASE	0x30
#define Device_config_ADDR_BASE	0x30
#define Money_ADDR_BASE		0x5D0

#define IDM_FLASH_HEADER	0x2323
#define TECHNICAL_FLASH_HEADER	0x1609


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
	printf("FLASH>> write: %d(B) at %d",sizeof(IDM_PARA), Machine_Para_ADDR_BASE);
	for(int n=0; n<sizeof(IDM_PARA); n++)
	{
		Write_Byte_Sram(Machine_Para_ADDR_BASE + n, ((uint8_t *)&IDM)[n]);
	}
}
void Read_config(void)
{
	for(int n=0; n<sizeof(IDM_PARA); n++)
	{
		((uint8_t *)&IDM)[n] = Read_Byte_Sram(Machine_Para_ADDR_BASE +n);
	}
	if(IDM.header != IDM_FLASH_HEADER)	// never set
	{
		// set default
		printf("IDM>> SET DEFAULT PARA\r\n");
		IDM.EnableHumidity = false;
		IDM.HumidityMAX = 30;
		IDM.header = IDM_FLASH_HEADER;
		IDM.NumberBuyMore =1000;
		IDM.NumberInsenseBuy =8;
		IDM.retryCellEmpty = 3;
		IDM.TimeSWAPRun = 20;
//		IDM.TimeDependIsense = 2;
		IDM.TimeTimeout = 2;
		IDM.TotalInsenseCycleSwapBuy = 1000;
		IDM.currentNumberBuyMore = IDM.NumberBuyMore;
		IDM.currentRetryCellEmpty = IDM.retryCellEmpty;
		IDM.currentTotalInsenseBuy = IDM.TotalInsenseCycleSwapBuy;
		Write_config();
	}	
}

vdm_device_config_accept_cash_t accept_denominations;

void save_device_config(void)
{
// Device_config_ADDR_BASE
//	m_device_config.accept_cash_max = 0xFAFF;
	printf("FLASH>> write: %d(B) at %d",sizeof(vdm_device_config_t), Device_config_ADDR_BASE);
	for(int n=0; n<sizeof(vdm_device_config_t); n++)
	{
		Write_Byte_Sram(Device_config_ADDR_BASE + n, ((uint8_t *)&m_device_config)[n]);
	}

}
void read_device_config(void)
{
// Device_config_ADDR_BASE	
	for(int n=0; n<sizeof(vdm_device_config_t); n++)
	{
		((uint8_t *)&m_device_config)[n] = Read_Byte_Sram(Device_config_ADDR_BASE +n);
	}
	if(m_device_config.header_valid != TECHNICAL_FLASH_HEADER)
	{
		m_device_config.header_valid = TECHNICAL_FLASH_HEADER;
		m_device_config.server.addr = VDM_DEVICE_CONFIG_DEFAULT_SERVER_ADDR;
		m_device_config.server.port = VDM_DEVICE_CONFIG_DEFAULT_SERVER_PORT;
	}
	save_device_config();
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

void sync_number_celled(uint16_t number_cell)
{
	if(IDM.currentTotalInsenseBuy <number_cell)	IDM.currentTotalInsenseBuy =0;
	else	IDM.currentTotalInsenseBuy -= number_cell;
	
	if(IDM_Status.isEmptyIsenseSW)
	{
		if(IDM.currentNumberBuyMore < number_cell)	IDM.currentNumberBuyMore =0;
		else	IDM.currentNumberBuyMore -=number_cell;
	}
	else
		IDM.currentNumberBuyMore = IDM.NumberBuyMore;
	
	if(number_cell < IDM.NumberInsenseBuy)
		if(IDM.currentRetryCellEmpty)	IDM.currentRetryCellEmpty --;
	Write_config();
}

void flash_init(void)
{
	SRAM_writeSTT();
}


