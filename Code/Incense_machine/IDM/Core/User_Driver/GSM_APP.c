#include "GSM_APP.h"
#include "main.h"
#include "RTC_time.h"
#include "vdm_device_config.h"

#define MAX_TIMEOUT_4G	200	//second
#define MaxCommand_GSM	16	//added 05/07/2010
#define MaxCommand_GSMLength	26	//added 05/07/2010

char APN[20];
char USER[20];
char PASS[20];

unsigned char commandGSMList[MaxCommand_GSM][MaxCommand_GSMLength]={
												 	/*0*/	"+CPIN: READY",
														 		"+CSQ: ",
												 			 	"GSV,",
																"OK",
																"45201",		//COPS 4
																"+NETOPEN: 1",
																"Network is already opened",
																"+IPCLOSE:",
																"+CIPOPEN:",		//8
																"CMD=|v|",
																"45204",	//vietel
																"45202",	//vinaphone
																"45207",	//Gmobile
																"45205",	//vietnammobile	
																"SIM REMOVED",
																"+CME ERROR",
															  };	//added 05/07/2010

//uint8_t queueUART2[];	//receiver queue
uint16_t topQueue, bottomQueue;
uint16_t AT_timeout=0, Port=0, count_timeout=0;
uint8_t GSM_buffer[MAX_QUEUE], TCP_step=0, current_TCP_step=255, AT_respone=0;
extern UART_HandleTypeDef huart3;
extern GSM_State GSM;
extern	rtc_date_time_t currentTime;
extern vdm_device_config_t m_device_config;
char s[50], ip_server[16];
//------------------------------------- for GSM module

void GSM_init(void)
{
	GSM_Off();
	topQueue=0;
	bottomQueue=0;
	GSM.Connect4G =0;
	GSM.Connect_State =0;
	GSM.CSQ =0;
	GSM.repone =0;
	GSM.SimReady =255;
	GSM_UART_init();
	GSM_On();
	sprintf(ip_server,"%d.%d.%d.%d", (m_device_config.server.addr>>24)&0xFF, (m_device_config.server.addr>>16)&0xFF,
																	(m_device_config.server.addr>>8)&0xFF, m_device_config.server.addr&0xFF);
	Port = m_device_config.server.port;
	
	printf("GSM>> ServerIP: %s\r\nPort: %d", ip_server, Port);
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



void TCP_send(uint8_t *data, uint32_t length)
{
	if(GSM.Connect4G)
	{
		printf("GSM>>");
		for(int n=0; n<length; n++)
			printf(" %02X",data[n]);
		printf("\r\n");
		
						sprintf(s, "AT+CIPSEND=1,\r\n");
						HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));		
						HAL_Delay(500);
//						sprintf(s, "###|TPA9999999|CMD=|a|1&&&");
						HAL_UART_Transmit_DMA(&huart3,data,length);
						HAL_Delay(500);
						HAL_UART_Transmit_DMA(&huart3,(uint8_t*)"\x1A",1);
	}
	else
		printf("GSM>> NO Connect to server\r\n");
}

void TCP_reconnect(void)
{
	TCP_step =0;
	GSM_Off();
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

void TCP_connect(void)
{
	if(count_timeout > MAX_TIMEOUT_4G)
	{
		TCP_reconnect();
		GSM_init();
		count_timeout =0;
	}
	if((GSM.SimReady!=1 || TCP_step==6) && (currentTime.second%5==0))
		HAL_UART_Transmit_DMA(&huart3,(uint8_t*)"AT+CPIN?\r\n",10);
	if(GSM.SimReady==1)
	{
		switch(TCP_step)
		{
			case 0:
				if(current_TCP_step != TCP_step)
				{
					current_TCP_step = TCP_step;
					AT_respone =AT_SENDING;
					AT_timeout =0;
					sprintf(s, "AT+COPS?\r\n");
					HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
					GSM.Connect4G =0;
				}
				else
				{
					if(AT_timeout>2000)
					{
						AT_timeout=0;
						HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
					}				
				}
				break;			
			case 1:
				if(current_TCP_step != TCP_step)
				{
					current_TCP_step = TCP_step;
					AT_respone =AT_SENDING;
					AT_timeout =0;
					sprintf(s, "AT+QICSGP=1,1,\"%s\",\"%s\",\"%s\",1\r\n", APN, USER, PASS);
					HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
				}
				else
				{
					if(AT_respone==OK)
						TCP_step =2;
					else
						if(AT_timeout>2000)
						{
							AT_timeout=0;
							HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
						}				
				}
				break;
				
			case 2:
				if(current_TCP_step != TCP_step)
				{
					current_TCP_step = TCP_step;
					AT_respone =AT_SENDING;
					AT_timeout =0;
					sprintf(s, "AT+QIACT=1\r\n");
					HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
				}
				else
				{
					if(AT_respone==OK)
						TCP_step =3;
					else
						if(AT_timeout>2000)
						{
							AT_timeout=0;
							HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
						}
				}
				break;
			case 3:
				if(current_TCP_step != TCP_step)
				{
					current_TCP_step = TCP_step;
					AT_respone =AT_SENDING;
					AT_timeout =0;
					sprintf(s, "AT+NETOPEN\r\n");
					HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
				}
				else
				{
					if(AT_respone==OK)
						TCP_step =5;
					else
					{
						if(AT_timeout>2000)
						{
							AT_timeout=0;						
							HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));	
						}	
					}					
				}
				break;
			case 5:
				if(current_TCP_step != TCP_step)
				{
					current_TCP_step = TCP_step;
					AT_respone =AT_SENDING;
					AT_timeout =0;
					sprintf(s, "AT+CIPOPEN=1,\"TCP\",\"%s\",%d\r\n", ip_server, Port);
	//				sprintf(s, "AT+CIPOPEN=1,\"TCP\",\"14.231.219.132\",9201\r\n");
					HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));
				}
				else
				{
					if(AT_respone==OK)
						TCP_step =6;
					else
						if(AT_timeout>2000)
						{
							AT_timeout=0;						
							HAL_UART_Transmit_DMA(&huart3,(uint8_t*)s,strlen(s));	
						}						
				}
				break;
		}
	}
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
							if(GSM.SimReady!=1)
							{
								TCP_step =0;
								current_TCP_step=255;
								GSM.SimReady =1;
								HAL_UART_Transmit_DMA(&huart3,"AT+AUTOCSQ=1,0\r\n",17);
							}
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
					case 3:
						AT_respone =OK;
						break;
					case 4:	//mobiphone
						sprintf(APN,"m-wap");
						sprintf(USER,"mms");
						sprintf(PASS,"mms");
						TCP_step =1;		//start connect TCP/IP
						break;
					case 10:	//viettel
						sprintf(APN,"v-internet");
						sprintf(USER,"");
						sprintf(PASS,"");
						TCP_step =1;		//start connect TCP/IP					
						break;
					case 11:	//vinaphone
						sprintf(APN,"m3-word");
						sprintf(USER,"mms");
						sprintf(PASS,"mms");
						TCP_step =1;		//start connect TCP/IP					
						break;
					case 12:	//GMobile
						sprintf(APN,"mms");
						sprintf(USER,"mms");
						sprintf(PASS,"mms");	
						TCP_step =1;		//start connect TCP/IP
						break;
					case 13:	//Vietnammobile
						sprintf(APN,"v-internet");
						sprintf(USER,"");
						sprintf(PASS,"");			
						TCP_step =1;		//start connect TCP/IP
						break;
					case 14:
						GSM.SimReady = 0;
						GSM.CSQ =0;
						break;
					case 15:
						GSM.SimReady = 0;
						GSM.CSQ =0;				
						break;
					case 5:
						AT_respone = NETOPEN_READY;
						TCP_step = 5;
						break;
					case 6:
						TCP_step =5;
						break;	
					case 7:
						TCP_step =0;
						break;
					case 8:
						GSM.Connect4G =1;

						count_timeout =0;
						break;
					case 9:		// server FB
						count_timeout =0;
						if(queueLengthGSM()<6)
						{
								topQueue=bandau;
								return 255;
						}
						for(n=0; n<6; n++)		// 6 byte time yy/mm/dd/hh/mm/ss
						{
							buffer[n] = readGSM_Buff();
						}
						rtc_date_time_t s_time;
						s_time.year = 2000+ buffer[0];
						s_time.month = buffer[1];
						s_time.day = buffer[2];
						s_time.hour = buffer[3];
						s_time.minute = buffer[4];
						s_time.second = buffer[5];
						if(currentTime.hour !=s_time.hour || currentTime.minute !=s_time.minute || currentTime.year !=s_time.year\
							|| currentTime.month !=s_time.month || currentTime.day !=s_time.day)
						{
							setRTC(s_time);
							printf("GSM>> Server time sync: %02X %02X %02X %02X %02X %02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3],\
							buffer[4], buffer[5]);
						}
						break;					
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
