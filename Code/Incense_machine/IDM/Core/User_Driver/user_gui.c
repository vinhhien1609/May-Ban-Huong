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
#include "TFT_glcd.h"
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
#include "All_75_Pic_256.h"
#include "touch.h"
#include "keyboard.h"
#include "graphic.h"
#include "vdm_fw.h"

unsigned char temp_menu_char[30];
unsigned int temp_menu_int[5], quantity_buy=0;
unsigned char point=0, old_point=0, sub1_point, sub2_point, Old_cell_state, stt_door, ischange=0;
unsigned int show_time=0;

//#define debug_display_info
#define COLOR_CHANGE	color_red
#define COLOR_MENU	color_green

#define MAX_TIMEOUT_MONEY	10

Display_Typedef current_display=DIS_Home, old_display=255, last_DIS=0;
rtc_date_time_t TimeSetting;
char str[30];


typedef uint16_t char16_t;

uint32_t temp_data_Menu;
char pass_shop[6] = VDM_DEVICE_CONFIG_DEFAULT_SHOP_PASSWORD;
char pass_tech[6] = VDM_DEVICE_CONFIG_DEFAULT_TECH_PASSWORD;
unsigned char isTouch=0, timeout_money=0;

extern GSM_State GSM;
extern uint8_t isSecond_display;
extern	rtc_date_time_t currentTime;
extern IDM_PARA IDM;
extern IDM_HARDWARE IDM_Status;
extern IDM_ERROR IDM_Errors;
extern BUY_PARA buy;
extern uint8_t isLCD_COLOR;

extern uint32_t m_lastest_note; // Menh gia to tien gan nhat
extern nv11_event_t m_last_n11_event;

extern vdm_device_config_t m_device_config;
extern unsigned char havemoney, IDM_state;
extern SSP6_SETUP_REQUEST_DATA m_setup_req;
extern 	float Humidity, Temperature;
extern int I_MOTOR, I_SWAP_M;

uint16_t dht_error;

/* Private define ------------------------------------------------------------*/

void draw_signal_gsm(void);


//static void small_delay()		// not importance function
//{
//	volatile uint32_t i = 0x02F;
//	while (i--);
//}


void show_Messenger(const char *messenger, uint16_t timeOn)
{
	if(isLCD_COLOR)
	{
		char str[100];
		uint16_t width = FontMakerGetWidth(messenger,&uArial_32);
		last_DIS = current_display;
		current_display = DIS_SHOW_MESSENGER;
		
		drawRoundRect((450-width)/2 + 10,240,width + 40, 100,10, color_blue, true);
		drawRoundRect((450-width)/2 + 10,240,width + 40, 100,10, color_brown, false);
		sprintf(str,"%s", messenger);
		LCD_background(color_blue);	
		TFT_putString((450-width)/2 +30,260,str,&Arial_32,color_green);
	}
	else
	{
		GLcd_FillRect(30, 10, 60, 35, BLACK);
		GLcd_DrawRect(30, 10, 60, 35, WHITE);
		GLcd_DrawStringUni(messenger, 35, 25, WHITE);
		last_DIS = current_display;
		current_display = DIS_SHOW_MESSENGER;
	}
		Buzz_On(100);	
}



//draw signal GSM
uint8_t csq=255;
void draw_signal_gsm(void)
{
	uint8_t t=0;//, n;
//	if(csq != GSM.CSQ)
//			csq = GSM.CSQ;
	if(GSM.SimReady==0)	GSM.CSQ =0;
//	else
//		return;
	if(isLCD_COLOR)
	{
//		GSM.CSQ =20;
//		drawRoundRect(10,9,80,40,0,color_blue,true);
		for(t=0; t<5; t++)	//draw y= 10-50
		{
			if(t<GSM.CSQ/7)
				drawRoundRect(20 + t*10, 22+(4-t)*5,6,25-(4-t)*5,2,color_green,true);		//10-->50
			else
			{
				drawRoundRect(20 + t*10, 22+(4-t)*5,6,25-(4-t)*5,2,color_blue,true);
				drawRoundRect(20 + t*10, 22+(4-t)*5,6,25-(4-t)*5,2,color_green,false);
			}
		}
	}
	else
	{
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
}

void Menu_draw(void)
{
	if(isLCD_COLOR)
		Menu_draw_TFT();
	else
		Menu_draw_MONO();
}

void draw_ON_OFF(unsigned int x, unsigned int y, bool state)
{
	if(state)
	{
		drawRoundRect(x,y,    50, 20,8, color_green, true);
		drawRoundRect(x+31,y+3,16, 14,6, color_white, true);	
	}
	else
	{
		drawRoundRect(x,y,    50, 20,8, color_gray, true);
		drawRoundRect(x+3,y+3,16, 14,6, color_white, true);
	}
}



void Menu_draw_MONO(void)
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
					NV11_Disable(true);
				}
				if(buy.StateBuy == STARTUP)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);
					NV11_Disable(true);
				}				
				if(buy.StateBuy == HELLO_CUSTOM)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);
					nv11_enable(true);
				}
				if(buy.StateBuy==WAIT_INSENCE)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_MONEY_RECEIVED),0, 10, WHITE);
					sprintf(s,"%3d.000VND", m_lastest_note/1000);
					GLcd_DrawString(s,64,15,WHITE);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_WAIT_RECEVER_INSENCE),15, 45, WHITE);
					NV11_Disable(true);
				}				
				if(buy.StateBuy ==INSENCE_EMPTY)
				{
					GLcd_FillRect(0, 10, 127, 50, BLACK);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_SORRY),40, 15, WHITE);
					GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_EMPTY_INSENCE),20, 30, WHITE);
					buy.StateBuy = HELLO_CUSTOM;
				}
				if(buy.StateBuy ==CELL_SUCCESS)
				{				
					buy.StateBuy = HELLO_CUSTOM;
				}
				if(buy.StateBuy ==DONATE)
				{
					buy.StateBuy = HELLO_CUSTOM;
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
				temp_menu_int[0] = (m_device_config.server.addr>>24)&0xFF;
				temp_menu_int[1] = (m_device_config.server.addr>>16)&0xFF;
				temp_menu_int[2] = (m_device_config.server.addr>>8)&0xFF;
				temp_menu_int[3] = m_device_config.server.addr&0xFF;
				temp_data_Menu = m_device_config.server.port;
				
				sprintf(s,"IP: %03d.%03d.%03d.%03d", temp_menu_int[0], temp_menu_int[1], temp_menu_int[2], temp_menu_int[3]);
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
				sprintf(s,"%5d", reven.number);
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
				sprintf(s,"%5d", reven.number);
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
				sprintf(s,"%5d", reven.number);
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
		if(key_pressed!=255 || get_touch_env()== TOUCH_DOWN)
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
					isSecond_display =0;
					if(show_time>0)	show_time --;
					sprintf(s,"%s %02d/%02d %2d  %02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
					GLcd_DrawString(s, 16, 0, WHITE);
					if(currentTime.second%2)
						GLcd_DrawString(":", 78, 0, WHITE);
					draw_signal_gsm();
					sprintf(s,"%2d%% ", (int)Humidity);
					GLcd_DrawString(s, 105, 0, WHITE);
					
//					sprintf(s,"E%3d", dht_error);
//					GLcd_DrawString(s, 96, 10, WHITE);
					if(m_lastest_note>100 && m_last_n11_event == NV11_NOTE_STACKED && buy.StateBuy == HELLO_CUSTOM)
					{
						
						if(m_device_config.item[0].price)
						{
							buy.StateBuy = SALES_SERVICE;
							m_device_config.closing_balance.closing_balance += m_lastest_note;
							save_device_config();
						}
						else
						{
							if(IDM.NumberInsenseBuy)
							{
								buy.StateBuy = WAIT_INSENCE;
								havemoney =1;
							}
							else
								buy.StateBuy = DONATE;
						}
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
							NV11_Disable(true);
						}
						if(buy.StateBuy==WAIT_INSENCE)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_MONEY_RECEIVED),0, 10, WHITE);
							sprintf(s,"%3d.000VND", m_lastest_note/1000);
//							m_lastest_note =0;
							GLcd_DrawString(s,64,15,WHITE);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_WAIT_RECEVER_INSENCE),15, 45, WHITE);
							NV11_Disable(true);
						}
						if(buy.StateBuy == HELLO_CUSTOM)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);
							nv11_enable(true);
						}
						if(buy.StateBuy ==INSENCE_EMPTY)
						{
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_SORRY),40, 15, WHITE);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_EMPTY_INSENCE),20, 30, WHITE);
							buy.StateBuy = HELLO_CUSTOM;
							if(buy.TotalIsenseDroped !=IDM.NumberInsenseBuy)
							{
								reven.number = IDM.NumberInsenseBuy - buy.TotalIsenseDroped;
								reven.money =0;
								add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);
							}
							vdm_app_gsm_send_qrm_sale_fail_frame((m_lastest_note/1000)&0xFF, 0xFF, 0x01);
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
							buy.StateBuy = HELLO_CUSTOM;
							vdm_app_gsm_send_released_item_frame((m_lastest_note/1000)&0xFF, 0x01);
						}
						if(buy.StateBuy ==DONATE)
						{
							show_time=10;
							GLcd_FillRect(0, 10, 127, 50, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_THANK),15, 25, WHITE);					
							buy.StateBuy = HELLO_CUSTOM;
							vdm_app_gsm_send_released_item_frame((m_lastest_note/1000)&0xFF, 0x01);
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
					sprintf(s,"%d   ", reven.number);
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
					sprintf(s,"%5d", reven.number);
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
					sprintf(s,"%5d", reven.number);
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
					buy.StateBuy = HELLO_CUSTOM;
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
						temp_menu_int[sub2_point/3] = (temp_menu_int[sub2_point/3]%100)*10 + key_pressed-'0';
					}
					else
						temp_data_Menu = (temp_data_Menu%1000)*10 + key_pressed-'0';
					sub2_point++;
					if(sub2_point>15)	sub2_point=0;
				}
				if(old_point!= sub2_point)
				{
					old_point= sub2_point;
					sprintf(s,"ID: %03d.%03d.%03d.%03d", temp_menu_int[0], temp_menu_int[1], temp_menu_int[2], temp_menu_int[3]);
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
					m_device_config.server.addr = (uint32_t)temp_menu_int[0];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_int[1];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_int[2];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_int[3];
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
void Menu_draw_TFT(void)
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
			vdm_app_gsm_send_door_frame(false);
			current_display = DIS_Password;
			NV11_Disable(true);
			if(m_device_config.sensor_error.error)
			{
				m_device_config.sensor_error.error =0;
				IDM.currentRetryCellEmpty = IDM.retryCellEmpty;
				IDM.currentNumberBuyMore = IDM.NumberBuyMore;
				
				Write_config();
				buy.StateBuy = HELLO_CUSTOM;
				printf("Retry: %d/%d\r\n Buy_state: %d\r\n", IDM.currentRetryCellEmpty, IDM.retryCellEmpty, buy.StateBuy);
				if(m_device_config.closing_balance.closing_balance)
				{
					m_device_config.closing_balance.closing_balance =0;
					save_device_config();
				}
			}
		}
		else
		{
			vdm_app_gsm_send_door_frame(true);
			current_display = DIS_Home;
		}
	}
	
	if(isTouch)
	{
		isTouch =0;
		GSLX680_read_data();
	}
	
	if(current_display!= old_display)		//init etc display
	{
		old_display = current_display;
		switch(current_display)
		{
			case DIS_SHOW_MESSENGER:
				show_time=0;
				break;
			case DIS_SALE:
					drawRoundRect(10,140,450, 330,10, color_blue, true);
					drawRoundRect(10,140,450, 330,10, color_brown, false);
//					Enable_Vitual_key(true);
					Enable_Vitual_Confirm_key(true);
					LCD_background(color_blue);
					
					sprintf(s, "%s", vdm_language_text(VDM_LANG_BALANCE));
					TFT_putString((200-FontMakerGetWidth(s,&ufont_16))/2 +30,145,s,&font_16,color_green_bright);
					drawRoundRect(30,180,200, 60,10, color_brown, false);
					
					if(m_device_config.closing_balance.closing_balance>=1000)
						sprintf(s, "%d %03d", m_device_config.closing_balance.closing_balance/1000, m_device_config.closing_balance.closing_balance%1000);
					else
						sprintf(s, "%d", m_device_config.closing_balance.closing_balance);
					TFT_DrawString_option(s,30 + (200 - FontGetWidth(s))/2, 185, color_green, false, 3);
					
					sprintf(s, "%s (VND)", vdm_language_text(VDM_LANG_UNIT_PRICE));
					TFT_putString((200-FontMakerGetWidth(s,&ufont_16))/2 +235,145,s,&font_16,color_green_bright);
					drawRoundRect(235,180,200, 60,10, color_brown, false);
					if(m_device_config.item[0].price>=1000)
						sprintf(s, "%d %03d", m_device_config.item[0].price/1000, m_device_config.item[0].price%1000);
					else
						sprintf(s, "%d", m_device_config.item[0].price);
					TFT_DrawString_option(s,235 + (200 - FontGetWidth(s))/2, 185, color_green, false, 3);						

					sprintf(s, "%s", vdm_language_text(VDM_LANG_MISSING_AMOUNT));
					TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,375,s,&font_16,color_green_bright);
					drawRoundRect(50,405,350, 60,10, color_brown, false);
					LCD_background(color_blue);
					if(m_device_config.item[0].price > m_device_config.closing_balance.closing_balance)
					{
						if(m_device_config.item[0].price - m_device_config.closing_balance.closing_balance>=1000)
							sprintf(s, "-%d %03d",(m_device_config.item[0].price - m_device_config.closing_balance.closing_balance)/1000, (m_device_config.item[0].price - m_device_config.closing_balance.closing_balance)%1000);
						else
							sprintf(s, "-%d",(m_device_config.item[0].price - m_device_config.closing_balance.closing_balance));
						TFT_DrawString_option(s,50 + (350 - FontGetWidth(s))/2, 410, color_red, false, 3);
					}
					else
					{
						if((m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)==0)
							sprintf(s, "0");
						else
						{						
							if((m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)>=1000)
								sprintf(s, "+%d %03d",(m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)/1000, (m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)%1000);
							else
								sprintf(s, "+%d",(m_device_config.closing_balance.closing_balance % m_device_config.item[0].price));
						}
						TFT_DrawString_option(s,50 + (350 - FontGetWidth(s))/2, 410, color_green, false, 3);
					}
//					if(m_device_config.item[0].price > m_device_config.closing_balance.closing_balance)
//					{
//						sprintf(s, "%3d",m_device_config.item[0].price - m_device_config.closing_balance.closing_balance);
//					}
//					else
//						sprintf(s, "%3d",0);
//					TFT_DrawString_option(s,250, 410, color_red, false, 3);
			
					sprintf(s, "%s", vdm_language_text(VDM_LANG_QUANLITY));
					TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,260,s,&font_16,color_green_bright);
					drawRoundRect(50,290,350, 60,10, color_white, true);
					drawRoundRect(50,290,350, 60,10, color_brown, false);
					quantity_buy = m_device_config.closing_balance.closing_balance/m_device_config.item[0].price;
					sprintf(s, "%d", quantity_buy);
					LCD_background(color_white);
					TFT_DrawString_option(s,50+ (350 - FontGetWidth(s))/2, 295, color_green, false, 3);
					timeout_money = MAX_TIMEOUT_MONEY;
					nv11_enable(true);
				break;
			case DIS_Home:
				Access_layer(0);				
				LCD_background(color_black);		
				Memory_Clear();
				if(vdm_device_get_mode()== MERIT_MODE)
					sprintf(s,"CD");
				if(vdm_device_get_mode()== SALES_MODE)
					sprintf(s,"BH");
				if(vdm_device_get_mode()== FIX_MODE)
					sprintf(s,"CP");				
				TFT_putString(100, 28,s,&font_16,color_green_bright);
				
				sprintf(s,"%s",VDM_FIRMWARE_VERSION);
				TFT_putString(600, 28,s,&font_16,color_green_bright);			
				Enable_Vitual_key(false);
//				csq = 255;
				show_time=0;
				Old_cell_state =HELLO_CUSTOM;
				if(buy.StateBuy==STOP_SERVICE)
				{
					Displaypicture(Stop_Service);
					NV11_Disable(true);
				}
				if(buy.StateBuy == STARTUP)
				{
					Displaypicture(Hello);
					NV11_Disable(true);
				}				
				if(buy.StateBuy == HELLO_CUSTOM)
				{
					Displaypicture(Hello);
					nv11_enable(true);
				}
				if(buy.StateBuy==WAIT_INSENCE)
				{
					Displaypicture(Wait_insense);
					LCD_background(color_white);

//					sprintf(s,"%3d.000", m_lastest_note/1000);
//					TFT_DrawString(s, 400, 200, color_red);
//					TFT_putString((400-FontMakerGetWidth(s,&uArial_32))/2 +200,210,s,&Arial_32,color_red);					
					NV11_Disable(true);
				}				
				if(buy.StateBuy ==INSENCE_EMPTY)
				{
					Displaypicture(Empty);
					buy.StateBuy = HELLO_CUSTOM;
				}
				if(buy.StateBuy ==CELL_SUCCESS)
				{				
					buy.StateBuy = HELLO_CUSTOM;
				}
				if(buy.StateBuy ==DONATE)
				{
					buy.StateBuy = HELLO_CUSTOM;
				}
				LCD_background(color_black);
				if(currentTime.second%2)
					sprintf(s,"%s %02d/%02d %02d:%02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
				else
					sprintf(s,"%s %02d/%02d %02d %02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
				TFT_DrawString_option(s,550, 440, color_green, false, 2);
				draw_signal_gsm();
				sprintf(s,"%2d%% ", (int)Humidity);
				TFT_putString(400, 28,s,&font_16,color_green_bright);
				sprintf(s,"%2dC ",(int)Temperature);
				TFT_putString(250, 28,s,&font_16,color_green_bright);
				
				sprintf(s,"%s",VDM_FIRMWARE_VERSION);
				TFT_putString(600, 28,s,&font_16,color_green_bright);				
				
				break;
			case DIS_Password:
				LCD_background(color_black);
				Memory_Clear();				
				Displaypicture(Setting_Sell);		
				LCD_background(color_blue);
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				sprintf(s,"%s",vdm_language_text(VDM_LANG_PASSWORD));
//				TFT_putString((450-FontMakerGetWidth(s,&uArial_32))/2 +10,150,s,&Arial_32,GREEN);			
				TFT_putString((450-FontMakerGetWidth(s,&uArial_32))/2 +10,150,s,&Arial_32,color_green);
				TFT_DrawString_option("------", 130, 220, color_red,false,4);			
				memcpy(temp_menu_char, "------", 6);
				Enable_Vitual_key(true);
				sprintf((char*)temp_menu_char,"");
				point=0;
				break;
			case DIS_Setup:
				LCD_background(color_black);
				Memory_Clear();
				Displaypicture(Setting_Sell);
//				TFT_putString(63,280,"CÃ i sá»‘ láº§n bÃ¡n lá»—i",&font_16,color_green_bright);
				Enable_Vitual_key(false);
				old_point = 255;
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
				LCD_background(color_black);
				Memory_Clear();				
				Displaypicture(Setting_Tech);
				drawRoundRect(55+(old_point/7)*235,70+(old_point%7)*40,220, 30,10, color_black, false);
				drawRoundRect(56+(old_point/7)*235,71+(old_point%7)*40,218, 28,8, color_black, false);
				if(m_device_config.run_mode == MERIT_MODE)
					draw_ON_OFF(450, 75, true);
				else
					draw_ON_OFF(450, 75, false);
				if(m_device_config.run_mode == FIX_MODE)
					draw_ON_OFF(450, 115, true);
				else
					draw_ON_OFF(450, 115, false);
				if(m_device_config.run_mode >= SALES_MODE)
					draw_ON_OFF(450, 155, true);
				else
					draw_ON_OFF(450, 155, false);
				Enable_Vitual_key(false);
				old_point = 255;
				break;
			case DIS_SET_IP:
				sub2_point=0;
				old_point =0;
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_SERVER_IP));
				LCD_background(color_blue);
				TFT_putString(150,150,s,&font_16,color_green_bright);
			
				temp_menu_int[0] = (m_device_config.server.addr>>24)&0xFF;
				temp_menu_int[1] = (m_device_config.server.addr>>16)&0xFF;
				temp_menu_int[2] = (m_device_config.server.addr>>8)&0xFF;
				temp_menu_int[3] = m_device_config.server.addr&0xFF;
				temp_data_Menu = m_device_config.server.port;
				
				sprintf(s,"IP:%03d.%03d.%03d.%03d", temp_menu_int[0], temp_menu_int[1], temp_menu_int[2], temp_menu_int[3]);
				LCD_background(color_blue);
				TFT_DrawString_option(s,20, 200, color_green, false, 3);
				drawRoundRect(92,205,71, 40,4, color_green, false);
				sprintf(s,"PORT: %04d", temp_data_Menu);
				TFT_DrawString_option(s,20, 250, color_green, false, 3);			
				break;
			case DIS_SET_ID_MACHINE:
				sub2_point=0;
				old_point = 0;
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
				sprintf(s, "%s",vdm_language_text(VDM_LANG_ID_MACHINE));
				LCD_background(color_blue);
				TFT_putString(150,150,s,&font_16,color_green_bright);
			
				for(int n=0; n<7; n++)
					temp_menu_char[n] = m_device_config.peripheral.machine_id[n] + '0';
				temp_menu_char[7]=0;
				sprintf(s,"ID: %s", temp_menu_char);
				LCD_background(color_blue);
				TFT_DrawString_option(s,20, 200, color_green, true, 3);
				drawRoundRect(116+ old_point*24,205,24, 40,4, COLOR_MENU, false);

				break;
			case DIS_PHONE_NUMBER:
				sub2_point=0;
				old_point = 0;
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
				sprintf(s, "%s",vdm_language_text(VDM_LANG_PHONE_NUMBER));
				LCD_background(color_blue);
				TFT_putString(150,150,s,&font_16,color_green_bright);
			
				for(int n=0; n<10; n++)
					temp_menu_char[n] = m_device_config.phone.phone[n];
				temp_menu_char[10]=0;
				sprintf(s,"%s", temp_menu_char);
				LCD_background(color_blue);
				TFT_DrawString_option(s,118, 200, color_green, true, 3);
				drawRoundRect(116+ old_point*24,205,24, 40,4, COLOR_MENU, false);				
				break;
			case DIS_TOTAL:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				LCD_background(color_blue);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_TOTAL));
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
				sprintf(s, "%s",vdm_language_text(VDM_LANG_NUMBER));
				TFT_putString(20,230,s,&font_16,color_green_bright);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_TOTAL_MONEY));
				TFT_putString(20,290,s,&font_16,color_green_bright);
			
				revenue_total revenTotal = get_revenue_total();
				sprintf(s,"%9lu000", revenTotal.money);
				TFT_DrawString_option(s,125, 210, color_green, false, 3);
				sprintf(s,"%9lu", revenTotal.number);
				TFT_DrawString_option(s,125, 270, color_green, false, 3);			
				break;
			case DIS_DAY:
			
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				LCD_background(color_blue);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_DAY));
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green);
			
				sprintf(s, "%s",vdm_language_text(VDM_LANG_NUMBER));
				TFT_putString(20,230,s,&font_16,color_green);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_TOTAL_MONEY));
				TFT_putString(20,290,s,&font_16,color_green);
			
				TimeSetting = currentTime;
				sprintf(s,"%02d/%02d/%4d",TimeSetting.day, TimeSetting.month,TimeSetting.year);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,color_green);
			
				reven = get_revenue_day(TimeSetting.day,TimeSetting.month, TimeSetting.year);
				sprintf(s,"%7d000", reven.money);
				TFT_DrawString_option(s,150, 270, color_green, false, 3);
				sprintf(s,"%7d", reven.number);
				TFT_DrawString_option(s,150, 210, color_green, false, 3);
				break;
			case DIS_MONTH:
//				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
//				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);
			
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				LCD_background(color_blue);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_MONTH));
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
				sprintf(s, "%s",vdm_language_text(VDM_LANG_NUMBER));
				TFT_putString(20,230,s,&font_16,color_green_bright);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_TOTAL_MONEY));
				TFT_putString(20,290,s,&font_16,color_green_bright);
			
				TimeSetting = currentTime;
				sprintf(s,"%02d/%4d",TimeSetting.month,TimeSetting.year);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,color_green_bright);
			
				reven = get_revenue_month(TimeSetting.month, TimeSetting.year);
				sprintf(s,"%7d000", reven.money);
				TFT_DrawString_option(s,150, 270, color_green, false, 3);
				sprintf(s,"%7d", reven.number);
				TFT_DrawString_option(s,150, 210, color_green, false, 3);
				break;				
//			case DIS_MONTH:
//				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
//				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);
//				TimeSetting = currentTime;				
//				sprintf(s,"%02d/%4d",TimeSetting.month,TimeSetting.year);
//				GLcd_DrawString(s, 40, 10,WHITE);
//				revenue_t reven = get_revenue_month(TimeSetting.month, TimeSetting.year);
//				sprintf(s,"%d000", reven.money);
//				GLcd_DrawString(s, 60, 45,WHITE);
//				sprintf(s,"%d", reven.number);
//				GLcd_DrawString(s, 60, 30,WHITE);			
//				break;
			
			case DIS_YEAR:
//				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
//				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);
			
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				LCD_background(color_blue);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_YEAR));
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
				sprintf(s, "%s",vdm_language_text(VDM_LANG_NUMBER));
				TFT_putString(20,230,s,&font_16,color_green_bright);
				sprintf(s, "%s",vdm_language_text(VDM_LANG_TOTAL_MONEY));
				TFT_putString(20,290,s,&font_16,color_green_bright);
			
				TimeSetting = currentTime;
				sprintf(s,"%4d",TimeSetting.year);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,color_green_bright);
			
				reven = get_revenue_year(TimeSetting.year);
				sprintf(s,"%7d000", reven.money);
				TFT_DrawString_option(s,150, 270, color_green, false, 3);
				sprintf(s,"%7d", reven.number);
				TFT_DrawString_option(s,150, 210, color_green, false, 3);
				break;				
	
			
			case DIS_CELL_EACH_TIME:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				sprintf(s, "%s (%d)",vdm_language_text(VDM_LANG_TOTAL_SETTING), IDM.NumberInsenseBuy);
//				uint16_t n = FontMakerGetWidth(s,&ufont_16);
//				sprintf(s, "%s (%d)",s, n);
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
				LCD_background(color_blue);
				sprintf(s, "%3d", IDM.NumberInsenseBuy);
				TFT_DrawString_option(s,200, 200, color_green, false, 3);
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
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				LCD_background(color_blue);
				TFT_putString(150,160,vdm_language_text(VDM_LANG_EN_CONTROL_HUMIDITY),&font_16,color_green_bright);
				if(IDM.EnableHumidity==0)
				{
					drawRoundRect(150,220,100, 30,15, color_gray, true);
					drawRoundRect(152,222,30, 26,12, color_white, true);
					sub1_point =1;
				}
				else
				{
					drawRoundRect(150,220,100, 30,15, color_green, true);
					drawRoundRect(218,222,30, 26,12, color_white, true);
					sub1_point =0;
				}
				Enable_Vitual_key(true);
			
				break;
			
			case DIS_HUMIDITY_SET:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
				sprintf(s, "%s (%d%%)", vdm_language_text(VDM_LANG_HUMUDITY_SETTING), IDM.HumidityMAX);
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
				sprintf(s, "%d%%", IDM.HumidityMAX);
				LCD_background(color_blue);
				TFT_DrawString_option(s,200, 200, color_green, false, 3);
				temp_data_Menu = IDM.HumidityMAX;
				break;
			
			case DIS_NUMBER_CELL_ERROR:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
				sprintf(s, "%s (%d)", vdm_language_text(VDM_LANG_NUMBER_SALE_ERROR), IDM.retryCellEmpty);
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
				sprintf(s, "%d ", IDM.retryCellEmpty);
				LCD_background(color_blue);
				TFT_DrawString_option(s,200, 200, color_green, false, 3);
				temp_data_Menu = IDM.retryCellEmpty;				
				break;
			case DIS_ACCEPT_DENO:
				temp_data_Menu = m_device_config.accept_cash_max;
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
//				TFT_putString(150,150,vdm_language_text(VDM_LANG_ACCEPT_MOUNT),&font_16,GREEN);
				sprintf(s,"%s",vdm_language_text(VDM_LANG_ACCEPT_MOUNT));
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
				for(int n=0; n< m_setup_req.NumberOfChannels; n++)
				{
					sprintf(s,"%3d.000 %s",m_setup_req.ChannelData[n].value/1000, m_setup_req.ChannelData[n].cc);
					
					if((temp_data_Menu>>n)&0x01)
					{
						TFT_DrawString_option(s,10+n/5*250, (n%5)*50 +170, color_green, true, 2);
					}
					else
					{	
						TFT_DrawString_option(s,10+n/5*250, (n%5)*50 +170, color_gray, true, 2);					
					}
				}
				break;
			case DIS_TIME_SWAP_RUN:		// thoi gian swap
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);			
				sprintf(s, "%s (%ds)", vdm_language_text(VDM_LANG_TIME_RUN_INSENSE), IDM.TimeSWAPRun);
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
				sprintf(s, "%3ds ", IDM.TimeSWAPRun);
				LCD_background(color_blue);
				TFT_DrawString_option(s,200, 200, color_green, false, 3);
				temp_data_Menu = IDM.TimeSWAPRun;
				Enable_Vitual_key(true);
				break;
			case DIS_NUMBER_CELL_SWAP:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
				sprintf(s, "%s (%d)", vdm_language_text(VDM_LANG_TOTAL_INSENSE_CELL), IDM.TotalInsenseCycleSwapBuy);
				LCD_background(color_blue);
				TFT_putString(150,160,s,&font_16,color_green_bright);
			
				sprintf(s, "%d", IDM.TotalInsenseCycleSwapBuy);
				LCD_background(color_blue);
				TFT_DrawString_option(s,200, 200, color_green, false, 3);
				temp_data_Menu = IDM.TotalInsenseCycleSwapBuy;
				break;
			case DIS_CELL_MORE:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
				sprintf(s, "%s (%d)", vdm_language_text(VDM_LANG_INSENSE_CELL_MORE), IDM.NumberBuyMore);
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
				sprintf(s, "%d", IDM.NumberBuyMore);
				LCD_background(color_blue);
				TFT_DrawString_option(s,200, 200, color_green, false, 3);
				temp_data_Menu = IDM.NumberBuyMore;
				break;
			case DIS_NUMBER_CELL_MORE_EMPTY:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);			
				sprintf(s, "%s (%d)", vdm_language_text(VDM_LANG_NUMBER_CELL_MORE), IDM.retryCellEmpty);
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
				sprintf(s, "%2d", IDM.retryCellEmpty);
				TFT_putString(150,160,s,&font_16,color_green_bright);
				temp_data_Menu = IDM.retryCellEmpty;				
				break;
			case DIS_DEL_CELL_ERROR:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				sprintf(s,"%s", vdm_language_text(VDM_LANG_DELETE_ERROR_CELL));
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);			
				break;
			case DIS_PRICE_SET:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				temp_data_Menu = m_device_config.item[0].price;
				sprintf(s, "%s (%d VND)", vdm_language_text(VDM_LANG_UNIT_PRICE), temp_data_Menu);
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
				if(temp_data_Menu>1000)
					sprintf(s, "%3d %03d (VND)", temp_data_Menu/1000, temp_data_Menu%1000);
				else	
					sprintf(s, "%7d (VND)", temp_data_Menu);
				LCD_background(color_blue);
//				TFT_putString(450-FontMakerGetWidth(s,&uArial_32),200,s,&Arial_32,COLOR_MENU);
				TFT_DrawString_option(s,100, 200, COLOR_MENU, false, 3);			
			
				break;
			case DIS_VERSION_INFO:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				sprintf(s, "%s", vdm_language_text(VDM_LANG_VERION_INFO));
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
			
//				if(temp_data_Menu>1000)
//					sprintf(s, "%3d %03d (VND)", temp_data_Menu/1000, temp_data_Menu%1000);
//				else	
//					sprintf(s, "%7d (VND)", temp_data_Menu);
//				LCD_background(color_blue);
////				TFT_putString(450-FontMakerGetWidth(s,&uArial_32),200,s,&Arial_32,COLOR_MENU);
//				TFT_DrawString_option(s,100, 200, COLOR_MENU, false, 3);					
				break;
			case DIS_MOTOR_PROTECT:
				drawRoundRect(10,140,450, 330,10, color_blue, true);
				drawRoundRect(10,140,450, 330,10, color_brown, false);
				Enable_Vitual_key(true);
				sprintf(s, "%s", vdm_language_text(VDM_PROTECT_SETUP));
				LCD_background(color_blue);
				TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,150,s,&font_16,color_green_bright);
				//conveyer
//				TFT_putString(170,180,"MAX(mA)",&font_16,color_green_bright);
//				TFT_putString(300,180,"CURRENT(mA)",&font_16,color_green_bright);
//				TFT_putString(20,220,vdm_language_text(VDM_MOTOR_CONVEYOR),&font_16,color_green_bright);
//				TFT_putString(20,270,vdm_language_text(VDM_MOTOR_SWAP),&font_16,color_green_bright);
				TFT_putString(20,220,"MAX(mA)",&font_16,color_green_bright);
				TFT_putString(20,270,"ON/OFF",&font_16,color_green_bright);
				TFT_putString(20,320,"ENABLE",&font_16,color_green_bright);
				TFT_putString(20,370,"CURRENT(mA)",&font_16,color_green_bright);			
				TFT_putString(170,180,vdm_language_text(VDM_MOTOR_CONVEYOR),&font_16,color_green_bright);
				TFT_putString(300,180,vdm_language_text(VDM_MOTOR_SWAP),&font_16,color_green_bright);
			
				temp_menu_int[0] = m_device_config.motor.max_current_conveyer;
				temp_menu_int[1] = m_device_config.motor.max_current_swap;
				sprintf(s, "%4d", temp_menu_int[0]);
				TFT_DrawString_option(s,170, 200, color_green_bright, false, 3);
				sprintf(s, "%4d", temp_menu_int[1]);
				TFT_DrawString_option(s,170+150, 200, color_green_bright, false, 3);
				sprintf(s, "%4d", I_MOTOR);
				TFT_DrawString_option(s,170, 350, COLOR_MENU, false, 3);
				sprintf(s, "%4d", I_SWAP_M);
				TFT_DrawString_option(s,170+150, 350, COLOR_MENU, false, 3);
				
				sprintf(s, "%s", (m_device_config.motor.trip_over==0) ? " OFF": "  ON");
				TFT_DrawString_option(s,170, 300, color_green_bright, false, 3);
				
				sub1_point =0;
				ischange =0;
				drawRoundRect(165,205,100, 45,3, color_green, false);
//				drawRoundRect(165,205+50,100, 45,3, color_brown, false);
				
				break;			
			default:
				current_display = DIS_Home;
				break;
		}
#ifdef debug_display_info
		sprintf(s,"DIS: %2d", current_display);
		printf(s);	printf("\r\n");
		TFT_DrawString(s, 20, 400, color_green);	
#endif		
	}
////////////////////////////////////////////////////////////////////////////////////////	
	else
	{
		key_pressed = scan_BT();
		scan_vitual_key();
		if(key_pressed!=255 || get_touch_env()==TOUCH_DOWN)
			isSecond_display =0;
		if(isSecond_display>120)
		{
			isSecond_display =0;
			if(IDM_Status.isDoorOpen==true)
			{
				current_display = DIS_Password;
			}
			else
			{
				current_display = DIS_Home;
				Enable_Vitual_key(false);
			}
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

				
			case DIS_SALE:
				if(isSecond_display)
				{
					isSecond_display = 0;
					if(show_time>0)	show_time --;
					if(show_time==0)
					{
						current_display = DIS_Home;
						Enable_Vitual_key(false);			
						isSecond_display =0;						
					}
					if(timeout_money)	timeout_money--;
					if(m_lastest_note>100 && m_last_n11_event == NV11_NOTE_STACKED)
					{
							show_time =300;
							m_device_config.closing_balance.closing_balance += m_lastest_note;
							save_device_config();
							m_last_n11_event = NV11_EVENT_NONE;
						//luu doanh thu
						reven.money = m_lastest_note/1000;
						reven.number =0;
						add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);
						timeout_money = 5;//MAX_TIMEOUT_MONEY;
//						LCD_background(color_blue);

						drawRoundRect(51,406,348, 58,9, color_blue, true);
						LCD_background(color_blue);
						if(m_device_config.item[0].price > m_device_config.closing_balance.closing_balance)
						{
							if(m_device_config.item[0].price - m_device_config.closing_balance.closing_balance>=1000)
								sprintf(s, "-%d %03d",(m_device_config.item[0].price - m_device_config.closing_balance.closing_balance)/1000, (m_device_config.item[0].price - m_device_config.closing_balance.closing_balance)%1000);
							else
								sprintf(s, "-%d",(m_device_config.item[0].price - m_device_config.closing_balance.closing_balance));
							TFT_DrawString_option(s,50 + (350 - FontGetWidth(s))/2, 410, color_red, false, 3);
						}
						else
						{
							if((m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)==0)
								sprintf(s, "0");
							else
							{
								if((m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)>=1000)
									sprintf(s, "+%d %03d",(m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)/1000, (m_device_config.closing_balance.closing_balance % m_device_config.item[0].price)%1000);
								else
									sprintf(s, "+%d",(m_device_config.closing_balance.closing_balance % m_device_config.item[0].price));
							}
							TFT_DrawString_option(s,50 + (350 - FontGetWidth(s))/2, 410, color_green, false, 3);
						}						
						
//						if(m_device_config.item[0].price > m_device_config.closing_balance.closing_balance)
//						{
//							sprintf(s, "-%d ",m_device_config.item[0].price - m_device_config.closing_balance.closing_balance);
//							TFT_DrawString_option(s,150, 410, color_red, false, 3);
//						}
//						else
//						{
//							sprintf(s, "+%d ",m_device_config.closing_balance.closing_balance % m_device_config.item[0].price);
//							TFT_DrawString_option(s,150, 410, color_green, false, 3);
//						}
						
						drawRoundRect(31,181,198, 58,9, color_blue, true);
						if(m_device_config.closing_balance.closing_balance>=1000)
							sprintf(s, "%d %03d", m_device_config.closing_balance.closing_balance/1000, m_device_config.closing_balance.closing_balance%1000);
						else
							sprintf(s, "%d", m_device_config.closing_balance.closing_balance);
						TFT_DrawString_option(s,30 + (200 - FontGetWidth(s))/2, 185, color_green, false, 3);
						////////////////
						
//						sprintf(s, "%6d", m_device_config.closing_balance.closing_balance);
//						TFT_DrawString_option(s,65, 185, color_green, false, 3);

						drawRoundRect(51,291,348, 58,9, color_white, true);
						quantity_buy = m_device_config.closing_balance.closing_balance/m_device_config.item[0].price;
						sprintf(s, "%d", quantity_buy);
						LCD_background(color_white);
						TFT_DrawString_option(s,50+ (350 - FontGetWidth(s))/2, 295, color_green, false, 3);									
//						quantity_buy = m_device_config.closing_balance.closing_balance/m_device_config.item[0].price;
//						sprintf(s, "%3d", quantity_buy);			
//						LCD_background(color_white);
//						TFT_DrawString_option(s,250, 295, color_red, false, 3);	
						nv11_enable(true);						
					}
				}	
				if(m_device_config.closing_balance.closing_balance >= m_device_config.item[0].price)
					active_keyboard(true);
				else
					active_keyboard(false);
				
				if(key_pressed==ENTER || timeout_money==0)
				{
					if(m_device_config.closing_balance.closing_balance >= m_device_config.item[0].price)
					{
						quantity_buy = m_device_config.closing_balance.closing_balance/m_device_config.item[0].price;
						buy.StateBuy = WAIT_INSENCE;
						havemoney =1;
						current_display = DIS_Home;
					}
				}				
				if(key_pressed==CANCEL)
					current_display = last_DIS;
//				if(key_pressed>='0' && key_pressed <='9')
//				{
//					quantity_buy = quantity_buy*10 + (key_pressed-'0');
//					if(quantity_buy>100)	quantity_buy%=100;
//					if(quantity_buy * m_device_config.item[0].price > m_device_config.closing_balance.closing_balance)
//					{
//						sprintf(s, "%6d",quantity_buy * m_device_config.item[0].price - m_device_config.closing_balance.closing_balance);
//					}
//					else
//						sprintf(s, "%6d",0);
//					LCD_background(color_blue);
//					TFT_DrawString_option(s,250, 410, color_green, false, 3);
//					
//					sprintf(s, "%3d", quantity_buy);
//					LCD_background(color_white);
//					TFT_DrawString_option(s,250, 295, color_red, false, 3);
//				}
							
				
				break;
			case DIS_Home:
				if(get_touch(TOUCH_DOWN, 0, 0, 799, 479)== true && m_device_config.run_mode == SALES_MODE && IDM_state==1 && buy.StateBuy!=STOP_SERVICE)
				{
					Buzz_On(100);
					last_DIS = current_display;
					current_display = DIS_SALE;
					show_time = 300;
				}
				if(isSecond_display)
				{
					isSecond_display =0;
					if(show_time>0)	show_time --;
//					Displaypicture(Old_cell_state);
//					sprintf(s,"%s %02d/%02d %02d  %02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
					LCD_background(color_black);
//					TFT_DrawString(s, 16, 50, color_white);
					if(currentTime.second%2)
//						TFT_DrawString(":", 195, 50, color_white);
						sprintf(s,"%s %02d/%02d/%04d %02d:%02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.year, currentTime.hour, currentTime.minute);
					else
						sprintf(s,"%s %02d/%02d/%04d %02d %02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.year, currentTime.hour, currentTime.minute);

					TFT_DrawString_option(s,460, 440, color_green, false, 2);
					draw_signal_gsm();
					sprintf(s,"%02d%% ", (int)Humidity);
					TFT_putString(400, 28,s,&font_16,color_green_bright);
					sprintf(s,"%02dC ",(int)Temperature);
//					TFT_DrawString(s, 250, 20, color_green);
//					TFT_DrawString_option(s,250, 20, color_green, false, 2);
					TFT_putString(250, 28,s,&font_16,color_green_bright);
					
					
					if(m_lastest_note>100 && m_last_n11_event == NV11_NOTE_STACKED && buy.StateBuy == HELLO_CUSTOM)
					{
						if(m_device_config.run_mode== SALES_MODE)
						{
//							buy.StateBuy = SALES_SERVICE;
							m_device_config.closing_balance.closing_balance += m_lastest_note;
							save_device_config();
							Buzz_On(100);
							last_DIS = current_display;
							current_display = DIS_SALE;
							show_time = 300;
						}
						else
						{
							if(m_device_config.closing_balance.closing_balance)
							{
								m_device_config.closing_balance.closing_balance =0;
								save_device_config();
							}
							if(m_device_config.run_mode== FIX_MODE)
							{
								buy.StateBuy = WAIT_INSENCE;
								havemoney =1;
							}
							else
								buy.StateBuy = DONATE;
						}
						
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
							Displaypicture(Stop_Service);
							sprintf(s,"%s",m_device_config.phone.phone);
							LCD_background(color_white);
							TFT_DrawString_option(s,380, 265, color_red, true, 3);
//							TFT_putString((260-FontMakerGetWidth(s,&ufont_16))/2 +370,280,s,&font_16,color_red);							
//							LCD_background(color_white);
//							TFT_DrawString("STOP SERVICE", 300, 200, color_red);
							NV11_Disable(true);
						}
						if(buy.StateBuy==WAIT_INSENCE)
						{
							Displaypicture(Wait_insense);
							LCD_background(color_white);
							if(m_device_config.run_mode== SALES_MODE)
								sprintf(s,"%3d 000", m_device_config.closing_balance.closing_balance/1000);
							
							else
								sprintf(s,"%3d 000", m_lastest_note/1000);

							LCD_background(color_white);
							TFT_putString(400-FontMakerGetWidth(s,&uArial_32)/2,210,s,&Arial_32,color_red);
							NV11_Disable(true);
						}
						if(buy.StateBuy == HELLO_CUSTOM)
						{
							Displaypicture(Hello);
							nv11_enable(true);
							if(m_device_config.closing_balance.closing_balance)
							{
								show_time =10;
								current_display = DIS_SALE;
							}
						}
						if(buy.StateBuy ==INSENCE_EMPTY)
						{
							if(m_device_config.run_mode== SALES_MODE)
								show_time =15;
							else
								show_time =10;
							Displaypicture(Empty);
							buy.StateBuy = HELLO_CUSTOM;
							sprintf(s,"%s",m_device_config.phone.phone);
							LCD_background(color_white);
							TFT_DrawString_option(s,380, 385, color_red, true, 3);
							
							if(m_device_config.run_mode== SALES_MODE)
								sprintf(s,"%3d 000", m_device_config.closing_balance.closing_balance/1000);
							else
								sprintf(s,"%3d 000", m_lastest_note/1000);
							LCD_background(color_white);
							TFT_putString(400-FontMakerGetWidth(s,&uArial_32)/2,210,s,&Arial_32,color_red);		
							
							if(buy.TotalIsenseDroped !=IDM.NumberInsenseBuy)
							{
								reven.number = IDM.NumberInsenseBuy - buy.TotalIsenseDroped;
								reven.money =0;
								add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);
							}
							vdm_app_gsm_send_qrm_sale_fail_frame((m_lastest_note/1000)&0xFF, 0xFF, 0x01);
						}
						if(buy.StateBuy ==CELL_SUCCESS)
						{
							show_time=10;
							Displaypicture(Insense_out);
							if(m_device_config.run_mode== SALES_MODE)
								sprintf(s,"%3d 000", m_device_config.closing_balance.closing_balance/1000);
							else
								sprintf(s,"%3d 000", m_lastest_note/1000);
							LCD_background(color_white);
							TFT_putString(400-FontMakerGetWidth(s,&uArial_32)/2,210,s,&Arial_32,color_red);								
							
							if(m_device_config.run_mode== SALES_MODE)
								reven.number = quantity_buy;
							if(m_device_config.run_mode== FIX_MODE)
								reven.number = IDM.NumberInsenseBuy;
							reven.money =0;
							if(m_device_config.run_mode== SALES_MODE)
							{
								m_device_config.closing_balance.closing_balance -= quantity_buy * m_device_config.item[0].price;
								quantity_buy =0;								
								save_device_config();
							}
							add_revenue_day(currentTime.day, currentTime.month, currentTime.year, reven);							
							buy.StateBuy = HELLO_CUSTOM;
//							if(m_device_config.closing_balance.closing_balance
//							m_device_config.closing_balance.closing_balance += m_lastest_note;
//							save_device_config();
							vdm_app_gsm_send_released_item_frame((m_lastest_note/1000)&0xFF, 0x01);
						}
						if(buy.StateBuy ==DONATE)
						{
							show_time=10;
							Displaypicture(Thanks);
							buy.StateBuy = HELLO_CUSTOM;
							LCD_background(color_white);
							sprintf(s,"%3d 000", m_lastest_note/1000);
							TFT_putString(400-FontMakerGetWidth(s,&uArial_32)/2,210,s,&Arial_32,color_red);	
							vdm_app_gsm_send_released_item_frame((m_lastest_note/1000)&0xFF, 0x01);
						}
					}
				}
				break;
			case DIS_Password:
//				char str[7];
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
//							Displaypicture(Setting_Sell);
							break;
						}
						if(temp_menu_char[0] == pass_tech[0] && temp_menu_char[1] == pass_tech[1] && temp_menu_char[2] == pass_tech[2]\
							&& temp_menu_char[3] == pass_tech[3] && temp_menu_char[4] == pass_tech[4] && temp_menu_char[5] == pass_tech[5])
						{
							current_display = DIS_SETUP_TECH;
							Buzz_On(100);							
							break;
						}
						Buzz_On(500);
						sprintf(str,"------");
					}
					
					for(int n=0; n<6; n++)
					{
						if(n<point) str[n] = '*';
						else	str[n] ='-';
					}
					str[6]=0;
					LCD_background(color_blue);
					TFT_DrawString_option(str, 130, 220, color_white,false,4);	
				}
				if(key_pressed==CANCEL)
				{
//					current_display = DIS_Home;
					point =0;
					sprintf(str,"------");
					LCD_background(color_blue);
					TFT_DrawString_option(str, 130, 220, color_white,false,4);					
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
					if(point>13)	point =0;
				}
				if(key_pressed ==UP)
				{
					if(point>0)	point--;
					else	point =13;
				}
				if(key_pressed == CANCEL)
				{
					current_display = DIS_Password;
					point=0;
				}

				if(get_touch(TOUCH_DOWN, 55, 70, 705, 240)== true)
				{
					struct _ts_event touch_mouse = get_mouse();
					if((touch_mouse.x1-55)%235 <230 && (touch_mouse.y1-70)%40 <35)
						point = ((touch_mouse.x1-55)/235)*6 + (touch_mouse.y1-70)/40;
				}
					
				if(old_point!= point)
				{
					if(old_point<255)
					{						
						drawRoundRect(55+(old_point/6)*235,70+(old_point%6)*40,220, 30,10, color_black, false);
						drawRoundRect(56+(old_point/6)*235,71+(old_point%6)*40,218, 28,8, color_black, false);
					}
					drawRoundRect(55+(point/6)*235,70+(point%6)*40,220, 30,10, color_green, false);
					drawRoundRect(56+(point/6)*235,71+(point%6)*40,218, 28,8, color_green, false);
					old_point =point;
					Buzz_On(10);
				}
				if(key_pressed == ENTER || get_touch(TOUCH_UP, 55, 70, 705, 240)== true)
				{
					sub1_point =0;
					switch(point)
					{
//						case 2:
//							current_display = DIS_ERROR_INFO;
//							sub1_point =0;
//							break;
						case 3:
							current_display = DIS_DEL_CELL_ERROR;
							break;
						case 5:
							current_display = DIS_NUMBER_CELL_ERROR;
							break;
						case 6:////////////////
							current_display = DIS_CELL_EACH_TIME;
							break;						
						case 7:
							current_display = DIS_TIME_SWAP_RUN;
							break;
						case 8:
							current_display = DIS_NUMBER_CELL_SWAP;
							break;
						case 9:
							current_display = DIS_CELL_MORE;
							break;
						case 10:	//////////////////
							current_display = DIS_ACCEPT_DENO;
							sub1_point=0;
							break;
						case 11:
							current_display = DIS_PRICE_SET;
							break;						
						case 12:	////////////
							current_display = DIS_ONOFF_HUMIDITY;
							break;
						case 13:
							current_display = DIS_HUMIDITY_SET;
							break;						
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
			case DIS_TOTAL:
				if(key_pressed== CANCEL)
					current_display = DIS_SETUP_TECH;				
				break;
			case DIS_DAY:
				if(key_pressed==CANCEL)
					current_display = DIS_SETUP_TECH;//DIS_TOTAL_MONEY;
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
//					TimeSetting = currentTime;
					sprintf(s,"%02d/%02d/%4d",TimeSetting.day, TimeSetting.month,TimeSetting.year);
//					FontMakerPutString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,GREEN, BLUE);
					drawRoundRect(100,180,300, 30,10, color_blue, true);
					LCD_background(color_blue);
//					TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,GREEN);
					TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,color_green);
					
					reven = get_revenue_day(TimeSetting.day,TimeSetting.month, TimeSetting.year);
					sprintf(s,"%7d000", reven.money);
					TFT_DrawString_option(s,150, 270, color_green, false, 3);
					sprintf(s,"%7d", reven.number);
					TFT_DrawString_option(s,150, 210, color_green, false, 3);		
					
//					sprintf(s,"%02d/%02d/%4d",TimeSetting.day,TimeSetting.month, TimeSetting.year);
//					GLcd_DrawString(s, 35, 10,WHITE);
//					GLcd_FillRect(60,30,67,25,BLACK);
//					reven = get_revenue_day(TimeSetting.day,TimeSetting.month, TimeSetting.year);
//					sprintf(s,"%d000", reven.money);
//					GLcd_DrawString(s, 60, 45,WHITE);
//					sprintf(s,"%d", reven.number);
//					GLcd_DrawString(s, 60, 30,WHITE);
				}
				break;
			case DIS_MONTH:
				if(key_pressed==CANCEL)
					current_display = DIS_SETUP_TECH;//DIS_TOTAL_MONEY;
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
					sprintf(s,"%02d/%4d", TimeSetting.month,TimeSetting.year);
					drawRoundRect(100,180,300, 30,10, color_blue, true);
					LCD_background(color_blue);
					TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,color_green_bright);
					LCD_background(color_blue);				
					reven = get_revenue_month(TimeSetting.month, TimeSetting.year);
					sprintf(s,"%7d000", reven.money);
					TFT_DrawString_option(s,150, 270, color_green, false, 3);
					sprintf(s,"%7d", reven.number);
					TFT_DrawString_option(s,150, 210, color_green, false, 3);						
				}
				break;
			case DIS_YEAR:
				if(key_pressed==CANCEL)
					current_display = DIS_SETUP_TECH;//DIS_TOTAL_MONEY;
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
					sprintf(s,"%4d", TimeSetting.year);
					drawRoundRect(100,180,300, 30,10, color_blue, true);
					LCD_background(color_blue);
					TFT_putString((450-FontMakerGetWidth(s,&ufont_16))/2 +10,180,s,&font_16,color_green_bright);
					LCD_background(color_blue);				
					reven = get_revenue_year(TimeSetting.year);
					sprintf(s,"%7d000", reven.money);
					TFT_DrawString_option(s,150, 270, color_green, false, 3);
					sprintf(s,"%7d", reven.number);
					TFT_DrawString_option(s,150, 210, color_green, false, 3);	
				}				
				break;
//			case DIS_TOTAL:
//				if(key_pressed==CANCEL)
//					current_display = DIS_TOTAL_MONEY;			
//				break;
			case DIS_CELL_EACH_TIME:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%100) *10 + (key_pressed-'0');
					if(temp_data_Menu >100)	temp_data_Menu =temp_data_Menu%100;
					sprintf(s, "%3d ", temp_data_Menu);
					LCD_background(color_blue);
					TFT_DrawString_option(s,200, 200, COLOR_CHANGE, false, 3);					
				}
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
				if(key_pressed=='8' || key_pressed=='0' || get_touch(TOUCH_UP, 120,200, 150,80)==true)
				{
					if(sub1_point)
						sub1_point =0;	// bat
					else
						sub1_point=1;		// tat
					
					if(sub1_point==1)
					{
					drawRoundRect(150,220,100, 30,15, color_gray, true);
					drawRoundRect(152,222,30, 26,12, color_white, true);
					}
					else
					{
						drawRoundRect(150,220,100, 30,15, color_green, true);
						drawRoundRect(218,222,30, 26,12, color_white, true);					
					}					
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				if(key_pressed==ENTER)
				{
					if(sub1_point)	IDM.EnableHumidity = 0;
					else	IDM.EnableHumidity = 1;
					Write_config();
					current_display = DIS_Setup;
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
				}

				break;
			case DIS_HUMIDITY_SET:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%100) *10 + (key_pressed-'0');
					if(temp_data_Menu >100)	temp_data_Menu =temp_data_Menu%100;		// Max
					sprintf(s, "%3d%% ", temp_data_Menu);
					LCD_background(color_blue);
					TFT_DrawString_option(s,200, 200, COLOR_CHANGE, false, 3);	
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
			case DIS_NUMBER_CELL_ERROR:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%100) *10 + (key_pressed-'0');
					if(temp_data_Menu >100)	temp_data_Menu =temp_data_Menu%100;		// Max
					sprintf(s, "%3d ", temp_data_Menu);
					LCD_background(color_blue);
					TFT_DrawString_option(s,200, 200, COLOR_CHANGE, false, 3);	
				}				
				if(key_pressed==ENTER)
				{
					IDM.retryCellEmpty = temp_data_Menu;
					IDM.currentRetryCellEmpty = temp_data_Menu;
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
					if(point==0)	point = m_setup_req.NumberOfChannels -1;
					else	point--;
				}
				if(key_pressed == DOWN)
				{
					point++;
					if(point >=m_setup_req.NumberOfChannels)	point =0;
				}
				if(key_pressed== ENTER)
				{
					if((temp_data_Menu>>point)&0x01) temp_data_Menu&=~(1<<point);
					else	temp_data_Menu|=(1<<point);
				}
				
//				if(get_touch(TOUCH_DOWN, 15, 170, 460, 250)== true)
//				{
//					struct _ts_event touch_mouse = get_mouse();
//					if((touch_mouse.x1-15)%230 <220 && (touch_mouse.y1-170)%50 <40)
//						point = ((touch_mouse.x1-10)/250)*5 + (touch_mouse.y1-170)/50;
//				}
				if(get_touch(TOUCH_UP, 15, 170, 460, 250)== true)
				{
					struct _ts_event touch_mouse = get_mouse();
					if((touch_mouse.x1-15)%230 <220 && (touch_mouse.y1-170)%50 <40)
					{
						point = ((touch_mouse.x1-10)/230)*5 + (touch_mouse.y1-170)/50;
						if(point < m_setup_req.NumberOfChannels)
						{
							if((temp_data_Menu>>point)&0x01) temp_data_Menu&=~(1<<point);
							else	temp_data_Menu|=(1<<point);
						}
						Buzz_On(10);
					}
				}			
					
				if(old_point!= point && point < m_setup_req.NumberOfChannels)
				{
					drawRoundRect(12+(old_point/5)*230,170+(old_point%5)*50,200, 40,10, color_blue, false);	
					drawRoundRect(12+(point/5)*230,170+(point%5)*50,200, 40,10, color_green, false);	
					old_point = point;
				}
				if(m_device_config.accept_cash_max != temp_data_Menu)
				{
					m_device_config.accept_cash_max = temp_data_Menu;
					Buzz_On(100);
					save_device_config();
					NV11_SetAcceptNote(temp_data_Menu, 0);					
					for(int n=0; n< m_setup_req.NumberOfChannels; n++)
					{
						sprintf(s,"%3d.000 %s",m_setup_req.ChannelData[n].value/1000, m_setup_req.ChannelData[n].cc);
						
						if((temp_data_Menu>>n)&0x01)
						{
							TFT_DrawString_option(s,10+n/5*250, (n%5)*50 +170, color_green, true, 2);
						}
						else
						{	
							TFT_DrawString_option(s,10+n/5*250, (n%5)*50 +170, color_gray, true, 2);					
						}
					}				
				}
				break;
			case DIS_TIME_SWAP_RUN:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%100) *10 + (key_pressed-'0');
					if(temp_data_Menu >100)	temp_data_Menu =temp_data_Menu%100;		// Max
					sprintf(s, "%3ds ", temp_data_Menu);
					LCD_background(color_blue);
					TFT_DrawString_option(s,200, 200, COLOR_CHANGE, false, 3);					
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
					LCD_background(color_blue);
					TFT_DrawString_option(s,200, 200, COLOR_CHANGE, false, 3);
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

				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_data_Menu = (temp_data_Menu%1000) *10 + (key_pressed-'0');
					if(temp_data_Menu >1000)	temp_data_Menu =temp_data_Menu%1000;		// Max
					sprintf(s, "%4d ", temp_data_Menu);
					LCD_background(color_blue);
					TFT_DrawString_option(s,200, 200, COLOR_CHANGE, false, 3);
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
					if(m_device_config.sensor_error.error)
					{
						m_device_config.sensor_error.error =0;					
						IDM.currentRetryCellEmpty = IDM.retryCellEmpty;
						IDM.currentNumberBuyMore = IDM.NumberBuyMore;
						
						Write_config();
						buy.StateBuy = HELLO_CUSTOM;
		//				IDM_state = 1;
						Buzz_On(100);
						printf("Retry: %d/%d\r\n Buy_state: %d\r\n", IDM.currentRetryCellEmpty, IDM.retryCellEmpty, buy.StateBuy);
						if(m_device_config.closing_balance.closing_balance)
						{
							m_device_config.closing_balance.closing_balance =0;
							save_device_config();
						}
						show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
					}
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;
			case DIS_SETUP_TECH:
				if(key_pressed == UP)
				{
					if(point==0)	point =20;
					else	point --;
					if(point==13)	point = 10;
					if(point==6)	point =3;
				}
				if(key_pressed == DOWN)
				{
					point ++;
					if(point>20)	point=0;
					if(point==4)	point = 7;
					if(point==11)	point = 14;
				}
				
				if(get_touch(TOUCH_DOWN, 55, 70, 705, 280)== true)
				{
					struct _ts_event touch_mouse = get_mouse();
					if((touch_mouse.x1-55)%235 <230 && (touch_mouse.y1-70)%40 <35)
						point = ((touch_mouse.x1-55)/235)*7 + (touch_mouse.y1-70)/40;
				}
					
				if(old_point!= point)
				{
					if(old_point<255)
					{	
						drawRoundRect(55+(old_point/7)*235,70+(old_point%7)*40,220, 30,10, color_black, false);
						drawRoundRect(56+(old_point/7)*235,71+(old_point%7)*40,218, 28,8, color_black, false);
					}
					drawRoundRect(55+(point/7)*235,70+(point%7)*40,220, 30,10, color_green, false);
					drawRoundRect(56+(point/7)*235,71+(point%7)*40,218, 28,8, color_green, false);
					old_point =point;
					Buzz_On(10);
#ifdef debug_display_info
					sprintf(s,"P: %2d", point);
					TFT_DrawString(s, 20, 430, color_green);	
#endif						
				}	
				
				if(key_pressed == ENTER || get_touch(TOUCH_UP, 55, 70, 705, 280)== true)
				{
					sub1_point =0;
					switch(point)
					{
						case 0:
							current_display = DIS_TOTAL;
							Buzz_On(100);
							break;
						case 1:
							current_display = DIS_DAY;
							Buzz_On(100);
							break;
						case 2:
							current_display = DIS_MONTH;
							Buzz_On(100);
							break;		
						case 3:
							current_display = DIS_YEAR;
							Buzz_On(100);
							break;
						case 7:
							temp_data_Menu =  set_mode(MERIT_MODE);
							break;
						case 8:
							temp_data_Menu =  set_mode(FIX_MODE);										
							break;
						case 9:
							temp_data_Menu =  set_mode(SALES_MODE);
							break;
						case 10:
							current_display = DIS_MOTOR_PROTECT;
							Buzz_On(100);	
							break;						
						case 17:
							current_display = DIS_SET_ID_MACHINE;
							Buzz_On(100);						
							break;
						case 18:
							current_display = DIS_SET_IP;
							Buzz_On(100);	
							break;
						case 14:
							current_display = DIS_PHONE_NUMBER;
							Buzz_On(100);	
							break;	
						case 20:
							current_display = DIS_VERSION_INFO;
							Buzz_On(100);	
							break;		
					}
					if(point ==7 || point ==8 || point ==9)
					{
						if(m_device_config.run_mode!=temp_data_Menu)
						{
							m_device_config.run_mode=temp_data_Menu;						
							if(m_device_config.run_mode == MERIT_MODE)
								draw_ON_OFF(450, 75, true);
							else
								draw_ON_OFF(450, 75, false);
							
							if(m_device_config.run_mode == FIX_MODE)
								draw_ON_OFF(450, 115, true);
							else
								draw_ON_OFF(450, 115, false);
							
							if(m_device_config.run_mode >= SALES_MODE)
								draw_ON_OFF(450, 155, true);
							else
								draw_ON_OFF(450, 155, false);
							save_device_config();
						}
					}						
						
				}
				if(key_pressed == CANCEL)	current_display = DIS_Password;
				break;
			case DIS_SET_IP:
				if(key_pressed>='0' && key_pressed<='9')
				{
					if(sub2_point/3 <4)
					{
						temp_menu_int[sub2_point/3] = (temp_menu_int[sub2_point/3]%100)*10 + key_pressed-'0';
						if(temp_menu_int[sub2_point/3]%1000>255)	temp_menu_int[sub2_point/3]%=100;
					}
					else
						temp_data_Menu = (temp_data_Menu%1000)*10 + key_pressed-'0';
					sub2_point++;
					if(sub2_point>15)	sub2_point=0;
				}
				if(old_point!= sub2_point)
				{
					old_point= sub2_point;
					sprintf(s,"IP:%03d.%03d.%03d.%03d", temp_menu_int[0], temp_menu_int[1], temp_menu_int[2], temp_menu_int[3]);
					LCD_background(color_blue);
					TFT_DrawString_option(s,20, 200, COLOR_CHANGE, false, 3);
				
					sprintf(s,"PORT: %04d", temp_data_Menu);
					TFT_DrawString_option(s,20, 250, COLOR_CHANGE, false, 3);	
					
					if((sub2_point/3)<4)
					{
						drawRoundRect(92+ (sub2_point/3)*96,205,71, 40,4, color_green, false);						
					}
					else
						drawRoundRect(116+ 40,255,104, 40,4, COLOR_CHANGE, false);	
				}
				if(key_pressed == ENTER)
				{
					m_device_config.server.addr = (uint32_t)temp_menu_int[0];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_int[1];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_int[2];
					m_device_config.server.addr = (m_device_config.server.addr<<8) + (uint32_t)temp_menu_int[3];
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
//					GLcd_DrawLine(17, 30, 110, 30, BLACK);
//					GLcd_DrawLine(23+ (sub2_point)*6, 30, 27+ (sub2_point)*6, 30, WHITE);	
					drawRoundRect(116+ old_point*24,205,24, 40,4, color_blue, false);
					sprintf(s,"ID: %s ", temp_menu_char);
					LCD_background(color_blue);
					TFT_DrawString_option(s,20, 200, COLOR_CHANGE, false, 3);
					drawRoundRect(116+ sub2_point*24,205,24, 40,4, COLOR_CHANGE, false);									
					old_point= sub2_point;
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
				
			case DIS_PHONE_NUMBER:
				if(key_pressed>='0' && key_pressed<='9')
				{
					temp_menu_char[sub2_point] = key_pressed;
					sub2_point++;
					if(sub2_point>9)	sub2_point=0;
				}
				if(old_point!= sub2_point)
				{
//					GLcd_DrawLine(17, 30, 110, 30, BLACK);
//					GLcd_DrawLine(23+ (sub2_point)*6, 30, 27+ (sub2_point)*6, 30, WHITE);	
					drawRoundRect(116+ old_point*24,205,24, 40,4, color_blue, false);
					sprintf(s,"%s ", temp_menu_char);
					LCD_background(color_blue);
					TFT_DrawString_option(s,118, 200, COLOR_CHANGE, false, 3);
					drawRoundRect(116+ sub2_point*24,205,24, 40,4, COLOR_CHANGE, false);									
					old_point= sub2_point;
				}
				if(key_pressed == ENTER)
				{
					for(int n=0; n<10; n++)
						m_device_config.phone.phone[n] = temp_menu_char[n];
					save_device_config();
					show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
//					vdm_app_gsm_set_device_id(m_device_config.peripheral.machine_id);
				}
				if(key_pressed == CANCEL)	current_display = DIS_SETUP_TECH;
				break;				
				
				case DIS_PRICE_SET:
					if(key_pressed>='0' && key_pressed<='9')
					{
						temp_data_Menu = (temp_data_Menu%100000) *10 + (key_pressed-'0');
						if(temp_data_Menu >500000)	temp_data_Menu =temp_data_Menu%100000;		// Max
						if(temp_data_Menu>1000)
							sprintf(s, "%3d %03d (VND)", temp_data_Menu/1000, temp_data_Menu%1000);
						else	
							sprintf(s, "%7d (VND)", temp_data_Menu);
						LCD_background(color_blue);
//						TFT_putString(450-FontMakerGetWidth(s,&uArial_32),200,s,&Arial_32,COLOR_CHANGE);
						TFT_DrawString_option(s,100, 200, COLOR_CHANGE, false, 3);
					}
					if(key_pressed ==ENTER)
					{
						m_device_config.item[0].price = temp_data_Menu;
						save_device_config();
						show_Messenger(vdm_language_get_text(VDM_LANG_FINISH), 2);
					}
					if(key_pressed== CANCEL)
						current_display = DIS_Setup;					
					break;
				case DIS_VERSION_INFO:
					if(key_pressed == CANCEL)	current_display = DIS_SETUP_TECH;
					break;
				case DIS_MOTOR_PROTECT:
					if(key_pressed == CANCEL)
					{
						current_display = DIS_SETUP_TECH;
						IDM_Status.Motor_conveyer.isRun=0;
						IDM_Status.Motor_swap.isRun=0;
					}
					if(key_pressed == ENTER)
					{
						if(ischange==0)
						{
							ischange =1;
							if(sub1_point<2)
							{
								LCD_background(color_white);
								sprintf(s, "%4d", temp_menu_int[sub1_point]);
								TFT_DrawString_option(s,170+sub1_point*150, 200, COLOR_CHANGE, false, 3);
								LCD_background(color_blue);
							}
							if(sub1_point ==2)
							{
								if(IDM_Status.Motor_conveyer.isRun==1)	IDM_Status.Motor_conveyer.isRun=0;
								else	IDM_Status.Motor_conveyer.isRun=1;
							}
							if(sub1_point==3)
							{
								if(IDM_Status.Motor_swap.isRun==1) IDM_Status.Motor_swap.isRun=0;
								else IDM_Status.Motor_swap.isRun=1;
							}
							if(sub1_point==4)
							{
								if(m_device_config.motor.trip_over)	m_device_config.motor.trip_over=0;
								else	m_device_config.motor.trip_over=1;
								sprintf(s, "%s", (m_device_config.motor.trip_over==0) ? " OFF": "  ON");
								TFT_DrawString_option(s,170, 300, color_green_bright, false, 3);
								save_device_config();
							}
						}
						else
						{
							ischange =0;
							if(sub1_point<2)
							{
								sprintf(s, "%4d", temp_menu_int[sub1_point]);
								TFT_DrawString_option(s,170+sub1_point*150, 200, COLOR_CHANGE, false, 3);
								drawRoundRect(165+sub1_point*150,205,100, 45,3, color_green_bright, false);
							}
							
							m_device_config.motor.max_current_conveyer = temp_menu_int[0];
							m_device_config.motor.max_current_swap = temp_menu_int[1];
							save_device_config();
						}
					}
					if(ischange)
					{
						if(key_pressed>='0' && key_pressed<='9' && sub1_point<2)
						{
							temp_menu_int[sub1_point] = temp_menu_int[sub1_point]*10 + (key_pressed-'0');
							if(temp_menu_int[sub1_point]>5000)	temp_menu_int[sub1_point] %=1000;
							LCD_background(color_white);
							sprintf(s, "%4d", temp_menu_int[sub1_point]);
							TFT_DrawString_option(s,170+sub1_point*150, 200, COLOR_CHANGE, false, 3);
							LCD_background(color_blue);
//							drawRoundRect(165+sub1_point*150,205,100, 45,3, color_white, false);
						}
						if(sub1_point>1 && (key_pressed==UP || key_pressed==DOWN))
						{
							ischange =0;
							IDM_Status.Motor_swap.isRun=0;
							IDM_Status.Motor_conveyer.isRun=0;
							sub1_point++;
							if(sub1_point>4)	sub1_point =0;							
						}
					}
					else
					{
						if(key_pressed==UP || key_pressed==DOWN)
						{
							sub1_point++;
							if(sub1_point>4)	sub1_point =0;
						}
					}
					if(sub1_point != old_point)
					{
						drawRoundRect(165+(old_point%2)*150,205+(old_point/2)*50,100, 45,3, color_blue, false);
						old_point = sub1_point;
						drawRoundRect(165+(sub1_point%2)*150,205+(sub1_point/2)*50,100, 45,3, color_green_bright, false);
					}
					if(isSecond_display)
					{
						isSecond_display =0;
						sprintf(s, "%4d", I_MOTOR);
						TFT_DrawString_option(s,170, 350, COLOR_MENU, false, 3);
						sprintf(s, "%4d", I_SWAP_M);
						TFT_DrawString_option(s,170+150, 350, COLOR_MENU, false, 3);
						
						sprintf(s, "%s", (IDM_Status.Motor_conveyer.isRun==0) ? " OFF": "  ON");
						TFT_DrawString_option(s,170, 250, color_green_bright, false, 3);
						sprintf(s, "%s", (IDM_Status.Motor_swap.isRun==0) ? " OFF": "  ON");
						TFT_DrawString_option(s,170+150, 250, color_green_bright, false, 3);
					}
					break;					
			default:
				current_display = DIS_Home;
				break;
		}
//		if(key_pressed!=255) GLcd_Flush();		
	}
}

