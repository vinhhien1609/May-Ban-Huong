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
#define KEY_HOLE_TIME 20
unsigned char key_map[16]= {'0','1','2','3','4','5','6','7','8','9','*','#'};
unsigned char key_count=0, old_key=255, pressed_old_key=255, point=0, old_point=0;


Display_Typedef current_display=Home, old_display=255;
rtc_date_time_t TimeSetting;
char str[30];


typedef uint16_t char16_t;

extern GSM_State GSM;
extern uint8_t isSecond_display;
extern	rtc_date_time_t currentTime;
extern IDM_PARA IDM;
extern unsigned char havemoney;

static uint8_t RTC_WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay);
/* Private define ------------------------------------------------------------*/

void draw_signal_gsm(void);

static void small_delay()		// not importance function
{
	volatile uint32_t i = 0x07F;
	while (i--);
}

unsigned char scan_BT(void)
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
		if(key_count<KEY_HOLE_TIME)
			key_count ++;	
		else
		{
			if(key !=pressed_old_key)
			{
				pressed_old_key = key;
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
				HAL_Delay(10);
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
				return key_map[key];// chi gui 1 lan cho toi khi nha
			}
//			return key_map[key];		//
		}
	}
	else
	{
		pressed_old_key =255;
		key_count =0;
		old_key = key;
	}
	return 255;
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
		point =0;
		old_point =0;
		GLcd_ClearScreen(BLACK);
		switch(current_display)
		{
			case Home:
//				GLcd_SetFont(g_vnfont_8x15);
				//sprintf(s, "NH\x1EACP M\x1EACT KH\x1EA8U");
//				GLcd_DrawStringUni(VDM_LANG_XIN_KINH_CHAO, 25, 20, WHITE);
//				GLcd_DrawStringUni(VDM_LANG_QUY_KHACH, 32, 38, WHITE);
				GLcd_DrawStringDef("Xin Kính Chào\n      Quý Khách",20, 20, WHITE);
				
				break;
			case Password:
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_PASSWORD),20, 20, WHITE);
				GLcd_DrawString("------", 40, 40, WHITE);
				break;
			case Setup:
				GLcd_DrawBitmap(img_arrow_bmp, 0, 15);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_MOUNT),10, 10, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_TOTAL_SETTING),10, 25, WHITE);
				GLcd_DrawStringUni(vdm_language_get_text(VDM_LANG_ERROR_INFO),10, 40, WHITE);
				break;
			case TimeSetup:
				GLcd_DrawString("Time Setting", 18, 0, WHITE);
				TimeSetting = currentTime;
				sprintf(s,"%s %02d/%02d/%d %02d:%02d:%02d ", day[TimeSetting.weekday]\
				, TimeSetting.day, TimeSetting.month,TimeSetting.year, TimeSetting.hour, TimeSetting.minute, TimeSetting.second);
				GLcd_DrawString(s, 0, 24, WHITE);
				sprintf(str,"%02d%02d%d%02d%02d%02d", TimeSetting.day, TimeSetting.month,TimeSetting.year, TimeSetting.hour, TimeSetting.minute, TimeSetting.second);
				GLcd_DrawLine(18,33,23,33,WHITE);
				break;
			default:
				current_display = Home;
				break;
		}
		GLcd_Flush();		
	}
	else
	{
		key_pressed = scan_BT();
		if(key_pressed!=255)
			isSecond_display =0;
		if(key_pressed =='*')
			current_display++;
		if(isSecond_display>30)
		{
			isSecond_display =0;
			current_display = Home;
		}
		switch(current_display)
		{
			case Home:
				if(key_pressed=='#')
					havemoney =1;				
				if(isSecond_display)
				{
					isSecond_display =0;
					if(currentTime.second%2)
						sprintf(s,"%s %02d/%02d %d:%02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
					else
						sprintf(s,"%s %02d/%02d %d %02d ", day[currentTime.weekday], currentTime.day, currentTime.month, currentTime.hour, currentTime.minute);
					GLcd_DrawString(s, 18, 0, WHITE);
	//				sprintf(s,"KEY %c, %d, %d %d", key_pressed, t, IDM_state, buy.TotalIsenseDroped);
	//				GLcd_DrawString(s, 0, 20, WHITE);
					//draw CSQ
					draw_signal_gsm();
						sprintf(s,"%02d%%", (int)IDM.Humidity);
						GLcd_DrawString(s, 100, 0, WHITE);
					GLcd_Flush();
				}
				break;
			case Password:
				break;
			case Setup:
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
				if(old_point!= point)
				{
					if(point%3!=1)	GLcd_ClearScreen(BLACK);
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
			case TimeSetup:
//				if(key_pressed=='#' && strlen(str)>0)
//				{
//					str[strlen(str)-1] =0;
//				}
//				if(key_pressed!=255 && key_pressed!='#' && strlen(str)<19)
//				{
//					str[strlen(str)] = key_pressed;
//				}
			
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
			default:
				current_display = Home;
				break;
		}
		if(key_pressed!=255) GLcd_Flush();		
	}
}


/* Private function prototypes -----------------------------------------------*/


