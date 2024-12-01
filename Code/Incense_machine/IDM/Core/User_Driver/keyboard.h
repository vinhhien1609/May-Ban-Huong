
#ifndef KEY_BOARD_H
#define KEY_BOARD_H

#include "TFT_glcd.h"
#include <stdbool.h>
#include "touch.h"

#define ENTER	'*'
#define CANCEL '#'
#define UP	'8'
#define DOWN	'0'


typedef enum keyboardType{
	NUMBER_KEY = 0U,
	CHAR_KEY,
} Keyboard_Typedef;

typedef enum keyboardRespone{
	KEY_OK = 0U,
	KEY_CANCEL,
	KEY_NONE,
} KeyboardRes;

typedef struct _vitual_keyboard
{
  unsigned char str[50];
  KeyboardRes res;
	unsigned char act;
	Keyboard_Typedef type;
}VITUAL_KEYBOARD;




unsigned char scan_BT(void);
void scan_switch(void);
void scan_vitual_key(void);
unsigned char get_vitual_key();
void Enable_Vitual_key(bool act);
bool get_touch(touch_event_t t_ev, unsigned int x, unsigned y, unsigned int w, unsigned h);
touch_event_t get_touch_env(void);
struct _ts_event get_mouse(void);
#endif /* GLCD_H */
