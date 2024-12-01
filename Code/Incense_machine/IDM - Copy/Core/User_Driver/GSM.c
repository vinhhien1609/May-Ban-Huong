#include "GSM.h"
#include "main.h"

#define MaxCommand_GSM	3	//added 05/07/2010
#define MaxCommand_GSMLength	5	//added 05/07/2010
unsigned char commandGSMList[MaxCommand_GSM][MaxCommand_GSMLength]={
												 	/*0*/	"RMC,",
														 		"GG5,",
												 			 	"GSV,",

															  };	//added 05/07/2010

uint8_t queueUART2[];	//receiver queue
uint16_t topQueue2, bottomQueue2;

//extern TimeStruct currentTime;
char Latitude[13]={0,0}, direct_Lat='N', Longitude[13]={0,0}, direct_Long='E', isGPSUpdate =0;
char splitBuff[20][15]={"","","","","","","","","","","","","","",""};
unsigned int speed=0;
float _fLatitude=0, _fLongitude=0, _fSpeed=0, _fAltitude=0;


void GSM_On(void)
{
	
}
void GSM_Off(void)
{
	writeMStringUART2("$PMTK353,0,0,0,0,0*2B");	//Disable GPS and NOT searching all stand GLONASS, GALILEO, GALILEO_FULL, BEIDOU
}
//------------------------------------- for GSM module
unsigned char checkGPSCommand(void)		//check all in commandGPSList	added in 05/07/2010
{
	char c=0, buffer[100], temp=0;
	unsigned char i, j, k=0, n=0;//, cmdCode;
	unsigned int bandau;
	while(queueLengthUART2())
	{
		
		bandau=topQueue2;
		for(i=0; i<MaxCommand_GSM; i++)
		{
			if(queueLengthUART2()==0)
			{
				topQueue2=bandau;
				return 255;
			}

			c=readUART2();				//lay phan tu thu bandau

			if(c == commandGSMList[i][0])	//tim ky tu dau tien cua lenh thu i
			{
				for(j=1; commandGSMList[i][j]>0; j++)
				{
					if(queueLengthUART2()==0)
					{
						topQueue2=bandau;
						return 255;
					}

					c=readUART2();
					if(c!=commandGSMList[i][j])
						break;


				}//end for
				
				if(commandGSMList[i][j]==0)	//da ket thuc command tim xem la lenh nao thi lam tuong ung
				{
					
					switch (i)
					{
						case	0:	// position data (including position, velocity and time).
							
							k=0, n=0;
							while(temp!='*')		//end messenger
							{
								
								if(queueLengthUART2()==0)
								{
									topQueue2=bandau;
									return 255;
								}
								temp = readUART2();
								if(temp=='*')
								{
									isGPSUpdate =1;
									splitBuff[n][k] = 0;	//chot chuoi
									// convert data nhan duoc
									
									if(splitBuff[1][0] == 'A')	//GPS Avalid
									{
										// time
										//read hour
//										Time.hour = (splitBuff[0][0] - '0')*10 + (splitBuff[0][1]-'0');
//										//read Minute
//										Time.minute = (splitBuff[0][2] - '0')*10 + (splitBuff[0][3]-'0');
//										//read Second
//										Time.second = (splitBuff[0][4] - '0')*10 + (splitBuff[0][5]-'0');
//										//date
//										Time.date = (splitBuff[8][0] - '0')*10 + (splitBuff[8][1]-'0');
//										//month
//										Time.month = (splitBuff[8][2] - '0')*10 + (splitBuff[8][3]-'0');
//										//year
//										Time.year = (splitBuff[8][4] - '0')*10 + (splitBuff[8][5]-'0');										
										//Latitude
										n=0;
										while(n<12)
										{
											Latitude[n] = splitBuff[2][n];
											if(splitBuff[2][n]==0)	break;
											n++;
										}
										//Longitude
										n=0;
										while(n<12)
										{
											Longitude[n] = splitBuff[4][n];
											if(splitBuff[4][n]==0)	break;
											n++;
										}
										
										_fLatitude = NMEAtoDegree(Latitude);
										_fLongitude = NMEAtoDegree(Longitude);
										
										//speed
										//_fSpeed = (float)atof(splitBuff[6]);

// 										sprintf(buffer, "LAT: %f\r\n", _fLatitude);					// Print fomat degree
// 										writeMStringUART0(buffer);
// 										sprintf(buffer, "LONG: %f\r\n", _fLongitude);					// Print fomat degree
// 										writeMStringUART0(buffer);
// 										sprintf(buffer, "Altitude: %f\r\n", _fAltitude);					// Print fomat degree
// 										writeMStringUART0(buffer);
// 										sprintf(buffer, "Speed: %f\r\n", _fSpeed);					// Print fomat degree
// 										writeMStringUART0(buffer);										
									}
									else
									{
										_fLatitude =0.0;
										_fLongitude =0.0;
									}
									
//  									sprintf(buffer, "%d:%d:%d  %d/%d/%d speed: %d\r", Time.hour, Time.minute, Time.second, Time.date, Time.month, Time.year, speed);									
//  									writeMStringUART2(buffer);
									//
									break;
								}
								if(temp==',')	//ket thuc truong
								{
									splitBuff[n][k] = 0;	//chot chuoi
									//writeMStringUART2(splitBuff[n]);		//out debug tung chuoi 
									n++;		// chuyen truong
									k=0;
									splitBuff[n][k] = 0;
								}
								else
								{
									splitBuff[n][k] = temp;
									k++;
								}
								
							}
						break;
							
							
							//GGA
					case	1:	// position data (including position, velocity and time).
							
							k=0, n=0;
							while(temp!='*')		//end messenger
							{
								
								if(queueLengthUART2()==0)
								{
									topQueue2=bandau;
									return 255;
								}
								temp = readUART2();
								if(temp=='*')
								{
									splitBuff[n][k] = 0;	//chot chuoi
									// convert data nhan duoc
									
									if(splitBuff[5][0] != '0')	//GPS Valid
									{
										 _fAltitude = (float)atof(splitBuff[8]);
// 										sprintf(buffer, "Altitude: %f", _fAltitude);					// Print fomat degree
// 										writeMStringUART2(buffer);												
									}

									break;
								}
								if(temp==',')	//ket thuc truong
								{
									splitBuff[n][k] = 0;	//chot chuoi
									//writeMStringUART2(splitBuff[n]);		//out debug tung chuoi 
									n++;		// chuyen truong
									k=0;
									splitBuff[n][k] = 0;
								}
								else
								{
									splitBuff[n][k] = temp;
									k++;
								}
								
							}
						break;
							//break;
					}
					return i;
				}//end if
			}//end if
			topQueue2=bandau;				//reset lai dau chuoi nhan duoc
		}//end for
		c=readUART2();		//xoa phan tu vua duyet xong
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


