#include "arrow.bmp.h"

unsigned char code img_arrow_bmp_char_table[] = {
		0x7F, 0x00, 	/*  [ *******  ]  */
		0x3E, 0x00, 	/*  [  *****   ]  */
		0x1C, 0x00, 	/*  [   ***    ]  */
		0x08, 0x00, 	/*  [    *     ]  */
		0x00, 0x00, 	/*  [          ]  */
		
};



struct IMG_DEF code arrow_bmp = {10, 5, img_arrow_bmp_char_table};


