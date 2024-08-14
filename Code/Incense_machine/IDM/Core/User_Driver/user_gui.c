/*********************************************************************************************************
*
* File                : RTC_Time.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include "stm32f1xx_hal.h"
//#include "stm32f1xx_hal_rtc.h"
#include "USER_GUIDE.h"
#include "GLCD.h"
#include "GSM_app.h"
#include "RTC_time.h"
#include "dht_sensor.h"
#include "IDM.h"
#include "vdm_language.h"
#include "images.h"
#include "main.h"
#include "ssp_helpers.h"
#include "nv11.h"
#include "flash.h"
#include "vdm_device_config.h"
#include "vdm_app_gsm.h"

#define KEY_HOLE_TIME 50
#define KEY_REPEAT_TIME 1500
#define ENTER	'*'
#define CANCEL '#'
#define UP	'8'
#define DOWN	'0'

unsigned char key_map[16]= {'0','1','2','3','4','5','6','7','8','9','*','#'}, key_tick=255, temp_menu_char[30];
unsigned char old_key=255, point=0, old_point=0, sub1_point, sub2_point, Old_cell_state, show_time=0, last_DIS=0, stt_door;
unsigned int key_count=0;


Display_Typedef current_display=DIS_Home, old_display=255;
rtc_date_time_t TimeSetting;
char str[30];


typedef uint16_t char16_t;

uint16_t temp_data_Menu;
char pass_shop[6] = VDM_DEVICE_CONFIG_DEFAULT_SHOP_PASSWORD;
char pass_tech[6] = VDM_DEVICE_CONFIG_DEFAULT_TECH_PASSWORD;

extern GSM_State GSM;
extern uint8_t isSecond_display;
extern	rtc_date_time_t currentTime;
extern IDM_PARA IDM;
extern IDM_HARDWARE IDM_Status;
extern IDM_ERROR IDM_Errors;
extern BUY_PARA buy;

extern uint32_t m_lastest_note; // Menh gia to tien gan nhat
extern nv11_event_t m_last_n11_event;

extern vdm_device_config_t m_device_config;
extern unsigned char havemoney, IDM_state;
extern SSP6_SETUP_REQUEST_DATA m_setup_req;
extern 	float Humidity;
static uint8_t RTC_WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay);
/* Private define ------------------------------------------------------------*/

void draw_signal_gsm(void);


static void small_delay()		// not importance function
{
	volatile uint32_t i = 0x02F;
	while (i--);
}

unsigned char scan_BT(void)
{
	unsigned char temp= key_tick;
	if( temp!=255)	printf("KEY >> %c Pressed\r\n", key_tick);
	key_tick =255;
	return (temp);
}
	
void scan_switch(void)
{
	unsigned char key=255;
	HAL_GPIO_WritePin(KEY_COL1_GPIO_Port, KEY_COL1_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =0;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =1;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =2;
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =3;	//*
	HAL_GPIO_WritePin(KEY_COL1_GPIO_Port, KEY_COL1_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(KEY_COL2_GPIO_Port, KEY_COL2_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =4;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =5;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =6;
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =7;
	HAL_GPIO_WritePin(KEY_COL2_GPIO_Port, KEY_COL2_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(KEY_COL3_GPIO_Port, KEY_COL3_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =8;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =9;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =10;	//*
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =11;	//#
	HAL_GPIO_WritePin(KEY_COL3_GPIO_Port, KEY_COL3_Pin, GPIO_PIN_RESET);

	if(old_key==key && key <255)
	{
		key_count ++;
		if(key_count > KEY_REPEAT_TIME)
			key_count =0;
		if(key_count==KEY_HOLE_TIME)
		{
			Buzz_On(10);
				key_tick = key_map[key];// chi gui 1 lan cho toi khi nha
		}
	}
	else
	{
		key_count =0;
		old_key = key;
	}

}

void show_Messenger(const char *messenger, uint16_t timeOn)
{
	GLcd_FillRect(30, 10, 60, 35, BLACK);
	GLcd_DrawRect(30, 10, 60, 35, WHITE);
	GLcd_DrawStringUni(messenger, 35, 25, WHITE);
	last_DIS = current_display;
	current_display = DIS_SHOW_MESSENGER;
	Buzz_On(100);
}

//draw signal GSM
uint8_t csq=255;
void draw_signal_gsm(void)
{
	uint8_t t=0;
	if(csq != GSM.CSQ)
			csq = GSM.CSQ;
	else
		return;
	while(t<14)
	{
		GLcd_DrawLine(t,0, t, 6, BLACK);
		t++;
	}
		printf("GSM>> CSQ: %d\r\n", GSM.CSQ);
		if(GSM.CSQ)
		{
			GLcd_DrawLine(0,0, 3, 3, WHITE);
			GLcd_DrawLine(3,3, 6, 0, WHITE);
			GLcd_DrawLine(0,0, 6, 0, WHITE);
			if(GSM.Connect4G)	GLcd_DrawLine(3,0, 3, 6, WHITE);
			else	GLcd_DrawLine(3,0, 3, 3, WHITE);
		}
		else
		{
			GLcd_DrawLine(0,0, 4, 4, WHITE);
			GLcd_DrawLine(0,4, 4, 0, WHITE);
		}
		t=0;
		while(t<5)
		{
			if(t<GSM.CSQ/6)	GLcd_DrawLine(5+t*2, 5 - t, 5+t*2, 6, WHITE);
			else	GLcd_DrawLine(5+t*2, 6, 5+t*2, 6, WHITE);
			t++;
		}
}

void Menu_draw(void)
{
	char s[100];
	char day[7][3] = {"CN", "T2", "T3", "T4", "T5", "T6", "T7"};
	unsigned char 	key_pressed=255;
	revenue_t reven;
//	t = HAL_GPIO_ReadPin(Empty_GPIO_Port, Empty_Pin);
	
	if(IDM_Status.isDoorOpen != stt_door)
	{
		stt_door = IDM_Status.isDoorOpen;
		if(IDM_Status.isDoorOpen == true)
		{
			current_display = DIS_Password;
			NV11_Disable(true);
		}
		else
			current_display = DIS_Home;
	}
	
	if(current_display!= old_display)		//init etc display
	{
		old_display = current_display;

//		point =sub1_point;
//		old_point =0;
		if(current_display!=DIS_SHOW_MESSENGER)
			GLcd_ClearScreen(BLACK);
		switch(current_display)
		{
			case DIS_SHOW_MESSENGER:
				show_time=0;
				break;			
			case DIS_Home:
				csq = 255;
				if(buy.StateBuy==STOP_SERVICE)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DO_NOT_SERVICE),5, 20, WHITE);
				}				
				if(buy.StateBuy==CELLING)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_MONEY_RECEIVED),0, 10, WHITE);
					sprintf(s,"%3d.000VND", m_lastest_note/1000);
//					//luu doanh thu
//					reven.money = m_lastest_note/1000;
//					reven.number =0;
//					add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);
//					m_lastest_note =0;
					GLcd_DrawString(s,64,15,WHITE);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_WAIT_RECEVER_INSENCE),15, 45, WHITE);
				}
				if(buy.StateBuy == CELL_WAIT)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);
					nv11_enable(true);
				}
				if(buy.StateBuy ==CELL_EMPTY_INSENCE)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_SORRY),40, 15, WHITE);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_EMPTY_INSENCE),20, 30, WHITE);
					buy.StateBuy = CELL_WAIT;
				}
				if(buy.StateBuy ==CELL_SUCCESS)
				{				
					buy.StateBuy = CELL_WAIT;
				}
				if(buy.StateBuy ==DONATE)
				{
					buy.StateBuy = CELL_WAIT;
				}					
				break;
			case DIS_Password:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_PASSWORD),20, 20, WHITE);
				GLcd_DrawString("------", 40, 40, WHITE);
				memcpy(temp_menu_char, "------", 6);
				break;
			case DIS_Setup:
					GLcd_DrawBitmap(img_arrow_bmp, 0, (point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((point/3)*3 +n >10)	break;
						GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MOUNT+(point/3)*3 +n),10, 10 + n*15, WHITE);
					}			
				break;
			case DIS_TimeSetup:
				GLcd_DrawString("Time Setting", 18, 0, WHITE);
				TimeSetting = currentTime;
				sprintf(s,"%s %02d/%02d/%d %02d:%02d:%02d ", day[TimeSetting.weekday]\
				, TimeSetting.day, TimeSetting.month,TimeSetting.year, TimeSetting.hour, TimeSetting.minute, TimeSetting.second);
				GLcd_DrawString(s, 0, 24, WHITE);
				sprintf(str,"%02d%02d%d%02d%02d%02d", TimeSetting.day, TimeSetting.month,TimeSetting.year, TimeSetting.hour, TimeSetting.minute, TimeSetting.second);
				GLcd_DrawLine(18,33,23,33,WHITE);
				break;
			case DIS_SETUP_TECH:
					GLcd_DrawBitmap(img_arrow_bmp, 0, (point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((point/3)*3 +n >1)	break;
						GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ID_MACHINE+(point/3)*3 +n),10, 10 + n*15, WHITE);
					}
				break;
			case DIS_SET_IP:
				sub2_point=0;
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_SERVER_IP),40, 0, WHITE);
				temp_menu_char[0] = (m_device_config.server.addr>>24)&0xFF;
				temp_menu_char[1] = (m_device_config.server.addr>>16)&0xFF;
				temp_menu_char[2] = (m_device_config.server.addr>>8)&0xFF;
				temp_menu_char[3] = m_device_config.server.addr&0xFF;
				temp_data_Menu = m_device_config.server.port;
				
				sprintf(s,"IP: %03d.%03d.%03d.%03d", temp_menu_char[0], temp_menu_char[1], temp_menu_char[2], temp_menu_char[3]);
				GLcd_DrawString(s, 0, 20, WHITE);
			
				sprintf(s,"PORT: %04d", temp_data_Menu);
				GLcd_DrawString(s, 0, 40, WHITE);
				GLcd_DrawLine(23, 30, 35, 30, WHITE);
				break;
			case DIS_SET_ID_MACHINE:
				sub2_point=0;
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ID_MACHINE),40, 0, WHITE);
				for(int n=0; n<7; n++)
					temp_menu_char[n] = m_device_config.peripheral.machine_id[n] + '0';
				temp_menu_char[7]=0;
				sprintf(s,"ID: %s", temp_menu_char);
				GLcd_DrawString(s, 0, 20, WHITE);
				GLcd_DrawLine(23, 30, 27, 30, WHITE);	
				break;
			case DIS_TOTAL_MONEY:
				GLcd_DrawBitmap(img_arrow_bmp, 0, 15 + 15*sub1_point);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DAY),10, 10, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_MONTH),10, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_YEAR),10, 40, WHITE);
				break;
			case DIS_DAY:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);
				TimeSetting = currentTime;
				sprintf(s,"%02d/%02d/%4d",TimeSetting.day, TimeSetting.month,TimeSetting.year);
				GLcd_DrawString(s, 35, 10,WHITE);
				reven = get_revenue_day(TimeSetting.day,TimeSetting.month, TimeSetting.year);
				sprintf(s,"%d000", reven.money);
				GLcd_DrawString(s, 60, 45,WHITE);
				sprintf(s,"%d", reven.number);
				GLcd_DrawString(s, 60, 30,WHITE);			
				break;
			case DIS_MONTH:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);
				TimeSetting = currentTime;				
				sprintf(s,"%02d/%4d",TimeSetting.month,TimeSetting.year);
				GLcd_DrawString(s, 40, 10,WHITE);
				revenue_t reven = get_revenue_month(TimeSetting.month, TimeSetting.year);
				sprintf(s,"%d000", reven.money);
				GLcd_DrawString(s, 60, 45,WHITE);
				sprintf(s,"%d", reven.number);
				GLcd_DrawString(s, 60, 30,WHITE);			
				break;
			
			case DIS_YEAR:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);
				TimeSetting = currentTime;
				sprintf(s,"%4d",TimeSetting.year);
				GLcd_DrawString(s, 45, 10,WHITE);
				reven = get_revenue_year(TimeSetting.year);
				sprintf(s,"%d000", reven.money);
				GLcd_DrawString(s, 60, 45,WHITE);
				sprintf(s,"%d", reven.number);
				GLcd_DrawString(s, 60, 30,WHITE);			
				break;
			
			case DIS_TOTAL:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MOUNT),10, 10, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);	
				break;
			
			case DIS_CELL_EACH_TIME:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				sprintf(s, "%d", IDM.NumberInsenseBuy);
				GLcd_DrawString(s, 64, 30, WHITE);
				temp_data_Menu = IDM.NumberInsenseBuy;
				break;
			
			case DIS_ERROR_INFO:
					GLcd_DrawBitmap(img_arrow_bmp, 0, (sub1_point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((sub1_point/3)*3 +n >6)	break;
						GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ERR_MOTOR+(sub1_point/3)*3 +n),10, 10 + n*15, WHITE);
					}
				break;
					
			case DIS_ERROR_MOTOR:
				if(IDM_Errors.isMotorERROR)
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ERR_MOTOR),0, 0, WHITE);
				else				
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NO_ERROR),0, 0, WHITE);
				break;
				
			case DIS_ERROR_PAYMENT:
				if(IDM_Errors.isPaymentERROR)
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ERR_PAYMENT),0, 0, WHITE);
				else
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NO_ERROR),0, 0, WHITE);
				break;
				
			case DIS_ERROR_SENSOR:
				if(IDM_Errors.isDropERROR)
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ERR_SENSOR_DROP),0, 0, WHITE);
				else
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NO_ERROR),0, 0, WHITE);
				break;
				
			case DIS_DEL_ERROR_MOTOR:
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DONE),0, 0, WHITE);
				break;
			
			case DIS_DEL_ERROR_HW:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DONE),0, 0, WHITE);
				break;
			
			case DIS_DEL_ERROR_SENSOR:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DONE),0, 0, WHITE);
				break;
			
			case DIS_ONOFF_HUMIDITY:
				if(IDM.EnableHumidity==0)	sub1_point =1;
				GLcd_DrawBitmap(img_arrow_bmp, 0, 15 + sub1_point*15);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ON),10, 10, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_OFF),10, 25, WHITE);
				break;
			
			case DIS_HUMIDITY_SET:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_HUMUDITY_SETTING),10, 10, WHITE);
				sprintf(s, "%d%%", IDM.HumidityMAX);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.HumidityMAX;
				break;
			
			case DIS_ACCEPT_DENO:
				temp_data_Menu = m_device_config.accept_cash_max;
				GLcd_DrawBitmap(img_arrow_bmp, 0, (sub1_point%5)*10 + 10);
				for(int n=0; n< 5; n++)
				{
					sprintf(s,"%3d.000 %s",m_setup_req.ChannelData[n + sub1_point/5].value/1000, m_setup_req.ChannelData[n + sub1_point/5].cc);
					GLcd_DrawString(s,20, n*10 +10,WHITE);
					if((temp_data_Menu>>(n + sub1_point/5))&0x01)
					{
						GLcd_DrawLine(10, n*10 +12, 12, n*10 +15, WHITE);
						GLcd_DrawLine(12, n*10 +15, 16, n*10 +10, WHITE);
					}
					else
					{
							GLcd_DrawLine(10, n*10 +12, 12, n*10 +15, BLACK);
							GLcd_DrawLine(12, n*10 +15, 16, n*10 +10, BLACK);					
					}
					if((n + sub1_point/5)>=m_setup_req.NumberOfChannels)	break;
				}
				break;
			case DIS_TIME_SWAP_RUN:		// thoi gian swap
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TIME_RUN_INSENSE),10, 10, WHITE);
				sprintf(s, "%3ds ", IDM.TimeSWAPRun);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.TimeSWAPRun;
				break;
			case DIS_NUMBER_CELL_SWAP:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_INSENSE_CELL),10, 10, WHITE);
				sprintf(s, "%d", IDM.TotalInsenseCycleSwapBuy);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.TotalInsenseCycleSwapBuy;
				break;
			case DIS_CELL_MORE:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_INSENSE_CELL_MORE),10, 10, WHITE);
				sprintf(s, "%d", IDM.NumberBuyMore);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.NumberBuyMore;
				break;
			case DIS_NUMBER_CELL_MORE_EMPTY:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER_CELL_MORE),10, 10, WHITE);
				sprintf(s, "%2d", IDM.retryCellEmpty);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.retryCellEmpty;				
				break;
			case DIS_DEL_CELL_ERROR:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DELETE_ERROR_CELL),10, 10, WHITE);
				break;
			default:
				current_display = DIS_Home;
				break;
		}
		GLcd_Flush();		
	}
////////////////////////////////////////////////////////////////////////////////////////	
	else
	{
		key_pressed = scan_BT();
		if(key_pressed!=255)
			isSecond_display =0;
//		if(key_pressed =='*')
//			current_display++;
		if(isSecond_display>60)
		{
			isSecond_display =0;
			if(IDM_Status.isDoorOpen==true)
				current_display = DIS_Password;
			else
				current_display = DIS_Home;
		}
		switch(current_display)
		{
			case DIS_SHOW_MESSENGER:
				if(isSecond_display)
				{
					isSecond_display =0;
					if(show_time<2)	show_time ++;
					else	current_display = last_DIS;
				}
				if(key_pressed==CANCEL)
					current_display = last_DIS;
				break;
			case DIS_Home:

				if(isSecond_display)
				{
					
//					if(buy.StateBuy==CELLING)
//					{
//						sprintf(s,"%2d ", buy.TotalIsenseDroped);
//						GLcd_DrawString(s, 10, 30, WHITE);						
//					}
					
					isSecond_display =0;
					if(show_time>0)	show_time --;
					sprintf(s,"%s %02d/%02d %2d  %02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
					GLcd_DrawString(s, 16, 0, WHITE);
					if(currentTime.second%2)
						GLcd_DrawString(":", 78, 0, WHITE);
					draw_signal_gsm();
					sprintf(s,"%2d%% ", (int)Humidity);
					GLcd_DrawString(s, 105, 0, WHITE);
					if(m_lastest_note>100 && m_last_n11_event == NV11_NOTE_STACKED && buy.StateBuy == CELL_WAIT)
					{
						havemoney =1;
						m_last_n11_event = NV11_EVENT_NONE;
						//luu doanh thu
						reven.money = m_lastest_note/1000;
						reven.number =0;
						add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);						
					}
					if(Old_cell_state !=buy.StateBuy && show_time==0)
					{
						show_time=3;
						Old_cell_state = buy.StateBuy;
						if(buy.StateBuy==STOP_SERVICE)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DO_NOT_SERVICE),5, 20, WHITE);
						}							
						if(buy.StateBuy==CELLING)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_MONEY_RECEIVED),0, 10, WHITE);
							sprintf(s,"%3d.000VND", m_lastest_note/1000);
//							m_lastest_note =0;
							GLcd_DrawString(s,64,15,WHITE);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_WAIT_RECEVER_INSENCE),15, 45, WHITE);
						}
						if(buy.StateBuy == CELL_WAIT)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);					
						}
						if(buy.StateBuy ==CELL_EMPTY_INSENCE)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_SORRY),40, 15, WHITE);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_EMPTY_INSENCE),20, 30, WHITE);
							buy.StateBuy = CELL_WAIT;
							if(buy.TotalIsenseDroped !=IDM.NumberInsenseBuy)
							{
								reven.number = IDM.NumberInsenseBuy - buy.TotalIsenseDroped;
								reven.money =0;
								add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);
							}							
						}
						if(buy.StateBuy ==CELL_SUCCESS)
						{
							show_time=10;
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_THANK),10, 15, WHITE);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_CUSTOM_RECIVER_INSENSE),15, 35, WHITE);
							
							reven.number = IDM.NumberInsenseBuy;
							reven.money =0;
							add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);							
							buy.StateBuy = CELL_WAIT;
						}
						if(buy.StateBuy ==DONATE)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_THANK),15, 25, WHITE);					
							buy.StateBuy = CELL_WAIT;
						}						
					}
					GLcd_Flush();
				}
				break;
			case DIS_Password:
				if(key_pressed>='0' && key_pressed <='9')
				{
					temp_menu_char[point] = key_pressed;
					point++;
					if(point>=6)
					{
						point=0;						
						if(temp_menu_char[0] == pass_shop[0] && temp_menu_char[1] == pass_shop[1] && temp_menu_char[2] == pass_shop[2]\
							&& temp_menu_char[3] == pass_shop[3] && temp_menu_char[4] == pass_shop[4] && temp_menu_char[5] == pass_shop[5])
						{
							current_display = DIS_Setup;
							Buzz_On(100);
							break;
						}
						if(temp_menu_char[0] == pass_tech[0] && temp_menu_char[1] == pass_tech[1] && temp_menu_char[2] == pass_tech[2]\
							&& temp_menu_char[3] == pass_tech[3] && temp_menu_char[4] == pass_tech[4] && temp_menu_char[5] == pass_tech[5])
						{
							Buzz_On(100);
							current_display = DIS_SETUP_TECH;
							break;
						}
						Buzz_On(500);
					}
					char str[7];
					for(int n=0; n<6; n++)
					{
						if(n<point) str[n] = '*';
						else	str[n] ='-';
					}
					str[6]=0;
					GLcd_DrawString(str, 40, 40, WHITE);
				}
				if(key_pressed == CANCEL)
				{
//					current_display = DIS_Home;
				}
				if(IDM_Status.isDoorOpen ==false)		// close
				{
					current_display = DIS_Home;
				}
				break;
			case DIS_Setup:
				if(key_pressed==DOWN)
				{
					point++;
					if(point>10)	point =0;
				}
				if(key_pressed ==UP)
				{
					if(point>0)	point--;
					else	point =10;
				}
				if(key_pressed == CANCEL)
				{
					current_display = DIS_Password;
					point=0;
				}
				if(key_pressed == ENTER)
				{
					sub1_point =0;
					switch(point)
					{
						case 0:
							current_display = DIS_TOTAL_MONEY;
							sub1_point=0;
							break;
						case 1:
							current_display = DIS_CELL_EACH_TIME;
							break;
						case 2:
							current_display = DIS_ERROR_INFO;
							sub1_point =0;
							break;
						case 3:
							current_display = DIS_ONOFF_HUMIDITY;
							break;
						case 4:
							current_display = DIS_HUMIDITY_SET;
							break;
						case 5:
							current_display = DIS_ACCEPT_DENO;
							sub1_point=0;
							break;
						case 6:
							current_display = DIS_TIME_SWAP_RUN;
							break;
						case 7:
							current_display = DIS_NUMBER_CELL_SWAP;
							break;
						case 8:
							current_display = DIS_CELL_MORE;
							break;
						case 9:
							current_display = DIS_NUMBER_CELL_MORE_EMPTY;
							break;
						case 10:
							current_display = DIS_DEL_CELL_ERROR;
							break;						
					}
				}
				if(old_point!= point)
				{
					if(point%3!=1 || point ==10)	GLcd_FillRect(0, 10, 127, 48, BLACK);
					GLcd_DrawBitmap(img_del_arrow_bmp, 0, (old_point%3)*15 + 15);
					old_point =point;
					GLcd_DrawBitmap(img_arrow_bmp, 0, (point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((point/3)*3 +n >10)	break;
						GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MOUNT+(point/3)*3 +n),10, 10 + n*15, WHITE);
					}
				}
				break;
			case DIS_TimeSetup:
				if(key_pressed>='0' && key_pressed<='9')
				{
					str[point]=key_pressed;
					point++;
					if(point>13)	point =0;
					TimeSetting.day = (str[0]-'0')*10 + str[1]-'0';
					TimeSetting.month = (str[2]-'0')*10 + str[3]-'0';
					TimeSetting.year = (str[4]-'0')*1000 + (str[5]-'0')*100 + (str[6]-'0')*10 + str[7]-'0';
					TimeSetting.hour = (str[8]-'0')*10 + str[9]-'0';
					TimeSetting.minute = (str[10]-'0')*10 + str[11]-'0';
					TimeSetting.second = (str[12]-'0')*10 + str[13]-'0';
//					TimeSetting.weekday = (char)RTC_WeekDayNum(TimeSetting.year, TimeSetting.month, TimeSetting.day);
					sprintf(s,"%s %02d/%02d/%d %02d:%02d:%02d ", day[TimeSetting.weekday]\
					, TimeSetting.day, TimeSetting.month,TimeSetting.year, TimeSetting.hour, TimeSetting.minute, TimeSetting.second);
					GLcd_DrawString(s, 0, 24, WHITE);
				}
				if(key_pressed==ENTER)
				{
					setRTC(TimeSetting);
					Buzz_On(100);
				}
				if(key_pressed==CANCEL)
					current_display = DIS_Password;
				char point_pos[14] ={18,24,34,39,53,59,65,71,81,87,99,105,115,121};
				if(key_pressed!=255)
				{
					if(point==0)
						GLcd_DrawLine(point_pos[13],33,5+point_pos[13],33,BLACK);
					else
						GLcd_DrawLine(point_pos[point-1],33,5+point_pos[point-1],33,BLACK);
					GLcd_DrawLine(point_pos[point],33,5+point_pos[point],33,WHITE);
				}
				break;
			case DIS_TOTAL_MONEY:
				if(key_pressed=='0')
				{
					sub1_point++;
					if(sub1_point>2)	sub1_point=0;
				}
				if(key_pressed=='8')
				{
					if(sub1_point==0)	sub1_point=2;
					else	sub1_point--;
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				if(key_pressed == ENTER)
				{
					current_display = sub1_point + DIS_DAY;
				}
				if(old_point!= sub1_point)
				{
					old_point	= sub1_point;
					GLcd_FillRect(0, 15, 5, 45, BLACK);
					GLcd_DrawBitmap(img_arrow_bmp, 0, sub1_point*15 + 15);
				}
				break;
			case DIS_DAY:
				if(key_pressed==CANCEL)
					current_display = DIS_TOTAL_MONEY;
				unsigned char monthday[14] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
				if(key_pressed == UP)
				{
					TimeSetting.day ++;
					if(TimeSetting.day > monthday[TimeSetting.month])
					{
						TimeSetting.day =1;
						TimeSetting.month ++;
						if(TimeSetting.month >12)
						{
							TimeSetting.month=1;
							TimeSetting.year++;
						}
					}
				}
				if(key_pressed == DOWN)
				{
					if(TimeSetting.year >2024 || TimeSetting.month >1 || TimeSetting.day>1)
					{
						TimeSetting.day --;
						if(TimeSetting.day<1)
						{
							if(TimeSetting.year >2024 || TimeSetting.month >1)
							{
								TimeSetting.month --;
								TimeSetting.day = monthday[TimeSetting.month];
							}
							if(TimeSetting.month <1)
							{
								TimeSetting.month=12;
								TimeSetting.year--;
							}
						}
					}
				}
				if(key_pressed == UP || key_pressed == DOWN)
				{
					sprintf(s,"%02d/%02d/%4d",TimeSetting.day,TimeSetting.month, TimeSetting.year);
					GLcd_DrawString(s, 35, 10,WHITE);
					GLcd_FillRect(60,30,67,25,BLACK);
					reven = get_revenue_day(TimeSetting.day,TimeSetting.month, TimeSetting.year);
					sprintf(s,"%d000", reven.money);
					GLcd_DrawString(s, 60, 45,WHITE);
					sprintf(s,"%d", reven.number);
					GLcd_DrawString(s, 60, 30,WHITE);
				}
				break;
			case DIS_MONTH:
				if(key_pressed==CANCEL)
					current_display = DIS_TOTAL_MONEY;
				if(key_pressed == UP)
				{
					TimeSetting.month ++;
					if(TimeSetting.month >12)
					{
						TimeSetting.month=1;
						TimeSetting.year++;
					}
				}
				if(key_pressed == DOWN)
				{
					if(TimeSetting.year >2024 || TimeSetting.month >1)
						TimeSetting.month --;
					if(TimeSetting.month <1)
					{
						TimeSetting.month=12;
						TimeSetting.year--;
					}				
				}
				if(key_pressed == UP || key_pressed == DOWN)
				{
					sprintf(s,"%02d/%4d",TimeSetting.month, TimeSetting.year);
					GLcd_DrawString(s, 40, 10,WHITE);
					GLcd_FillRect(60,30,67,25,BLACK);
					reven = get_revenue_month(TimeSetting.month, TimeSetting.year);
					sprintf(s,"%d000", reven.money);
					GLcd_DrawString(s, 60, 45,WHITE);
					sprintf(s,"%d", reven.number);
					GLcd_DrawString(s, 60, 30,WHITE);						
				}
				break;
			case DIS_YEAR:
				if(key_pressed==CANCEL)
					current_display = DIS_TOTAL_MONEY;
				if(key_pressed == UP)
				{
					TimeSetting.year ++;
				}
				if(key_pressed == DOWN)
				{
					if(TimeSetting.year>2024) TimeSetting.year--;
				}
				if(key_pressed == UP || key_pressed == DOWN)
				{
					sprintf(s,"%4d",TimeSetting.year);
					GLcd_DrawString(s, 45, 10,WHITE);
					GLcd_FillRect(60,30,67,25,BLACK);
					reven = get_revenue_year(TimeSetting.year);
					sprintf(s,"%d000", reven.money);
					GLcd_DrawString(s, 60, 45,WHITE);
					sprintf(s,"%d", reven.number);
					GLcd_DrawString(s, 60, 30,WHITE);
				}				
				break;
			case DIS_TOTAL:
				if(key_pressed==CANCEL)
					current_display = DIS_TOTAL_MONEY;			
				break;
			case DIS_CELL_EACH_TIME:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%100) *10 + (key_pressed-'0');
					if(temp_data_Menu >100)	temp_data_Menu =temp_data_Menu%100;
					sprintf(s, "%3d ", temp_data_Menu);
					GLcd_DrawString(s, 64, 30, WHITE);					
				}
//				if(key_pressed=='0')
//					if(temp_data_Menu>0)	temp_data_Menu --;
//				if(key_pressed=='8')
//					if(temp_data_Menu<30)	temp_data_Menu ++;
//				if(key_pressed=='0' || key_pressed=='8')
//				{
//					sprintf(s, "%2d ", temp_data_Menu);
//					GLcd_DrawString(s, 64, 30, WHITE);
//				}
				if(key_pressed == ENTER)
				{
					IDM.NumberInsenseBuy=temp_data_Menu;
					Write_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				break;
				
			case DIS_ERROR_INFO:
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				if(key_pressed==DOWN)
				{
					sub1_point++;
					if(sub1_point>5)	sub1_point =0;
				}
				if(key_pressed ==UP)
				{
					if(sub1_point>0)	sub1_point--;
					else	sub1_point =5;
				}
				if(key_pressed ==ENTER)
				{
					current_display = DIS_ERROR_MOTOR + sub1_point;
					sub2_point =0;
				}
				if(old_point!= sub1_point)
				{
					if(sub1_point%3!=1)	GLcd_FillRect(0, 15, 127, 48, BLACK);
					GLcd_DrawBitmap(img_del_arrow_bmp, 0, (old_point%3)*15 + 15);
					old_point =sub1_point;
					GLcd_DrawBitmap(img_arrow_bmp, 0, (sub1_point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((sub1_point/3)*3 +n >5)	break;
						GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ERR_MOTOR+(sub1_point/3)*3 +n),10, 10 + n*15, WHITE);
					}
				}				
				break;
				
			case DIS_ERROR_MOTOR:
				if(key_pressed== CANCEL)
					current_display = DIS_ERROR_INFO;				
				break;
				
			case DIS_ERROR_PAYMENT:
				if(key_pressed== CANCEL)
					current_display = DIS_ERROR_INFO;					
				break;
				
			case DIS_ERROR_SENSOR:
				if(key_pressed== CANCEL)
					current_display = DIS_ERROR_INFO;					
				break;
				
			case DIS_DEL_ERROR_MOTOR:
				if(key_pressed== CANCEL)
					current_display = DIS_ERROR_INFO;					
				break;
				
			case DIS_DEL_ERROR_HW:
				if(key_pressed== CANCEL)
					current_display = DIS_ERROR_INFO;					
				break;
				
			case DIS_DEL_ERROR_SENSOR:
				if(key_pressed== CANCEL)
					current_display = DIS_ERROR_INFO;					
				break;
				
			case DIS_ONOFF_HUMIDITY:
				if(key_pressed=='8' || key_pressed=='0') 
				{
					if(sub1_point)
						sub1_point =0;	// bat
					else
						sub1_point=1;		// tat
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				if(key_pressed==ENTER)
				{
					if(sub1_point)	IDM.EnableHumidity = 0;
					else	IDM.EnableHumidity = 1;
					Write_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed!=255)
				{
					GLcd_FillRect(0, 15, 5, 35, BLACK);
					GLcd_DrawBitmap(img_arrow_bmp, 0, sub1_point*15 + 15);					
				}
				break;
			case DIS_HUMIDITY_SET:
//				if(key_pressed=='0')
//					if(temp_data_Menu>5)	(temp_data_Menu%5==0) ? (temp_data_Menu -=5) : (temp_data_Menu -= temp_data_Menu%5);
//				if(key_pressed=='8')
//					if(temp_data_Menu<30)	(temp_data_Menu%5==0) ? (temp_data_Menu +=5) : (temp_data_Menu += 5 - temp_data_Menu%5);
//				if(key_pressed=='0' || key_pressed=='8')
//				{
//					sprintf(s, "%2d%%", temp_data_Menu);
//					GLcd_DrawString(s, 50, 30, WHITE);
//				}
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%100) *10 + (key_pressed-'0');
					if(temp_data_Menu >100)	temp_data_Menu =temp_data_Menu%100;		// Max
					sprintf(s, "%3d%% ", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}				
				if(key_pressed==ENTER)
				{
					IDM.HumidityMAX = temp_data_Menu;
					Write_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;
			case DIS_ACCEPT_DENO:
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				if(key_pressed == UP)
				{
					if(sub1_point==0)	sub1_point = m_setup_req.NumberOfChannels -1;
					else	sub1_point--;
					GLcd_FillRect(0,10,10,48, BLACK);
					GLcd_DrawBitmap(img_arrow_bmp, 0, (sub1_point%5)*10 + 10);
				}
				if(key_pressed == DOWN)
				{
					sub1_point++;
					if(sub1_point >=m_setup_req.NumberOfChannels)	sub1_point =0;
					GLcd_FillRect(0,10,10,48, BLACK);
					GLcd_DrawBitmap(img_arrow_bmp, 0, (sub1_point%5)*10 + 10);
				}
				if(key_pressed== ENTER)
				{
					if((temp_data_Menu>>sub1_point)&0x01) temp_data_Menu&=~(1<<sub1_point);
					else	temp_data_Menu|=(1<<sub1_point);
					m_device_config.accept_cash_max = temp_data_Menu;
					Buzz_On(100);
					save_device_config();
					NV11_SetAcceptNote(temp_data_Menu, 0);
				}
				if(key_pressed != 255)
				{
					if(sub1_point == 0 || sub1_point == 4 || sub1_point == 5 || sub1_point >=7)
							GLcd_FillRect(10,10,117,52, BLACK);
					for(int n=0; n< 5; n++)
					{
						if((n+(sub1_point/5)*5)>=m_setup_req.NumberOfChannels)	break;						
						sprintf(s,"%3d.000 %s",m_setup_req.ChannelData[n + (sub1_point/5)*5].value/1000, m_setup_req.ChannelData[n + (sub1_point/5)*5].cc);
						GLcd_DrawString(s,20, n*10 +10,WHITE);
						if((temp_data_Menu>>(n + (sub1_point/5)*5))&0x01)
						{
							GLcd_DrawLine(10, n*10 +12, 12, n*10 +15, WHITE);
							GLcd_DrawLine(12, n*10 +15, 16, n*10 +10, WHITE);
						}
						else
						{
							GLcd_DrawLine(10, n*10 +12, 12, n*10 +15, BLACK);
							GLcd_DrawLine(12, n*10 +15, 16, n*10 +10, BLACK);
						}
					}					
				}
				break;
			case DIS_TIME_SWAP_RUN:
//				if(key_pressed=='0')	
//					if(temp_data_Menu>20)	(temp_data_Menu%10==0) ? (temp_data_Menu -=10) : (temp_data_Menu -= temp_data_Menu%10);;
//				if(key_pressed=='8')
//					if(temp_data_Menu<120)	(temp_data_Menu%10==0) ? (temp_data_Menu +=10) : (temp_data_Menu += 10 -temp_data_Menu%10);;
//				if(key_pressed=='0' || key_pressed =='8')
//				{
//					sprintf(s, "%3ds", temp_data_Menu);
//					GLcd_DrawString(s, 50, 30, WHITE);
//				}
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%100) *10 + (key_pressed-'0');
					if(temp_data_Menu >100)	temp_data_Menu =temp_data_Menu%100;		// Max
//					if(temp_data_Menu<10)	temp_data_Menu = temp_data_Menu%10+10;
					sprintf(s, "%3ds ", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);					
				}				
				if(key_pressed ==ENTER)
				{
					IDM.TimeSWAPRun = temp_data_Menu;
					Write_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				break;
			case DIS_NUMBER_CELL_SWAP:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%1000) *10 + (key_pressed-'0');
					if(temp_data_Menu >1000)	temp_data_Menu =temp_data_Menu%1000;		// Max
					sprintf(s, "%4d ", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}				
				if(key_pressed ==ENTER)
				{
					IDM.TotalInsenseCycleSwapBuy = temp_data_Menu;
					IDM.currentTotalInsenseBuy = temp_data_Menu;
					Write_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				break;
			case DIS_CELL_MORE:
//				if(key_pressed=='0')	
//					if(temp_data_Menu>100)	(temp_data_Menu%50==0) ? (temp_data_Menu -=50) : (temp_data_Menu -= temp_data_Menu%50);
//				if(key_pressed=='8')
//					if(temp_data_Menu<1000)	(temp_data_Menu%50==0) ? (temp_data_Menu +=50) : (temp_data_Menu += 50 -temp_data_Menu%50);
//				if(key_pressed=='0' || key_pressed =='8')
//				{
//					sprintf(s, "%4d ", temp_data_Menu);
//					GLcd_DrawString(s, 50, 30, WHITE);
//				}
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%1000) *10 + (key_pressed-'0');
					if(temp_data_Menu >1000)	temp_data_Menu =temp_data_Menu%1000;		// Max
					sprintf(s, "%4d ", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}
				if(key_pressed ==ENTER)
				{
					IDM.NumberBuyMore = temp_data_Menu;
					IDM.currentNumberBuyMore = temp_data_Menu;
					Write_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				break;
			case DIS_NUMBER_CELL_MORE_EMPTY:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%10) *10 + (key_pressed-'0');
					if(temp_data_Menu >10)	temp_data_Menu =temp_data_Menu%10;		// Max
					if(temp_data_Menu==0)	temp_data_Menu =1;
					sprintf(s, "%2d ", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}
				if(key_pressed ==ENTER)
				{
					IDM.retryCellEmpty = temp_data_Menu;
					if(IDM.currentRetryCellEmpty > IDM.retryCellEmpty)	IDM.currentRetryCellEmpty = IDM.retryCellEmpty;
					Write_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;
			case DIS_DEL_CELL_ERROR:
				if(key_pressed==ENTER)
				{
	//				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DELETE_ERROR_CELL),10, 10, WHITE);
					
					IDM.currentRetryCellEmpty = IDM.retryCellEmpty;
					IDM.currentNumberBuyMore = IDM.NumberBuyMore;
					Write_config();
					buy.StateBuy = CELL_WAIT;
	//				IDM_state = 1;
					Buzz_On(100);
					printf("Retry: %d/%d\r\n Buy_state: %d\r\n", IDM.currentRetryCellEmpty, IDM.retryCellEmpty, buy.StateBuy);
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;
			case DIS_SETUP_TECH:
				if(key_pressed == UP)
				{
					if(point==0)	point =1;
					else	point --;
				}
				if(key_pressed == DOWN)
				{
					point ++;
					if(point>1)	point=0;
				}
				if(old_point != point)
				{
					old_point = point;
					GLcd_FillRect(0, 15, 127, 48, BLACK);
					GLcd_DrawBitmap(img_arrow_bmp, 0, (point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((point/3)*3 +n >1)	break;
						GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ID_MACHINE+(point/3)*3 +n),10, 10 + n*15, WHITE);
					}
				}
				if(key_pressed == ENTER)
				{
					sub1_point =0;
					switch(point)
					{
						case 0:
							current_display = DIS_SET_ID_MACHINE;
							break;
						case 1:
							current_display = DIS_SET_IP;
							break;
					}
				}
				if(key_pressed == CANCEL)	current_display = DIS_Password;
				break;
			case DIS_SET_IP:
				if(key_pressed>='0' && key_pressed<='9')
				{
					if(sub2_point/3 <4)
					{
						temp_menu_char[sub2_point/3] = (temp_menu_char[sub2_point/3]%100)*10 + key_pressed-'0';
					}
					else
						temp_data_Menu = (temp_data_Menu%1000)*10 + key_pressed-'0';
					sub2_point++;
					if(sub2_point>15)	sub2_point=0;
				}
				if(old_point!= sub2_point)
				{
					old_point= sub2_point;
					sprintf(s,"ID: %03d.%03d.%03d.%03d", temp_menu_char[0], temp_menu_char[1], temp_menu_char[2], temp_menu_char[3]);
					GLcd_DrawString(s, 0, 20, WHITE);
				
					sprintf(s,"PORT: %04d", temp_data_Menu);
					GLcd_DrawString(s, 0, 40, WHITE);
					
					GLcd_DrawLine(17, 30, 110, 30, BLACK);
					GLcd_DrawLine(17, 50, 60, 50, BLACK);
					if((sub2_point/3)<4)
					{
						GLcd_DrawLine(23+ (sub2_point/3)*24, 30, 35+ (sub2_point/3)*24, 30, WHITE);
					}
					else
						GLcd_DrawLine(36, 50, 56, 50, WHITE);
				}
				if(key_pressed == ENTER)
				{
					m_device_config.server.addr = (uint32_t)temp_menu_char[0];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_char[1];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_char[2];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_char[3];
					m_device_config.server.port = temp_data_Menu;
					save_device_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}
				if(key_pressed == CANCEL)	current_display = DIS_SETUP_TECH;
				break;
			case DIS_SET_ID_MACHINE:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_menu_char[sub2_point] = key_pressed;
					sub2_point++;
					if(sub2_point>6)	sub2_point=0;
				}
				if(old_point!= sub2_point)
				{
					old_point= sub2_point;
					GLcd_DrawLine(17, 30, 110, 30, BLACK);
					GLcd_DrawLine(23+ (sub2_point)*6, 30, 27+ (sub2_point)*6, 30, WHITE);					
					sprintf(s,"ID: %s", temp_menu_char);
					GLcd_DrawString(s, 0, 20, WHITE);
				}
				if(key_pressed == ENTER)
				{
					for(int n=0; n<7; n++)
						m_device_config.peripheral.machine_id[n] = temp_menu_char[n] -'0';
					save_device_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
					vdm_app_gsm_set_device_id(m_device_config.peripheral.machine_id);
				}
				if(key_pressed == CANCEL)	current_display = DIS_SETUP_TECH;
				break;			
			default:
				current_display = DIS_Home;
				break;
		}
		if(key_pressed!=255) GLcd_Flush();		
	}
}


/* Private function prototypes -----------------------------------------------*/


