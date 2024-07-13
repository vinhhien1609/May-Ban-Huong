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

#define KEY_HOLE_TIME 50
#define KEY_REPEAT_TIME 1500
#define ENTER	'*'
#define CANCEL '#'

unsigned char key_map[16]= {'0','1','2','3','4','5','6','7','8','9','*','#'}, key_tick=0;
unsigned char old_key=255, point=0, old_point=0, sub1_point, sub2_point, Old_cell_state;
unsigned int key_count=0;


Display_Typedef current_display=DIS_Home, old_display=255;
rtc_date_time_t TimeSetting;
char str[30];


typedef uint16_t char16_t;

uint16_t temp_data_Menu;

extern GSM_State GSM;
extern uint8_t isSecond_display;
extern	rtc_date_time_t currentTime;
extern IDM_PARA IDM;
extern IDM_HARDWARE IDM_Status;
extern IDM_ERROR IDM_Errors;
extern BUY_PARA buy;
extern unsigned char havemoney;
extern SSP6_SETUP_REQUEST_DATA m_setup_req;

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
//	key_tick = 255;
}
//draw signal GSM
void draw_signal_gsm(void)
{
	uint8_t t=0;
	while(t<14)
	{
		GLcd_DrawLine(t,0, t, 6, BLACK);
		t++;
	}
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
	char day[7][3] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
	unsigned char 	key_pressed=255;
//	t = HAL_GPIO_ReadPin(Empty_GPIO_Port, Empty_Pin);	
	if(current_display!= old_display)		//init etc display
	{
		old_display = current_display;
//		point =sub1_point;
//		old_point =0;
		GLcd_ClearScreen(BLACK);
		switch(current_display)
		{
			case DIS_Home:
//				GLcd_SetFont(g_vnfont_8x15);
				//sprintf(s, "NH\x1EACP M\x1EACT KH\x1EA8U");
//				GLcd_DrawStringUni(VDM_LANG_XIN_KINH_CHAO, 25, 20, WHITE);
//				GLcd_DrawStringUni(VDM_LANG_QUY_KHACH, 32, 38, WHITE);
				GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);
				break;
			case DIS_Password:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_PASSWORD),20, 20, WHITE);
				GLcd_DrawString("------", 40, 40, WHITE);
				break;
			case DIS_Setup:
					GLcd_DrawBitmap(img_arrow_bmp, 0, (point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((point/3)*3 +n >9)	break;
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
			case DIS_TOTAL_MONEY:
				GLcd_DrawBitmap(img_arrow_bmp, 0, 15);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DAY),10, 10, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_MONTH),10, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_YEAR),10, 40, WHITE);
				break;
			case DIS_DAY:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);
				break;
			case DIS_MONTH:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);				
				break;
			case DIS_YEAR:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_NUMBER),0, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MONEY),0, 40, WHITE);				
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
				for(int n=0; n< m_setup_req.NumberOfChannels; n++)
				{
					sprintf(s,"%3d.000 %s",m_setup_req.ChannelData[n].value/1000, m_setup_req.ChannelData[n].cc);
//					sprintf(s,"IN: %d -",m_setup_req.NumberOfChannels);//, m_setup_req.ChannelData[n].cc);
					GLcd_DrawString(s,10, n*10 +10,WHITE);
					if((n*10 +10) >54)	break;
				}
				break;
			case DIS_TIME_RUN:		// thoi gian cap huong
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TIME_RUN_INSENSE),10, 10, WHITE);
				sprintf(s, "%ds", IDM.TimeConveyerRun);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.TimeConveyerRun;
				break;
			case DIS_NUMBER_CELL:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_INSENSE_CELL),10, 10, WHITE);
				sprintf(s, "%d", IDM.TotalInsenseBuy);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.TotalInsenseBuy;
				break;
			case DIS_CELL_MORE:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_INSENSE_CELL_MORE),10, 10, WHITE);
				sprintf(s, "%d", IDM.NumberBuyMore);
				GLcd_DrawString(s, 50, 30, WHITE);
				temp_data_Menu = IDM.NumberBuyMore;
				break;
			case DIS_DEL_ERROR:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_DELETE_ERROR_CELL),10, 10, WHITE);
				break;
			default:
				current_display = DIS_Home;
				break;
		}
		GLcd_Flush();		
	}
	
	else
	{
		key_pressed = scan_BT();
		if(key_pressed!=255)
			isSecond_display =0;
//		if(key_pressed =='*')
//			current_display++;
		if(isSecond_display>30)
		{
			isSecond_display =0;
			current_display = DIS_Home;
		}
		switch(current_display)
		{
			case DIS_Home:
				if(key_pressed=='#')
					havemoney =1;
				if(key_pressed=='*')
					current_display = DIS_Setup;
				if(isSecond_display)
				{
					isSecond_display =0;
					sprintf(s,"%s %02d/%02d %2d  %02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
					GLcd_DrawString(s, 16, 0, WHITE);
					if(currentTime.second%2)
						GLcd_DrawString(":", 78, 0, WHITE);
					if(Old_cell_state !=buy.StateBuy && currentTime.second%3==0)
					{
						Old_cell_state = buy.StateBuy;
						if(buy.StateBuy==CELLING)
						{
							GLcd_FillRect(0, 15, 127, 48, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_MONEY_RECEIVED),15, 10, WHITE);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_WAIT_RECEVER_INSENCE),15, 45, WHITE);
						}
						if(buy.StateBuy == CELL_WAIT)
						{
							GLcd_FillRect(0, 15, 127, 48, BLACK);
							GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);
						}
						if(buy.StateBuy ==CELL_EMPTY_INSENCE)
						{
							GLcd_FillRect(0, 15, 127, 48, BLACK);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_SORRY),40, 15, WHITE);
							GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_EMPTY_INSENCE),20, 30, WHITE);
							buy.StateBuy = CELL_WAIT;
						}
					}
	//				sprintf(s,"KEY %c, %d, %d %d", key_pressed, t, IDM_state, buy.TotalIsenseDroped);
	//				GLcd_DrawString(s, 0, 20, WHITE);
					//draw CSQ
					draw_signal_gsm();
						sprintf(s,"%2d%%", (int)IDM.Humidity);
						GLcd_DrawString(s, 105, 0, WHITE);
					GLcd_Flush();
				}
				break;
			case DIS_Password:
				break;
			case DIS_Setup:
				if(key_pressed=='8')
				{
					point++;
					if(point>9)	point =0;
				}
				if(key_pressed =='0')
				{
					if(point>0)	point--;
					else	point =9;
				}
				if(key_pressed == CANCEL)
				{
					current_display = DIS_Home;
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
							break;
						case 6:
							current_display = DIS_TIME_RUN;
							break;
						case 7:
							current_display = DIS_NUMBER_CELL;
							break;
						case 8:
							current_display = DIS_CELL_MORE;
							break;
						case 9:
							current_display = DIS_DEL_ERROR;
							break;						
					}
				}
				if(old_point!= point)
				{
					if(point%3!=1)	GLcd_FillRect(0, 15, 127, 48, BLACK);
					GLcd_DrawBitmap(img_del_arrow_bmp, 0, (old_point%3)*15 + 15);
					old_point =point;
					GLcd_DrawBitmap(img_arrow_bmp, 0, (point%3)*15 + 15);
					for(int n=0; n<3; n++)
					{
						if((point/3)*3 +n >9)	break;
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
				if(key_pressed=='#')
					setRTC(TimeSetting);
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
				if(old_point!= sub1_point)
				{
					old_point	= sub1_point;
					GLcd_FillRect(0, 15, 5, 45, BLACK);
					GLcd_DrawBitmap(img_arrow_bmp, 0, sub1_point*15 + 15);
				}
				break;
			case DIS_DAY:
				
				break;
			case DIS_MONTH:
			
				break;
			case DIS_YEAR:
				
				break;
			case DIS_TOTAL:
			
				break;
			case DIS_CELL_EACH_TIME:
				if(key_pressed=='0')	
					if(temp_data_Menu>0)	temp_data_Menu --;
				if(key_pressed=='8')
					if(temp_data_Menu<30)	temp_data_Menu ++;
				if(key_pressed=='0' || key_pressed=='8')
				{
					sprintf(s, "%2d ", temp_data_Menu);
					GLcd_DrawString(s, 64, 30, WHITE);
				}
				if(key_pressed == ENTER)
				{
					IDM.NumberInsenseBuy=temp_data_Menu;
					Write_config();
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;
				
			case DIS_ERROR_INFO:
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				if(key_pressed=='8')
				{
					sub1_point++;
					if(sub1_point>5)	sub1_point =0;
				}
				if(key_pressed =='0')
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
				if(key_pressed=='8') sub1_point =0;	// bat
				if(key_pressed=='0') sub1_point =1;	// tat
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				if(key_pressed==ENTER)
				{
					if(sub1_point)	IDM.EnableHumidity = 0;
					else	IDM.EnableHumidity = 1;
					Write_config();
				}
				if(key_pressed!=255)
				{
					GLcd_FillRect(0, 15, 5, 35, BLACK);
					GLcd_DrawBitmap(img_arrow_bmp, 0, sub1_point*15 + 15);					
				}
				break;
			case DIS_HUMIDITY_SET:
				if(key_pressed=='0')
					if(temp_data_Menu>5)	(temp_data_Menu%5==0) ? (temp_data_Menu -=5) : (temp_data_Menu -= temp_data_Menu%5);
				if(key_pressed=='8')
					if(temp_data_Menu<30)	(temp_data_Menu%5==0) ? (temp_data_Menu +=5) : (temp_data_Menu += 5 - temp_data_Menu%5);
				if(key_pressed=='0' || key_pressed=='8')
				{
					sprintf(s, "%2d%%", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}
				if(key_pressed==ENTER)
				{
					IDM.HumidityMAX = temp_data_Menu;
					Write_config();
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;
			case DIS_ACCEPT_DENO:
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;
			case DIS_TIME_RUN:
				if(key_pressed=='0')	
					if(temp_data_Menu>20)	(temp_data_Menu%10==0) ? (temp_data_Menu -=10) : (temp_data_Menu -= temp_data_Menu%10);;
				if(key_pressed=='8')
					if(temp_data_Menu<120)	(temp_data_Menu%10==0) ? (temp_data_Menu +=10) : (temp_data_Menu += 10 -temp_data_Menu%10);;
				if(key_pressed=='0' || key_pressed =='8')
				{
					sprintf(s, "%3ds", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}
				if(key_pressed ==ENTER)
				{
					IDM.TimeConveyerRun = temp_data_Menu;
					Write_config();
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				break;
			case DIS_NUMBER_CELL:
				if(key_pressed=='0')	
					if(temp_data_Menu>100)	(temp_data_Menu%50==0) ? (temp_data_Menu -=50) : (temp_data_Menu -= temp_data_Menu%50);
				if(key_pressed=='8')
					if(temp_data_Menu<1000)	(temp_data_Menu%50==0) ? (temp_data_Menu +=50) : (temp_data_Menu += 50 -temp_data_Menu%50);
				if(key_pressed=='0' || key_pressed =='8')
				{
					sprintf(s, "%4d ", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}				
				if(key_pressed ==ENTER)
				{
					IDM.TotalInsenseBuy = temp_data_Menu;				
					Write_config();
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				break;
			case DIS_CELL_MORE:
				if(key_pressed=='0')	
					if(temp_data_Menu>100)	(temp_data_Menu%50==0) ? (temp_data_Menu -=50) : (temp_data_Menu -= temp_data_Menu%50);
				if(key_pressed=='8')
					if(temp_data_Menu<1000)	(temp_data_Menu%50==0) ? (temp_data_Menu +=50) : (temp_data_Menu += 50 -temp_data_Menu%50);
				if(key_pressed=='0' || key_pressed =='8')
				{
					sprintf(s, "%4d ", temp_data_Menu);
					GLcd_DrawString(s, 50, 30, WHITE);
				}					
				if(key_pressed ==ENTER)
				{
					IDM.NumberBuyMore = temp_data_Menu;
					Write_config();
				}
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;
				break;
			case DIS_DEL_ERROR:
				if(key_pressed== CANCEL)
					current_display = DIS_Setup;				
				break;				
			default:
				current_display = DIS_Home;
				break;
		}
		if(key_pressed!=255) GLcd_Flush();		
	}
}


/* Private function prototypes -----------------------------------------------*/


