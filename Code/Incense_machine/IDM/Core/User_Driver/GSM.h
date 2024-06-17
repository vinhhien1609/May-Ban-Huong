#ifndef _GSM_APP_H_
#define _GSM_APP_H_


#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"



unsigned char checkGPSCommand(void);		//check all in commandList	added in 05/07/2010
void GSM_On(void);
void GSM_Off(void);
float NMEAtoDegree(char* str);


//typedef struct {
//	 char year;
//	 char month;
//	 char date;
//	 char hour;
//	 char minute;
//	 char second;
//} TimeStruct;

#endif
