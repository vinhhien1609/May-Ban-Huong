/*********************************************************************************************************
*
* File                : RTC_Time.h
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

#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
//#include "stm32f1xx_hal_rtc.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t weekday; // 0-6, 0 = sunday, 1 = monday, ... 6 = sat
} rtc_date_time_t;

/* Private function prototypes -----------------------------------------------*/
void RTC_Init(void);
rtc_date_time_t getRTC(void);
void setRTC(rtc_date_time_t time);
void get_time(rtc_date_time_t *rtime);

/* Private variables ---------------------------------------------------------*/

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
