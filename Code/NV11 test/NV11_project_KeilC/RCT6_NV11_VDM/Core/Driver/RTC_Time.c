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
#include "stm32f10x.h"
#include "RTC_Time.h"

/* Private define ------------------------------------------------------------*/
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */  

TimeStruct currentTime;


/* Private function prototypes -----------------------------------------------*/
void Time_Set(u32 t);

/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar
* Description    : 
* Input          : 
* Output         : None
* Return         : struct tm
* Attention		 : None
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix
* Description    : 
* Input          : - t: struct tm
* Output         : None
* Return         : time_t
* Attention		 : None
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;
	return mktime(&t);
}


/*******************************************************************************
* Function Name  : Time_GetUnixTime
* Description    : 
* Input          : None
* Output         : None
* Return         : time_t
* Attention		 : None
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_GetCounter();
}

/*******************************************************************************
* Function Name  : Time_GetCalendarTime
* Description    : 
* Input          : None
* Output         : None
* Return         : struct tm
* Attention		 : None
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	return t_tm;
}

/*******************************************************************************
* Function Name  : Time_SetUnixTime
* Description    : 
* Input          : - t: time_t 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
	return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime
* Description    : 
* Input          : - t: struct tm
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
}

// /*******************************************************************************
// * Function Name  : USART_Scanf
// * Description    : USART Receive
// * Input          : - min_value: 
// *                  - max_value:
// *                  - lenght:
// * Output         : None
// * Return         : uint8_t
// * Attention		 : None
// *******************************************************************************/
// static uint16_t USART_Scanf(uint32_t min_value,uint32_t max_value,uint8_t lenght)
// {
//   uint16_t index = 0;
//   uint32_t tmp[4] = {0, 0, 0, 0};

//   while (index < lenght)
//   {
//     /* Loop until RXNE = 1 */
//     while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
//     {}
//     tmp[index++] = (USART_ReceiveData(USART1));

// 	if( tmp[index - 1] == 0x0D ) { index--; continue; }

//     if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
//     {
// //      printf("Please enter valid number between 0 and 9\r\n");
//       index--;
//     }
//   }
//   /* Calculate the Corresponding value */
//   if( lenght ==2 )
//   index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10 );

//   else  /* lenght ==4 */
//   index = (tmp[3] - 0x30) + ((tmp[2] - 0x30) * 10 ) + ((tmp[1] - 0x30) * 100 ) + ((tmp[0] - 0x30) * 1000 );
//   /* Checks */
//   if (index > max_value || index < min_value)
//   {
// //	printf("Please enter valid number between %d and %d\r\n", min_value, max_value);
//     return 0;
//   }


//   return index;
// }

/*******************************************************************************
* Function Name  : Time_Regulate
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_Regulate(void)
{
  struct tm time;

  memset(&time, 0 , sizeof(time) );
	RTC_Configuration();
//  printf("=======================Time Settings==========================\r\n");
  time.tm_year=currentTime.year + 2000;
  time.tm_mon=currentTime.month-1;
  time.tm_mday=currentTime.date;
  time.tm_hour=currentTime.hour;
  time.tm_min=currentTime.minute;
  time.tm_sec=0;
  /* Return the value to store in RTC counter register */
  Time_SetCalendarTime(time);  
}

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC Initialization
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None	
*******************************************************************************/
void RTC_Init(void)
{

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

 //   printf("RTC not yet configured....\r\n");

    /* RTC Configuration */
    RTC_Configuration();
		currentTime.year = 18;
		currentTime.month = 9;
		currentTime.date = 16;
		currentTime.hour=9;
		currentTime.minute=0;
		currentTime.second=0;		
		Time_Regulate();

	/* Adjust time by values entred by the user on the hyperterminal */

  //  printf("RTC configured....\r\n");

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
  //    printf("Power On Reset occurred....\r\n");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
//      printf("External Reset occurred....\r\n");
    }
// 		RTC_Configuration();
// 		Time_Regulate();
 //   printf("No need to configure RTC....\r\n");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  }

   /* NVIC configuration */
   NVIC_Configuration();

#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

   /* Clear reset flags */
  RCC_ClearFlag();
	
  return;
}

/*******************************************************************************
* Function Name  : Time_Display
* Description    : Printf Time
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_Display(void)
{
   struct tm time;

   time = Time_GetCalendarTime();
	 currentTime.second = time.tm_sec;
	 currentTime.minute = time.tm_min;
	 currentTime.hour = time.tm_hour;
	 currentTime.date = time.tm_mday;
	 currentTime.month = time.tm_mon+1;
	 currentTime.year = time.tm_year%100;

}

