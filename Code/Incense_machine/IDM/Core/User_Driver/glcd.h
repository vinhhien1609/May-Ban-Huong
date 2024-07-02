
#ifndef GLCD_H
#define GLCD_H

#include "platform.h"
#include "glcd_font.h"
#include "main.h"

/* LCD Dimension */
#define GLCD_WIDTH      128
#define GLCD_HEIGHT     64

/* Color code */
#define WHITE   1
#define BLACK   0


typedef enum
{
	GLCD_LCD_PIN_CS,
	GLCD_LCD_PIN_CLK,
	GLCD_LCD_PIN_DATA,
} glcd_lcd_pin_t;


typedef void (*glcd_pin_write_cb_t)(glcd_lcd_pin_t pin, uint32_t value);
typedef void (*glcd_protect_cb_t)(uint32_t is_lock, uint32_t timeout_ms);


/**
 * @brief		Initialize GLCD driver
 * @param[in]	pin_write_cb Function pointer write data to pin
 */
void GLcd_Init(glcd_pin_write_cb_t pin_write_cb, glcd_protect_cb_t lock_cb);
void glcd_lcd_write_pin(glcd_lcd_pin_t pin, uint32_t output);
void glcd_lcd_write_pin(glcd_lcd_pin_t pin, uint32_t output);
static void glcd_lcd_lock(uint32_t is_lock, uint32_t timeout);
void GLcd_ClearScreen(int32_t color);
void GLcd_SetFont(font_t* font);
void *GLcd_GetFont(void);
void GLcd_DrawPoint(int32_t x, int32_t y, int32_t color);
void GLcd_DrawChar(char c, int32_t x, int32_t y, int32_t color);
void GLcd_DrawCharUni(const uint8_t* image, int32_t x, int32_t y, uint8_t w, uint8_t h, int32_t color);
void GLcd_DrawString(const char* str, int32_t x, int32_t y, int32_t color);
void GLcd_DrawStringUni(const char* s, int32_t x1, int32_t y1, int32_t color);
void GLcd_DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color);
void GLcd_DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color);
void GLcd_DrawCircle(int32_t x, int32_t y, int32_t radius, int32_t color);
void GLcd_FillRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color);
void GLcd_DrawImage(const uint8_t* img);
void GLcd_DrawBitmap(const uint8_t *image, int32_t x, int32_t y);

void GLcd_DrawStringDef(const char *s, int32_t x, int32_t y, int32_t color);
void GLcd_DrawCharDef(const uint8_t *image, int32_t x, int32_t y, uint8_t w, uint8_t h, int32_t color);

/* Get the length in pixel of a string with the current font */
int32_t  GLcd_MeasureString(const char* str);
int32_t  GLcd_MeasureStringUni(const uint16_t* str);
int32_t  GLcd_CalculateXCenterStringUni(const uint16_t* s);
int32_t GLcd_CalculateXCenterStringNormal(const uint8_t* s);
/* Draw the offscreen buffer to the LCD
 * All of the above functions just do drawing on the offscreen buffer.
 * In order to draw on the LCD, user have to call this function after calling those above functions
 * This example will draw a string and a line on the LCD
 *     GLcd_DrawString("Hello", 0, 0, WHITE);   // Just draw on the offscreen buffer. The string does not appear on the LCD yet
 *     GLcd_DrawLine(10, 10, 50, 50, WHITE);    // Just draw on the offscreen buffer. The line does not appear on the LCD yet
 *     GLcd_Flush();    // Draw the above string and line on to the LCD
 */
void GLcd_Flush(void);
void GLcd_FlushRegion(int32_t y, int32_t height);

// Thread safe lock func
/**
 * @brief           Lock GLCD driver
 * @param[in]       ms Max wait timeout 
 */
void glcd_lock(uint32_t ms);

/**
 * @brief           Unlock GLCD driver
 */
void glcd_unlock(void);

#endif /* GLCD_H */
