#ifndef _SD_H
#define _SD_H
#include "ff.h"
#include "SPI_MSD_Driver.h"
#include "diskio.h"
#include "stdio.h"
#include "pcd8544.h"

unsigned char sd_init(void);
int SD_TotalSize(void);
FRESULT scan_files (char* path);
void WriteLog(char *str, uint8_t *filename);


#endif
