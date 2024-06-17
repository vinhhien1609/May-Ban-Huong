#ifndef _GSM_DRV_H_
#define _GSM_DRV_H_

#define ON 1
#define OFF 0

#include "main.h"

#define MAX_QUEUE  256

void GSM_PWR(uint8_t pwr);
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
