#include "flash.h"
#include "main.h"
#include "IDM.h"
#include "vdm_device_config.h"

extern IDM_HARDWARE IDM_Status;
extern BUY_PARA buy;

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

#define IDM_FLASH_HEADER	0x0204
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

//    case FLASH_PIN_CLK:
//        HAL_GPIO_WritePin(Flash_SCK_GPIO_Port, Flash_SCK_Pin, outpin);
//        break;

//    case FLASH_PIN_DATA:
//        HAL_GPIO_WritePin(Flash_MOSI_GPIO_Port, Flash_MOSI_Pin, outpin);
//        break;

    default:
        break;
    }
}


void SRAM_writeSTT(void)
{
	uint8_t n=0, code=WREN, buff[3];
	Flash_write_pin(FLASH_PIN_SS, 0);
	
	buff[0] = WREN;
	buff[1] = WRSR;
	buff[2] = 0x82;
	HAL_SPI_Transmit(&hspi1, &buff[0], 3, 100);
	
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}
//	code = WRSR;
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}	
//	code = 0x82;
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}	
	
	
	Flash_write_pin(FLASH_PIN_SS, 1);
}	

uint32_t Read_SRAM_ID(void)
{
	uint32_t Result=0;
	uint8_t n=0, code=Read_ID, buff[4];
	Flash_write_pin(FLASH_PIN_SS, 0);
	buff[0] = Read_ID;
	HAL_SPI_Transmit(&hspi1, &buff[0], 1, 100);
	
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}
	//read ID
	
	HAL_SPI_Receive(&hspi1, &buff[0], 4, 100);
	Result = buff[0]<<24 | buff[1]<<16 | buff[2]<<8 | buff[3];
	
//	for(n=0; n<32; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		Result= Result<<1;		
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//		if(HAL_GPIO_ReadPin(Flash_MISO_GPIO_Port, Flash_MISO_Pin))
//			Result |=0x01;
//	}
	Flash_write_pin(FLASH_PIN_SS, 1);
	return Result;
	
}

uint8_t Read_Byte_Sram(uint16_t add)
{
	uint16_t addr=add;
	uint8_t n=0, code=Read_SRAM, d_out=0, buff[3];
	Flash_write_pin(FLASH_PIN_SS, 0);
	
	buff[0] = code;
	HAL_SPI_Transmit(&hspi1, &buff[0], 1, 100);
	
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}
	
	//send addr
	
	buff[0] = ((addr>>8)&0xFF);
	buff[1] = (addr&0xFF);
	HAL_SPI_Transmit(&hspi1, &buff[0], 2, 100);
	
//	for(n=0; n<16; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (addr & (0x8000>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}	
	//read data
	HAL_SPI_Receive(&hspi1, &buff[0], 1, 100);
	d_out = buff[0];	
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		d_out= d_out<<1;		
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//		if(HAL_GPIO_ReadPin(Flash_MISO_GPIO_Port, Flash_MISO_Pin))
//			d_out |=0x01;
//	}
	Flash_write_pin(FLASH_PIN_SS, 1);
	return d_out;
	
}

void Write_Byte_Sram(uint16_t add, uint8_t data)
{
	uint16_t addr=add;
	uint8_t n=0, code=Write_SRAM, d_in=data, buff[3];
	
	Flash_write_pin(FLASH_PIN_SS, 0);
	
	buff[0] = code;
	HAL_SPI_Transmit(&hspi1, &buff[0], 1, 100);
	
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}
	//send addr
	buff[0] = ((addr>>8)&0xFF);
	buff[1] = (addr&0xFF);
	HAL_SPI_Transmit(&hspi1, &buff[0], 2, 100);	
//	for(n=0; n<16; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (addr & (0x8000>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}	
	//read data
	buff[0] = d_in;
	HAL_SPI_Transmit(&hspi1, &buff[0], 1, 100);	
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (d_in & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}

	
	Flash_write_pin(FLASH_PIN_SS, 1);
}

uint8_t Read_SRAM_STT(void)
{
	uint32_t Result=0;
	uint8_t n=0, code=0x05, buff[3];
	Flash_write_pin(FLASH_PIN_SS, 0);

	buff[0] = code;
	HAL_SPI_Transmit(&hspi1, &buff[0], 1, 100);	
	
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		if (code & (0x80>>n))	Flash_write_pin(FLASH_PIN_DATA, 1);
//		else	Flash_write_pin(FLASH_PIN_DATA, 0);
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//	}
	//read STT
	HAL_SPI_Receive(&hspi1, &buff[0], 1, 100);
	Result = buff[0];	
//	Result=0;
//	for(n=0; n<8; n++)
//	{
//		Flash_write_pin(FLASH_PIN_CLK, 0);
//		Result= Result<<1;		
//		Flash_write_pin(FLASH_PIN_CLK, 1);
//		if(HAL_GPIO_ReadPin(Flash_MISO_GPIO_Port, Flash_MISO_Pin))
//			Result |=0x01;
//	}
	
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
//	uint16_t m;
//	for(m=0; m<1488; m++)
//	{
//		Write_Byte_Sram(Money_ADDR_BASE + m, 0);
//	}	
//		for(m=22104; m<1488; m++)
//	{
//		printf("%2X ",Read_Byte_Sram(Money_ADDR_BASE + m));
//	}
//		printf("%d-xxxxxxxxxxxxxxxxxxxxxxxx\r\n", m);	
}

vdm_device_config_accept_cash_t accept_denominations;

void save_device_config(void)
{
// Device_config_ADDR_BASE
//	m_device_config.accept_cash_max = 0xFAFF;
	printf("FLASH>> write: %d(B) at %d\r\n",sizeof(vdm_device_config_t), Device_config_ADDR_BASE);
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
		printf("IDM>> SET DEFAULT TECHNICAL PARA\r\n");
		m_device_config.header_valid = TECHNICAL_FLASH_HEADER;
		m_device_config.server.addr = VDM_DEVICE_CONFIG_DEFAULT_SERVER_ADDR;
		m_device_config.server.port = VDM_DEVICE_CONFIG_DEFAULT_SERVER_PORT;
		m_device_config.accept_cash_max = 0x1FF;
		save_device_config();		
	}
	else
		printf("IDM>> READ_CONFIG_DONE\r\n");
}

revenue_total get_revenue_total(void)
{
	revenue_total money;
	uint16_t addr = Money_ADDR_BASE;
	for(int n=0; n<sizeof(money); n++)
		((uint8_t *)&money)[n] = Read_Byte_Sram(addr + n);
	return (money);
}

void add_revenue_total(revenue_t money)
{
	revenue_total Total_money = get_revenue_total();
	Total_money.money += money.money;
	Total_money.number += money.number;
	
	uint16_t addr = Money_ADDR_BASE;// + (day + month*31 + (year-2024)*372)*4;
	for(int n=0; n<sizeof(Total_money); n++)
	{
		Write_Byte_Sram(addr + n, ((uint8_t *)&Total_money)[n]);
	}
}

revenue_t get_revenue_day(uint8_t day, uint8_t month, unsigned int year)
{
	revenue_t money;
	uint16_t addr = Money_ADDR_BASE + 8 + (day + month*31 + (year-2024)*372)*4;
	for(int n=0; n<sizeof(money); n++)
	{
	((uint8_t *)&money)[n] = Read_Byte_Sram(addr + n);
	}
	
	return (money);
}

revenue_t get_revenue_month(uint8_t month, unsigned int year)
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

revenue_t get_revenue_year(unsigned int year)
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

void add_revenue_day(uint8_t day, uint8_t month, unsigned int year, revenue_t revenue_day)
{
	revenue_t money = get_revenue_day(day, month, year);
	money.money += revenue_day.money;
	money.number += revenue_day.number;
	
	uint16_t addr = Money_ADDR_BASE + 8 + (day + month*31 + (year-2024)*372)*4;
	for(int n=0; n<sizeof(money); n++)
	{
		Write_Byte_Sram(addr + n, ((uint8_t *)&money)[n]);
	}
	add_revenue_total(revenue_day);
}

void sync_number_celled(uint16_t number_cell)
{
	if(IDM.currentTotalInsenseBuy <number_cell)	IDM.currentTotalInsenseBuy =0;
	else	IDM.currentTotalInsenseBuy -= number_cell;
	
	if(IDM_Status.isEmptyIsenseSW)		// không tác dong
	{
		if(IDM.currentNumberBuyMore < number_cell)	IDM.currentNumberBuyMore =0;
		else	IDM.currentNumberBuyMore -=number_cell;
	}
	else
		IDM.currentNumberBuyMore = IDM.NumberBuyMore;
	
	if(number_cell < buy.TotalSale)
	{
		if(m_device_config.run_mode== SALES_MODE)	IDM.currentRetryCellEmpty=0;
		if(IDM.currentRetryCellEmpty)	IDM.currentRetryCellEmpty --;
	}
	Write_config();
}

void flash_init(void)
{
	SRAM_writeSTT();
}


