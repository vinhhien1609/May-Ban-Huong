#include "keyboard.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "vdm_language.h"

#define KEY_HOLE_TIME 50
#define KEY_REPEAT_TIME 1500

extern struct _ts_event ts_event; 

VITUAL_KEYBOARD key_board;
unsigned char key_tick, key_map[16]= {'0','1','2','3','4','5','6','7','8','9','*','#'}, old_key;
unsigned int key_count=0, pen= 255, old_pen=0;
bool sta_up=false;


void show_Vitual_key(void);
void show_Vitual_confirm_key(void);

static void small_delay()		// not importance function
{
	volatile uint32_t i = 0x02F;
	while (i--);
}

unsigned char scan_BT(void)
{
	unsigned char temp= key_tick;
	if( temp!=255)	printf("KEY >> %c Pressed\r\n", key_tick);
	key_tick =255;
	return (temp);
}
	
void scan_switch(void)
{
	unsigned char key=255;
	HAL_GPIO_WritePin(KEY_COL1_GPIO_Port, KEY_COL1_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =0;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =1;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =2;
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =3;	//*
	HAL_GPIO_WritePin(KEY_COL1_GPIO_Port, KEY_COL1_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(KEY_COL2_GPIO_Port, KEY_COL2_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =4;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =5;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =6;
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =7;
	HAL_GPIO_WritePin(KEY_COL2_GPIO_Port, KEY_COL2_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(KEY_COL3_GPIO_Port, KEY_COL3_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =8;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =9;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =10;	//*
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =11;	//#
	HAL_GPIO_WritePin(KEY_COL3_GPIO_Port, KEY_COL3_Pin, GPIO_PIN_RESET);

	if(old_key==key && key <255)
	{
		key_count ++;
		if(key_count > KEY_REPEAT_TIME)
			key_count =0;
		if(key_count==KEY_HOLE_TIME)
		{
			Buzz_On(10);
				key_tick = key_map[key];// chi gui 1 lan cho toi khi nha
		}
	}
	else
	{
		key_count =0;
		old_key = key;
	}

}

void active_keyboard(bool act)
{
	key_board.act = act;
}

//vitual keyboard
void Enable_Vitual_Confirm_key(bool act)
{
	if(act)
	{
		key_board.act = true;
		sta_up=false;
	}
	else
	{
		key_board.act = false;
		sta_up=false;
	}
	key_board.type = CONFIRM_KEY;	
}


//vitual keyboard
void Enable_Vitual_key(bool act)
{
	if(act)
	{
		key_board.act = true;
		sta_up=false;
	}
	else
	{
		key_board.act = false;
		sta_up=false;
	}
	key_board.type = NUMBER_KEY;
}

touch_event_t get_touch_env()
{
	return ts_event.touch_event;
}

bool get_touch(touch_event_t t_ev, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	touch_event_t temp = ts_event.touch_event;
	if( temp== t_ev)
	{
		ts_event.touch_event = NO_TOUCH;		
		if(ts_event.x1>x && ts_event.x1<(x+w) && ts_event.y1>y && ts_event.y1<(y+h))
			return true;
	}
	return false;
}

struct _ts_event get_mouse(void)
{
	return ts_event;
}

void scan_vitual_key(void)
{
	if(key_board.act)
	{
		if(	key_board.type == NUMBER_KEY)
			show_Vitual_key();
		if(	key_board.type == CONFIRM_KEY)
			show_Vitual_confirm_key();
	}
//	else
//		ts_event.touch_event = NO_TOUCH;
}

unsigned char get_vitual_key()
{
	KeyboardRes key = key_board.res;
	key_board.res = KEY_NONE;
	return key;
}

void show_Vitual_confirm_key(void)
{
	uint16_t n, m, point_x= 470, point_y= 140, b_w = 270, b_h= 90;
	char s[20], key_matrix[12] = {'1','2','3','4','5','6','7','8','9','*','0','#'};	
	if(sta_up==false)
	{
		sta_up=true;
	
//		drawRoundRect(point_x+gap, point_y+gap, b_w*3+gap*2, b_h, 10, color_white, true);	//Text box
		drawRoundRect(point_x, point_y, 320, 330, 10, color_blue, true);	// frame
		drawRoundRect(point_x, point_y, 320,330, 10, color_brown, false);	// frame
		drawRoundRect(point_x+1, point_y+1, 318, 328, 9, color_brown, false);
		drawRoundRect(point_x + (320 - b_w)/2, point_y + (330 - b_h)/2, b_w, b_h, 10, color_green, true);		//drawRoundRect button
		sprintf(s, "%s", vdm_language_text(VDM_LANG_ACCEPT));
		LCD_background(color_green);
		TFT_putString(point_x + (320 - b_w)/2 + 20,point_y + (330 - b_h)/2 + 20,s,&Arial_32,color_red);
	}
	else
	{
		if(ts_event.x1>point_x + (320 - b_w)/2 && ts_event.x1<point_x + (320 - b_w)/2 + b_w && point_y + (330 - b_h)/2 && ts_event.y1<point_y + (330 - b_h)/2 + b_h)
		{
		
			if(ts_event.touch_event == TOUCH_DOWN)
			{
				key_tick = ENTER;
				Buzz_On(50);
			}
		}
	}	
}

void show_Vitual_key(void)
{
	uint16_t n, m, point_x= 470, point_y= 140, b_w = 100, b_h= 60, gap =5;
	char s[20], key_matrix[12] = {'1','2','3','4','5','6','7','8','9','*','0','#'};
	if(sta_up==false)
	{
		sta_up=true;
	
		drawRoundRect(point_x+gap, point_y+gap, b_w*3+gap*2, b_h, 10, color_white, true);	//Text box
		drawRoundRect(point_x, point_y, b_w*3 + gap*4, b_h*5+gap*6, 10, color_brown, false);	// frame
		drawRoundRect(point_x+1, point_y+1, b_w*3 + gap*4-2, b_h*5+gap*6-2, 9, color_brown, false);
		for(n=0; n<4; n++)
			for(m=0; m<3; m++)
			{
				drawRoundRect(point_x+gap + (m*(b_w+gap)), point_y + b_h + gap*2+ (n*(b_h+gap)), b_w, b_h, 10, color_green, true);
				sprintf(s,"%c",key_matrix[n*3 + m]);
//				LCD_background(color_green);
				TFT_DrawString_X4(s,point_x+gap + (m*(b_w+gap)) + (b_w-24)/2, point_y + b_h + gap*2+ (n*(b_h+gap))+(b_h - 48)/2,color_red);
			}
	}
	else
	{
		if(ts_event.x1>point_x && ts_event.x1<point_x + (b_w +gap)*3 && ts_event.y1>point_y + b_h +gap && ts_event.y1<point_y + b_h +gap + (b_h+gap)*4)
		{
			if(((ts_event.x1-point_x)%(b_w +gap)) <b_w && ((ts_event.y1-(point_y+b_h+gap))%(b_h + gap))<b_h)
				pen = (ts_event.x1-point_x)/(b_w + gap) + ((ts_event.y1-(point_y+b_h+gap))/(b_h + gap) *3);
			
			if(ts_event.touch_event == TOUCH_DOWN)
			{
//				key_count =0;
				if(old_pen != pen)
				{
//						sprintf(s,"%d-%d ",old_pen, pen);
//						LCD_background(color_white);
//						TFT_DrawString(s,300, 30,color_red);
					m = pen%3;
					n= pen/3;
					
          drawRoundRect(point_x+gap + (m*(b_w+gap)), point_y + b_h + gap*2+ (n*(b_h+gap)), b_w, b_h, 10, color_black, false);
					drawRoundRect(point_x+gap + (m*(b_w+gap))+1, point_y + b_h + gap*2+ (n*(b_h+gap))+1, b_w-2, b_h-2, 9, color_black, false);
					drawRoundRect(point_x+gap + (m*(b_w+gap))+2, point_y + b_h + gap*2+ (n*(b_h+gap))+2, b_w-4, b_h-4, 8, color_black, false);
					if(old_pen<0xFF)
					{
						m = old_pen%3;
						n= old_pen/3;
						drawRoundRect(point_x+gap + (m*(b_w+gap)), point_y + b_h + gap*2+ (n*(b_h+gap)), b_w, b_h, 10, color_green, false);
						drawRoundRect(point_x+gap + (m*(b_w+gap))+1, point_y + b_h + gap*2+ (n*(b_h+gap))+1, b_w-2, b_h-2, 9, color_green, false);
						drawRoundRect(point_x+gap + (m*(b_w+gap))+2, point_y + b_h + gap*2+ (n*(b_h+gap))+2, b_w-4, b_h-4, 8, color_green, false);
					}
					old_pen = pen;
				}			
			}
			if(ts_event.touch_event == TOUCH_UP)
			{
//				Buzz_On(10);
				old_pen = 0xff;
				ts_event.touch_event = NO_TOUCH;
					m = pen%3;
					n= pen/3;
          drawRoundRect(point_x+gap + (m*(b_w+gap)), point_y + b_h + gap*2+ (n*(b_h+gap)), b_w, b_h, 10, color_green, false);
					drawRoundRect(point_x+gap + (m*(b_w+gap))+1, point_y + b_h + gap*2+ (n*(b_h+gap))+1, b_w-2, b_h-2, 9, color_green, false);
					drawRoundRect(point_x+gap + (m*(b_w+gap))+2, point_y + b_h + gap*2+ (n*(b_h+gap))+2, b_w-4, b_h-4, 8, color_green, false);						
						
						Buzz_On(50);
//						m = (ts_event.x1-100)/105;
//						n = (ts_event.y1-100)/65;
//						key_tick = key_matrix[n*3 + m];
						key_tick = key_matrix[pen];
//						sprintf(s,"%c",key_tick);
//						LCD_background(color_white);
//						TFT_DrawString(s,point_x+gap +(b_w-24)/2, point_y+gap+(b_h - 48)/2,color_red);
						
//						if(key_tick =='#')
//						{
//							key_board.res = KEY_CANCEL;
//							Enable_Vitual_key(false);
//						}
//						else
							key_board.res = key_tick;
			}
		}
	}
}
