#ifndef _GSM_DRV_H_
#define _GSM_DRV_H_

#define ON GPIO_PIN_SET
#define OFF GPIO_PIN_RESET

#include "main.h"

#define MAX_QUEUE  256

void GSM_PWR(GPIO_PinState pwr);
void GSM_On(void);
void GSM_Off(void);
void GSM_UART_init(void);


//typedef struct {
//	 char year;
//	 char month;
//	 char date;
//	 char hour;
//	 char minute;
//	 char second;
//} TimeStruct;

#endif
