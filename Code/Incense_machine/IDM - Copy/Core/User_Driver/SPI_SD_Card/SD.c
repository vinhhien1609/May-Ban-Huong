#include "SD.h"

	FATFS fs;         /* Work area (file system object) for logical drive */
	FIL fsrc;         /* file objects */   
	FRESULT res;
	DIR dir;
	UINT br;
	char path[512]="0:";
	unsigned long cap;
	uint8_t Buffer[256] = "Thank you for using HY-FireBull V1.0 Development Board !^_^ \r\n";

	uint8_t isInsertSD =1;


unsigned char sd_init(void)
{
	res = disk_initialize(0);
	/*
	LcdPutString(30, 0, "  ",0);
	sprintf(Buffer,"%02u",res);
	LcdPutString(30, 0, Buffer,0);
	*/
	if(res==0)
	{
		res = f_mount(0,&fs);
		isInsertSD =1;
	}
	else
		isInsertSD =0;
	return(res);
// 	if(f_mount(0,&fs)==FR_OK)
// 	{
//		LcdPutString(10, 1, "mount",0);
// 		cap = SD_TotalSize();
// 		scan_files(path);
// 		res = f_open( &fsrc , "0:/Demofile ten dai.TXT" , FA_CREATE_NEW | FA_WRITE);
// 		if ( res == FR_OK )
//     { 
//       // Write buffer to file 
//       res = f_write(&fsrc, textFileBuffer, sizeof(textFileBuffer), &br);     
//  
//     
//       //close file 
//       f_close(&fsrc);      
//     }		
		
// 		return 0;
// 	}
// 	else
// 		return 1;
}


void WriteLog(char *str, uint8_t *filename)
{
	char s[100];
	if(sd_init())
	{
		isInsertSD =0;
		return;
	}
	
	res = f_opendir(&dir, path);
	isInsertSD = 1;
	if(f_open(&fsrc,filename, FA_READ | FA_WRITE) == 0){
		do{
			res = f_read(&fsrc, Buffer, sizeof(Buffer)-1, &br);
		}while(br != 0);
	}else{
		res = f_open(&fsrc,filename, FA_READ | FA_WRITE | FA_CREATE_NEW);
	}
	if(res == FR_OK)
	{
		f_write(&fsrc, str, strlen(str), &br);
 //		sprintf(s,"%u-",strlen(str));
 //		LcdPutString(0, 4, s,0);

		f_close(&fsrc);
	}
	
}

/*******************************************************************************
* Function Name  : scan_files
* Description    : 搜索文件目录下所有文件
* Input          : - path: 根目录
* Output         : None
* Return         : FRESULT
* Attention		 : 不支持长文件名
*******************************************************************************/
FRESULT scan_files (char* path)
{
	char  s[100];
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "/%s", fn);
								//GUI_Text(10,n*16,&path[i],Red,Black);
								LcdPutString(10, 1, &path[i],0);
							_delay_ms(100);
						
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {
                sprintf(s,"%s/%s", path, fn);

								//GUI_Text(10,n*16,s,Red,Black);
								LcdPutString(10, 1, &path[i],0);
							_delay_ms(100);
            }
        }
    }
    return res;
}

/*******************************************************************************
* Function Name  : SD_TotalSize
* Description    : 文件空间占用情况
* Input          : None
* Output         : None
* Return         : 返回1成功 返回0失败
* Attention		 : None
*******************************************************************************/
int SD_TotalSize(void)
{
    FATFS *fs;
    DWORD fre_clust;        
		char s[100];
    res = f_getfree("0:", &fre_clust, &fs);  /* 必须是根目录，选择磁盘0 */
		//LcdPutString(10, 1, "get FRee",0);
    if ( res==FR_OK ) 
    {
			isInsertSD =1;
	  /* Print free space in unit of MB (assuming 512 bytes/sector) */
      sprintf(s,"%4u/%u MB",
           (fre_clust * fs->csize) / 2 /1024,( (fs->n_fatent - 2) * fs->csize ) / 2 /1024  );
			LcdPutString(5, 4, s,0);
			DrawBar(10,2,60,(((fs->n_fatent - 2) - fre_clust)  *100)/(fs->n_fatent - 2));
			
			isInsertSD= 1;
			return ENABLE;
		}
		return DISABLE;
}	

