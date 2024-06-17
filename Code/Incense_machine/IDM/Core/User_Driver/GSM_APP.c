#include "GSM_APP.h"
#include "main.h"

#define MaxCommand_GSM	3	//added 05/07/2010
#define MaxCommand_GSMLength	12	//added 05/07/2010
unsigned char commandGSMList[MaxCommand_GSM][MaxCommand_GSMLength]={
												 	/*0*/	"SIM REMOVED",
														 		"+CSQ: ",
												 			 	"GSV,",

															  };	//added 05/07/2010

//uint8_t queueUART2[];	//receiver queue
uint16_t topQueue, bottomQueue;
uint8_t GSM_buffer[MAX_QUEUE];
extern GSM_State GSM;
//------------------------------------- for GSM module

void GSM_init(void)
{
	topQueue=0;
	bottomQueue=0;
	GSM.Connect4G =0;
	GSM.Connect_State =0;
	GSM.CSQ =0;
	GSM.repone =0;
	GSM.SimReady =255;
	GSM_UART_init();	
	GSM_On();
}
uint16_t queueLengthGSM(void)
{
	return ((bottomQueue>=topQueue) ? (bottomQueue - topQueue) : ((MAX_QUEUE - topQueue) + bottomQueue));
}
char readGSM_Buff(void)
{
	uint8_t temp=0;
	if(topQueue!=bottomQueue)
	{
		temp = GSM_buffer[topQueue];
		topQueue++;
		if (topQueue >= MAX_QUEUE) topQueue = 0;
	}
	return temp;
}
																
unsigned char checkGPSCommand(void)		//check all in commandGPSList	added in 05/07/2010
{
	char c=0, buffer[100];
	unsigned char i, j, n;//, cmdCode;
	unsigned int bandau;	
	while(queueLengthGSM())
	{		
		bandau=topQueue;
		for(i=0; i<MaxCommand_GSM; i++)
		{
			if(queueLengthGSM()==0)
			{
				topQueue=bandau;
				return 255;
			}
			c=readGSM_Buff();				//lay phan tu thu bandau

			if(c == commandGSMList[i][0])	//tim ky tu dau tien cua lenh thu i
			{
				for(j=1; commandGSMList[i][j]>0; j++)
				{
					if(queueLengthGSM()==0)
					{
						topQueue=bandau;
						return 255;
					}

					c=readGSM_Buff();
					if(c!=commandGSMList[i][j])
						break;
				}//end for
				
				if(commandGSMList[i][j]==0)	//da ket thuc command tim xem la lenh nao thi lam tuong ung
				{
					
					switch (i)
					{
						case	0:	// position data (including position, velocity and time).
							GSM.SimReady =0;
						break;
							
							
							//GGA
					case	1:	// position data (including position, velocity and time).
						if(queueLengthGSM()<2)
						{
								topQueue=bandau;
								return 255;
						}
						for(n=0; n<2; n++)
						{
							buffer[n] = readGSM_Buff();
							if(buffer[n]==',')	break;
						}
						buffer[n] = 0;
						GSM.CSQ = atoi(buffer);
//						if(buffer[0] >='0' && buffer[0] <='9')
//							GSM.CSQ = buffer[0]-'0';
//						buffer[1] = readGSM_Buff();
//						if(buffer[1] >='0' && buffer[1] <='9')
//						{
//							GSM.CSQ *=10;
//							GSM.CSQ +=buffer[1]-'0';
//						}
						break;
							//break;
					}
					return i;
				}//end if
			}//end if
			topQueue=bandau;				//reset lai dau chuoi nhan duoc
		}//end for
		c=readGSM_Buff();		//xoa phan tu vua duyet xong
	}//end while
	return 255;
}

float NMEAtoDegree(char* str)
{
//	char s[100];
	float temp, resume;
	unsigned int _itemp=0;
	
	temp = (float)atof(str); //convert string to float "10547.5089" -->10547.5089
	resume = temp;
	_itemp = ((unsigned int)temp/100)*100;
	temp =(float)temp-_itemp;		//lay minutes cua toa do
	temp/=60;		//degree to minutes
	resume =  (int)(resume/100) + (float)temp;
	return (resume);
}


