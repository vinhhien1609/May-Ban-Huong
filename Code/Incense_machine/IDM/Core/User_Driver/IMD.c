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
#include "stm32f1xx_hal.h"
//#include "stm32f1xx_hal_rtc.c"
#include "IDM.h"
#include "main.h"
#include "flash.h"
#include "nv11.h"
IDM_PARA IDM;
IDM_HARDWARE IDM_Status;
BUY_PARA buy;
IDM_ERROR IDM_Errors;
LED Led1, Led2, Led3;
float Humidity, Temperature;
unsigned char havemoney=0, IDM_state=0, State=0xFF;

/* Private define ------------------------------------------------------------*/
uint32_t count=0;
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void Led_init(LED *led, uint16_t time_On, uint16_t time_Cycle, unsigned int time_Active)
{
	led->counter =0;
	led->timeActive = time_Active;
	led->timeCycle = time_Cycle;
	led->timeOn = time_On;
	led->enable = true;
}

void IDM_init(void)
{
//	Write_config();
	Read_config();
	Led_init(&Led1, 250, 500, 2000);
	Led_init(&Led2, 250, 500, 3000);
	Led_init(&Led3, 250, 500, 4000);
}


uint8_t time_blink_led_error_cell =0;

void Scan_IDM(void)
{
	uint16_t timeConveyerRun[3] ={10, 10, 20};

//	if(buy.StateBuy == STOP_SERVICE && IDM_Status.isEmptyIsenseSW ==false)
//	{
//		buy.StateBuy = CELL_WAIT;
//		IDM_state = 1;
//		IDM.currentNumberBuyMore = IDM.NumberBuyMore;
//		IDM.currentRetryCellEmpty = IDM.retryCellEmpty;
//		Write_config();
//	}
	
	if(time_blink_led_error_cell<12)
	{
		if(Led3.timeActive ==1000 && Led3.enable==false)
		{
			Led_init(&Led3,0,500,500);		// led tat 500ms
			time_blink_led_error_cell++;
		}
		if(Led3.timeActive ==500 && Led3.enable==false)
		{
			Led_init(&Led3,250,500,1000);		//2 chu ky on/off
		}
	}
	
	switch (IDM_state)
	{
		case 0:		//start Machine
			if(State!=IDM_state)
			{
				count=0;
				State = IDM_state;
				IDM_Status.Motor_swap.isRun =1;
				printf("CELL>> %d\r\n", IDM_state);
//				NV11_Disable(true);
			}
			else
			{
				if(count/1000 >=IDM.TimeSWAPRun)	// dao huong	TimeSwapIsense
				{
					IDM_Status.Motor_swap.isRun =0;
					IDM_state =1;
					buy.StateBuy = HELLO_CUSTOM;
				}
			}
			break;
		case 1:		// wait custom
			if(State!=IDM_state)
			{
				count=0;
				State = IDM_state;
				IDM_Status.Motor_conveyer.isRun =0;
				IDM_Status.Motor_swap.isRun = 0;
				IDM_Status.Solenoid.isRun=0;
//				buy.StateBuy = CELL_WAIT;
				printf("CELL>> %d\r\n", IDM_state);
//				nv11_enable(true);
				Led_init(&Led1,0,10,2000);
				
				if((IDM.currentNumberBuyMore==0 || IDM.currentRetryCellEmpty==0) && buy.StateBuy != STOP_SERVICE)
				{
					buy.StateBuy = STOP_SERVICE;
					IDM_state =6;
//					NV11_Disable(true);
				}
				if(IDM.currentTotalInsenseBuy==0)		//chu ky dao huong
					IDM_state = 5;
			}
			else
			{
				if(havemoney)
				{
					havemoney =0;
					count=MAXTimeDropRespone-10;
					buy.TotalIsenseDroped = IDM.NumberInsenseBuy;
					buy.numberRetry =0;		// time retry when conveyer no out insense
	//						buy.StateBuy = WAIT_INSENCE;
					IDM_state = 2;
					Led_init(&Led1,10,10,200000);
					printf("CELL>> %d\r\n", IDM_state);
//					NV11_Disable(true);
				}
			}
			break;
		case 2:		// Buying conveyer run
			if(State!=IDM_state)
			{
				State = IDM_state;
				IDM_Status.Motor_conveyer.isRun =1;
				IDM_Status.Motor_swap.isRun =0;
				Led_init(&Led2,0,10,2000);
				Led_init(&Led3,0,10,2000);
				count =0;
			}
			else
			{
				if(count/1000 >=timeConveyerRun[buy.numberRetry])
				{
					IDM_state =3;
				}
				if(buy.TotalIsenseDroped==0)
				{
					IDM_state =4;
					buy.StateBuy = CELL_SUCCESS;
				}
			}				
			break;
		case 3:		// qua thoi gian ma khong thay huong--> Dao huong
			if(State!=IDM_state)
			{
				count=0;
				State = IDM_state;
				if(buy.numberRetry<2)
				{
					buy.numberRetry ++;
					IDM_Status.Motor_swap.isRun =1;		// dao huong
					IDM_Status.Motor_conveyer.isRun =0;
				}
				else
				{
					IDM_Status.Motor_swap.isRun =0;		// dao huong
					IDM_Status.Motor_conveyer.isRun =0;
					IDM_state =6;		// INSENCE IMPTY
					buy.StateBuy = INSENCE_EMPTY;
				  sync_number_celled(IDM.NumberInsenseBuy - buy.TotalIsenseDroped);		// so que da nha ra
					printf("IDM>> EMPTY INSENCE AND WILL RETRY TIME: %d\r\n",IDM.currentRetryCellEmpty);
					printf("IDM>> CYCLE CELL INSENCE: %d\r\n",IDM.currentTotalInsenseBuy);
					Led_init(&Led3,125,250,20000);		//20s
				}
			}
			else
			{
				if(count>=1500)		//1.5s
				{
					IDM_state =2;
				}
				if(buy.TotalIsenseDroped==0)
				{
					buy.StateBuy = CELL_SUCCESS;
					IDM_state =4;
				}
			}
			break;

		case 4:		//CELL_SUCCESS
			if(State!=IDM_state)
			{
				count=0;
				IDM_Status.Motor_conveyer.isRun =0;
				IDM_Status.Motor_swap.isRun =0;
				IDM_Status.Solenoid.isRun=1;
				State = IDM_state;
				printf("CELL>> %d\r\n", IDM_state);
				IDM.currentRetryCellEmpty = IDM.retryCellEmpty;
				sync_number_celled(IDM.NumberInsenseBuy);
			  printf("IDM>> CYCLE CELL INSENCE: %d\r\n",IDM.currentTotalInsenseBuy);				
			}
			else
			{
				if(count/1000 >=2)
				{
					IDM_Status.Solenoid.isRun=0;
					IDM_state =1;
					Led_init(&Led2,100,100,10000);
					Led_init(&Led1,0,10,2000);		//led1 off
				}
			}
			break;
		case 5:		// Chu ky dao huong
			if(State!=IDM_state)
			{
				State = IDM_state;				
				count=0;
				IDM_Status.Motor_swap.isRun =1;
				printf("CELL>> %d\r\n", IDM_state);
//				NV11_Disable(true);
			}
			else
			{
				if(count/1000 >=IDM.TimeSWAPRun)
				{
					IDM_state =1;		// wait cell
					IDM.currentTotalInsenseBuy = IDM.TotalInsenseCycleSwapBuy;
					Write_config();
				}
			}
			break;
		case 6:	// INSENCE IMPTY
			if(State != IDM_state)
			{
				State = IDM_state;
				Led_init(&Led3,250,500,1000);		//2 chu ky on/off
				Led_init(&Led1,0,10,2000);		//led1 off
				time_blink_led_error_cell =0;
			}
			else
				if(buy.StateBuy == HELLO_CUSTOM)
				{
					IDM_state = 1; //celling
				}
				if(buy.StateBuy == STOP_SERVICE)	// tam ngung phuc vu
				{
					time_blink_led_error_cell = 100;
					if(Led3.enable==false)
						Led_init(&Led3,250,500,1000);		//2 chu ky on/off
				}
			break;
	}
	
	if(Humidity >= IDM.HumidityMAX && IDM.EnableHumidity>0)
		IDM_Status.isHeater = true;
	else
		IDM_Status.isHeater = false;
	
	if(IDM_Status.isHeater==true)	HAL_GPIO_WritePin(Heater_GPIO_Port, Heater_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(Heater_GPIO_Port, Heater_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Motor_conveyer.isRun)	HAL_GPIO_WritePin(CONVEYER_MOTOR_GPIO_Port, CONVEYER_MOTOR_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(CONVEYER_MOTOR_GPIO_Port, CONVEYER_MOTOR_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Motor_swap.isRun)	HAL_GPIO_WritePin(SWAP_MOTOR_GPIO_Port, SWAP_MOTOR_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(SWAP_MOTOR_GPIO_Port, SWAP_MOTOR_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Solenoid.isRun)	HAL_GPIO_WritePin(SOLENOID_GPIO_Port, SOLENOID_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(SOLENOID_GPIO_Port, SOLENOID_Pin, GPIO_PIN_RESET);
	
	if(HAL_GPIO_ReadPin(DOOR_GPIO_Port, DOOR_Pin)==0)	IDM_Status.isDoorOpen = true;
	else	IDM_Status.isDoorOpen = false;
	
		if(HAL_GPIO_ReadPin(Empty_GPIO_Port, Empty_Pin))	IDM_Status.isEmptyIsenseSW = false;
	else	IDM_Status.isEmptyIsenseSW = true;
	
}

/* Private function prototypes -----------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == Drop_Pin)
	{
		if(count>= MAXTimeDropRespone)
		{
			if(buy.TotalIsenseDroped)	buy.TotalIsenseDroped--;
			if(buy.TotalIsenseDroped==0)	IDM_Status.Motor_conveyer.isRun=0;
			count =0;		// chong rung			
		}

	}
}

