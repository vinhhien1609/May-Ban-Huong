#ifndef _GSM_APP_H_
#define _GSM_APP_H_


#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "GSM_drv.h"
#include <stdbool.h>


unsigned char checkGPSCommand(void);		//check all in commandList	added in 05/07/2010
void GSM_init(void);



typedef struct {
	 unsigned char SimReady;
	 bool Connect4G;
	 char CSQ;
	 unsigned char Connect_State;
	 unsigned char repone;
} GSM_State;

#endif
