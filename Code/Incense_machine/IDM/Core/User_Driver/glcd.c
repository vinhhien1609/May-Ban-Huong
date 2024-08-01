
#include "glcd.h"
//#include "FreeRTOS.h"
//#include "task.h"
#include "stm32f1xx_hal.h"
#include "glcd_font.h"
#include "main.h"
//#include "back_ground.h"
#include "stm32f1xx_hal_spi.h"
#include "fatfs.h"
#include "ff.h"

#define LCD_DISPLAY_CLEAR 0x01
#define LCD_FUNCTION_SET_BASIC_ALPHA 0x20
#define LCD_ENTRY_MODE_SET 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_DISPLAY_OFF 0x08
#define LCD_FUNCTION_SET_BASIC_GRAPHIC 0x22
#define LCD_FUNCTION_SET_EXTENDED_GRAPHIC 0x26
#define LCD_SET_GDRAM_ADDRESS 0x80

#define DELAY_GPIO_ACTION           1//50


static font_t *m_font;
static uint8_t m_off_buffer[(GLCD_WIDTH * GLCD_HEIGHT) / 8];

static void send_data(uint8_t data);
static void send_cmd(uint8_t command);
static void send_lcd(uint8_t data1, uint8_t data2);
static void send_slow(uint8_t data);
static void cmd_delay(void);
static void goto_extended_mode(void);
static void goto_basic_mode(void);
static void set_graphics_address(uint32_t r, uint32_t c);

static glcd_pin_write_cb_t m_write_cb = 0;
static glcd_protect_cb_t m_lock = 0;

extern SPI_HandleTypeDef hspi2;

#ifdef  LCD_MONO

#endif




/**
 * @brief Ham cau hinh chan va khoi tao cho man h�nh LCD graphic 128x64
 * 
 * @param dataPin -> Chan truyen du lieu toi GLCD chuan noi tiep   
 * @param clockPin -> Chan clock  
 * @param csPin -> Chan cho phep hoat dong  
 * @retval NONE
 */

void glcd_lcd_write_pin(glcd_lcd_pin_t pin, uint32_t output)
{
	GPIO_PinState outpin;
    if (output)
    {
        outpin = GPIO_PIN_SET;
    }
    else
    {
        outpin = GPIO_PIN_RESET;
    }

    switch (pin)
    {
    case GLCD_LCD_PIN_CS:
				HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, outpin);
        break;

//    case GLCD_LCD_PIN_CLK:
//        HAL_GPIO_WritePin(LCD_CLK_GPIO_Port, LCD_CLK_Pin, outpin);
//        break;

//    case GLCD_LCD_PIN_DATA:
//        HAL_GPIO_WritePin(LCD_DATA_GPIO_Port, LCD_DATA_Pin, outpin);
//        break;

    default:
        break;
    }
}

#ifdef  LCD_MONO
void GLcd_Init(glcd_pin_write_cb_t write_callback, glcd_protect_cb_t lock_cb)
{
	
	 
	m_write_cb = write_callback;
    m_lock = lock_cb;

//    m_write_cb(GLCD_LCD_PIN_CLK, 0);
//    m_write_cb(GLCD_LCD_PIN_DATA, 0);
    m_write_cb(GLCD_LCD_PIN_CS, 1);

    send_cmd(LCD_FUNCTION_SET_BASIC_ALPHA); /* Set dia chi DDRAM ve 0x00 va dua con tro ve goc */
//		cmd_delay();
    HAL_Delay(1);
    send_cmd(LCD_FUNCTION_SET_BASIC_ALPHA); /* Set dia chi DDRAM ve 0x00 va dua con tro ve goc */
    HAL_Delay(1);
    send_cmd(LCD_ENTRY_MODE_SET); /* Set vi tri con tro va hien thi dich chuyen khi doc ghi*/
    HAL_Delay(1);
    goto_basic_mode(); /* Dua GLCD ve che do Basic */
    HAL_Delay(1);
    send_cmd(LCD_DISPLAY_CLEAR); /* Xoa man hinh */
    HAL_Delay(50);
    goto_extended_mode(); /* Dua GLCD ve che do Extended */
    HAL_Delay(1);
    send_cmd(LCD_DISPLAY_ON); /* Bat che do hien thi (diplay, cursor, blink) */
    HAL_Delay(1);
    GLcd_ClearScreen(BLACK); /* Boi den man hinh trong mang m_off_buffer */
    GLcd_Flush();            /* Day du lieu vao GDRAM */
    HAL_Delay(50);
    send_cmd(0x01); /* Xoa man hinh */
    HAL_Delay(50);

    GLcd_SetFont(&font6x8); /* Cai dat font 6x8 */
}

void glcd_lock(uint32_t ms)
{
    if (m_lock)
    {
        m_lock(true, ms);
    }
}

void glcd_unlock(void)
{
    if (m_lock)
    {
        m_lock(false, 0);
    }
}


void GLcd_ClearScreen(int32_t color)
{
    GLcd_FillRect(0, 0, GLCD_WIDTH, GLCD_HEIGHT, color);
}

void GLcd_SetFont(font_t *font)
{
    m_font = font;
}

void *GLcd_GetFont(void)
{
    return m_font;
}

/**
 * @brief   Ham ve 1 diem tren man hinh graphic (Thao tac bit tren 1 byte thuoc m_off_buffer)
 * 
 * @param   int32_t x -> Toa do x diem hien thi 
 * @param   int32_t y -> Toa do y diem hien thi
 * @param   int32_t color -> Lua chon mau hien thi (theo define)          
 * @retval  
 */

void GLcd_DrawPoint(int32_t x, int32_t y, int32_t color)
{
    if ((x >= 0) && (x < GLCD_WIDTH) && (y >= 0) && (y < GLCD_HEIGHT))
    {
        uint8_t *p = m_off_buffer + ((y * (GLCD_WIDTH / 8)) + (x / 8)); /*m_off_buffer Bo dem duoc su dung lu du lieu cua toan man hinh Graphic truoc khi truyen toi GLCD */
        uint8_t mask = 0x80u >> (x % 8);

        if (color == BLACK)
        {
            *p &= ~mask;
        }
        else
        {
            *p |= mask;
        }
    }
}
/**
 * @brief Ham ve mot ki tu ra man hinh           
 * 
 * @param char c -> Ki tu can hien thi  
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi          
 * @retval NONE  
 */
void GLcd_DrawChar(char c, int32_t x, int32_t y, int32_t color)
{
    uint8_t i, j, k;
    /*Lay so byte tren mot hang, neu chia het cho 8 = ket qua phep chia, khong chia het 8 thi bang 1*/
    uint8_t bytes_per_row = m_font->width % 8 == 0 ? m_font->width / 8 : m_font->width / 8 + 1;
    /* Bien template*/
    uint32_t tmp = 0;
    /*Lay byte dau tien cua ki tu can hien thi
      Vidu: Ki tu '1' = 0x31 (Hex) voi font 6x8, heght = 8, startCharacter = ' ' = 0x20, bytes_per_row = 1;
      p = (uint8_t*)(m_font->content) + 8*(0x31 - 0x20)*1;
      p = uint8_t*)(m_font->content) + 136 = byte dau tien cua trong mang cua ki tu '1' = 0x04;*/
    uint8_t *p = (uint8_t *)(m_font->content) + m_font->height * (c - m_font->startCharacter) * bytes_per_row;

    uint8_t tmp_y = x + m_font->width - 1; /* Diem cuoi cung ben phai cua mot hang, cach quet tu phai -> trai */

    for (i = 0; i < m_font->height; i++) /* Quet toi toan bo chieu cao cua ki tu*/
    {
        for (k = 0; k < bytes_per_row; k++) /* Gui theo so byte cua ki tu tren 1 hang*/
        {
            tmp <<= 8; /* Dua tung byte trong mang vao tmp, toi da 4 byte*/
            tmp |= *(p + bytes_per_row - 1 - k);
        }
        p += bytes_per_row;
        for (j = 0; j < m_font->width; j++) /* Quet de ve tung diem (dot) tren mot hang */
        {
            if (CHECKBIT(tmp, j)) /* Kiem tra tung bit trong tmp */
            {
               GLcd_DrawPoint(tmp_y - j, y + i, color); /* Neu = 1 thi ve dot len man hinh*/
            }
						else
							GLcd_DrawPoint(tmp_y - j, y + i, BLACK); /* Neu = 1 thi ve dot len man hinh*/
        }
    }
}
/**
 * @brief Ham ve mot ki tu tieng viet ra man h�nh(Unicode)<=> quet theo cot           
 * 
 * @param uint8_t* image -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawCharUni(const uint8_t *image, int32_t x, int32_t y, uint8_t w, uint8_t h, int32_t color)
{
    for (uint16_t i = 0; i < w; i++) /* Quet den het do rong ki tu (cot) */
    {
        for (uint16_t j = 0; j < h; j++) /* Quet den het chieu cao ki tu (hang) */
        {
            if (READBIT(*(image + i + (j / 8) * w), j % 8)) /* Lay byte dau tien ghep voi byte thu w trong trong mang cua ki tu */
            {                                               /* Kiem tra tung bit trong byte, neu bang 1 thi chuyen mau diem hien thi */
                GLcd_DrawPoint(x + i, y + j, color);
            }
						else
							GLcd_DrawPoint(x + i, y + j, BLACK);
        }
    }
}
/**
 * @brief Ham ve mot ki tu tieng viet ra man h�nh(Unicode)<=> quet theo cot           
 * 
 * @param uint8_t* image -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawCharDef(const uint8_t *image, int32_t x, int32_t y, uint8_t w, uint8_t h, int32_t color)
{
    for (uint16_t i = 0; i < h; i++) /* Quet den het do cao ki tu (cot) */
    {
			int nbyte = (w-1)/8 +1;
        for (uint16_t j = 0; j < w; j++) /* Quet den het rong ki tu (hang) */
        {
            if (READBIT(*(image + i*nbyte + j/8), 7-(j % 8))) /* Lay byte dau tien ghep voi byte thu w trong trong mang cua ki tu */
            {                                               /* Kiem tra tung bit trong byte, neu bang 1 thi chuyen mau diem hien thi */
                GLcd_DrawPoint(x + j, y + i, color);
            }
        }
    }
}

/**
 * @brief Ham ve mot chuoi ki tu ra man hinh           
 * 
 * @param const char* str -> Chuoi ki tu can hien thi  
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param int32_t color -> Mau hien thi         
 * @retval NONE  
 */
void GLcd_DrawString(const char *str, int32_t x, int32_t y, int32_t color)
{
    while (*str) /* Quet den khi het ki tu */
    {
        GLcd_DrawChar(*str, x, y, color); /* Xuat tung ki tu ra man hinh */
        if (*str == ' ')                  /* Neu ki tu la dau cach, bo qua 4 cot */
        {
            x = x + 4;
        }
        else /* Neu la cac ki tu khac */
        {
            if (m_font->width > 20) /* Neu do lon ki tu > 20 */
            {
                x += m_font->width + 3; /* Toa do x cua ki tu tiep theo tang theo do rong ki tu + 3 */
            }
            else /* Neu do lon ki tu < 20 */
            {
                x += m_font->width; /* Toa do x cua ki tu tiep theo tang theo do rong ki tu */
            }
        }
        str++; /* Chuyen toi ki tu tiep theo */
    }
}
/**
 * @brief Ham ve mot chuoi ki tu tieng viet ra man h�nh (Unicode)          
 * 
 * @param const uint16_t* s -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawStringUni(const char *s, int32_t x, int32_t y, int32_t color)
{
    while (*s != 0) /* Quet den het ki tu */
    {
        if (*s != ' ') /* Neu khong phai la ki tu dau cach */
        {
            uint32_t char_pos = vnfontFindPosition(*s); /* Tra ve vi tri bat dau cua ki tu trong man Vnfont */

            /* Truyen byte thu 2 va byte thu w+1 cua ki tu vao ham ve ra man hinh */
            GLcd_DrawCharUni(&(g_vnfont_8x15[char_pos + 1]), x, y, g_vnfont_8x15[char_pos], 15, color);
            /* Lay do lon w cua ki tu la byte dau tien */
            s++;                                  /* Toi ki tu tiep theo */
            x += g_vnfont_8x15[char_pos] + 1; /* Toa do x ki tu tiep theo tang theo do lon ki tu w */
        }
        else
        {
            s++;    /* Neu la ki tu dau cach */
            x += 3; /* Bo qua 3 cot */
        }
    }
}
/**
 * @brief Ham ve mot chuoi ki tu tieng viet ra man h�nh (Unicode)          
 * 
 * @param const uint16_t* s -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawStringDef(const char *s, int32_t x, int32_t y, int32_t color)
{
	int32_t r=x, c=y;
    while (*s != 0) /* Quet den het ki tu */
    {
			if(*s!='\n')
			{
				uint8_t char_pos = code_mapping_table[*s]; /* Tra ve vi tri bat dau cua ki tu trong man Vnfont */

				/* Truyen byte thu 2 va byte thu w+1 cua ki tu vao ham ve ra man hinh */
				GLcd_DrawCharDef(&(char_table[code_offset_table[char_pos]]), r, c, code_width_table[char_pos], heigh, color);
				/* Lay do lon w cua ki tu la byte dau tien */
				s++;                                  /* Toi ki tu tiep theo */
				r += code_width_table[char_pos] + 1; /* Toa do x ki tu tiep theo tang theo do lon ki tu w */
			}
			else
			{
					s++;
					c +=heigh +1;
					r=0;
			}
    }
}

/**
 * @brief Ham kiem tra do lon cua chuoi ki tu         
 * 
 * @param const char* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t px -> Do lon cua chuoi ki tu can kiem tra  
 */
int32_t GLcd_MeasureString(const char *str)
{
    int32_t px = 0;

    while (*str) /* Quet den het ki tu cua chuoi */
    {
        if (*str == ' ') /* Neu la ki tu dau cach, do lon chuoi tang them 4 */
        {
            px = px + 4;
        }
        else /* Neu la ki tu khac */
        {
            if (m_font->width > 30) /* Neu do lon ki tu > 30, do lon chuoi tang them = do lon ki tu + 3 */
            {
                px += m_font->width + 3;
            }
            else /* Neu do lon ki tu <= 30, do lon chuoi tang them = do lon ki tu */
            {
                px += m_font->width;
            }
        }
        str++; /* Toi ki tu tiep theo */
    }

    return px; /* Tra ket qua do lon chuoi sau khi het ki tu */
}
/**
 * @brief Ham kiem tra do lon cua chuoi ki tu (Voi ki tu tieng viet (Unicode)         
 * 
 * @param const uint16_t* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t px -> Do lon cua chuoi ki tu can kiem tra  
 */

int32_t GLcd_MeasureStringUni(const uint16_t *str)
{
    int32_t px = 0;
    while (*str != 0)
    {
        if (*str != ' ')
        {
            uint32_t char_pos = vnfontFindPosition(*str);
            px += g_vnfont_8x15[char_pos] + 1;
            str++;
        }
        else
        {
            str++;
            px += 3;
        }
    }
    return px;
}
/**
 * @brief Toa do vi tri giua cua chuoi ki tu unicode     
 * 
 * @param const uint16_t* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t -> Toa do vi tri giua cua chuoi ki tu  
 */
int32_t GLcd_CalculateXCenterStringUni(const uint16_t *s)
{
    return (GLCD_WIDTH - GLcd_MeasureStringUni(s)) / 2;
}

/**
 * @brief Toa do vi tri giua cua chuoi ki tu         
 * 
 * @param const uint16_t* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t -> Toa do vi tri giua cua chuoi ki tu  
 */
int32_t GLcd_CalculateXCenterStringNormal(const uint8_t *s)
{
    return (GLCD_WIDTH - GLcd_MeasureString((const char*)s)) / 2;
}

/**
 * @brief Ham ve mot duong thang len man hinh         
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t x1 -> Toa do x diem ket thuc
 * @param int32_t y1 -> Toa do y diem ket thuc
 * @param int32_t color -> Mau sac hien thi                
 * @retval NONE 
 */
void GLcd_DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color)
{
    int32_t delta_x = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t delta_y = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int32_t step_x, step_y, error1, error2;

    step_x = (x0 < x1) ? 1 : (-1);
    step_y = (y0 < y1) ? 1 : (-1);

    error1 = delta_x - delta_y;
    while (1)
    {
        GLcd_DrawPoint(x0, y0, color);
        if ((x0 == x1) && (y0 == y1))
        {
            break;
        }
        error2 = 2 * error1;
        if (error2 > -delta_y)
        {
            error1 = error1 - delta_y;
            x0 = x0 + step_x;
        }
        if (error2 < delta_x)
        {
            error1 = error1 + delta_x;
            y0 = y0 + step_y;
        }
    }
}

/**
 * @brief Ham ve mot hinh chu nhat len man hinh         
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t width -> Do lon hinh 
 * @param int32_t height -> Chieu cao hinh
 * @param int32_t color -> Mau sac hien thi                
 * @retval NONE 
 */
void GLcd_DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color)
{
    uint8_t current_value = 0;
    /* Draw the two horizontal lines */
    for (current_value = 0; current_value < width + 1; current_value++)
    {
        GLcd_DrawPoint(x + current_value, y, color);
        GLcd_DrawPoint(x + current_value, y + height, color);
    }

    /* draw the two vertical lines */
    for (current_value = 0; current_value < height + 1; current_value++)
    {
        GLcd_DrawPoint(x, y + current_value, color);
        GLcd_DrawPoint(x + width, y + current_value, color);
    }
}

/**
 * @brief Ham ve mot hinh tron len man hinh         
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t int32_t radius -> Ban kinh hinh tron
 * @param int32_t color -> Mau sac hien thi                
 * @retval NONE 
 */
void GLcd_DrawCircle(int32_t x, int32_t y, int32_t radius, int32_t color)
{
    int32_t sw = 0, tmp_y = 0, tmp_x = 0;
    uint8_t d;

    d = y - x;
    tmp_y = radius;
    sw = 3 - 2 * radius;
    while (tmp_x <= tmp_y)
    {
        GLcd_DrawPoint(x + tmp_x, y + tmp_y, color);
        GLcd_DrawPoint(x + tmp_x, y - tmp_y, color);

        GLcd_DrawPoint(x - tmp_x, y + tmp_y, color);
        GLcd_DrawPoint(x - tmp_x, y - tmp_y, color);

        GLcd_DrawPoint(y + tmp_y - d, y + tmp_x, color);
        GLcd_DrawPoint(y + tmp_y - d, y - tmp_x, color);
        GLcd_DrawPoint(y - tmp_y - d, y + tmp_x, color);
        GLcd_DrawPoint(y - tmp_y - d, y - tmp_x, color);

        if (sw < 0)
            sw += (4 * tmp_x + 6);
        else
        {
            sw += (4 * (tmp_x - tmp_y) + 10);
            tmp_y--;
        }

        tmp_x++;
    }
}

/**
 * @brief Ham hien thi boi den mot hinh vuong hoac chu nhat      
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t width -> Do rong cua hinh
 * @param int32_t height -> Chieu cao cua hinh
 * @param int32_t color -> Mau sac cua hinh                 
 * @retval NONE 
 */
void GLcd_FillRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color)
{
    uint8_t i, j;
    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
            GLcd_DrawPoint(i, j, color);
    }
}
/**
 * @brief Ham ve mot anh len toan man hinh    
 * 
 * @param const uint8_t* img -> Mang chua anh can ve                 
 * @retval NONE 
 */

void GLcd_DrawImage(const uint8_t *img)
{
    int32_t x = 0, y = 0, b = 0;
    int32_t index = 0;

    for (y = 0; y < 8; y++) /* Quet lan luot 8 block, moi block la 128x8 */
    {
        for (x = 0; x < 128; x++) /* Quet tong 128 cot */
        {
            for (b = 0; b < 8; b++) /* Quet theo cot, moi cot 8 bit */
            {
                index = y * 128 + x;
                if (CHECKBIT(img[index], b)) /* Kiem tra neu bit bang 1 */
                {
                    GLcd_DrawPoint(x, y * 8 + b, WHITE); /* Ve diem tren man hinh mau trang */
                }
                else
                {
                    GLcd_DrawPoint(x, y * 8 + b, BLACK); /* Ve diem tren man hinh mau den */
                }
            }
        }
    }
}

/**
 * @brief Ham ve mot anh tai toa do bat ki   
 * 
 * @param const uint8_t *image -> Mang chua anh can ve 
 * @param int32_t x -> Toa do x diem bat dau 
 * @param int32_t y -> Toa do y diem bat dau                
 * @retval NONE 
 */

void GLcd_DrawBitmap(const uint8_t *image, int32_t x, int32_t y)
{
    int32_t w = *image++; /* Lay do rong cua anh */
    int32_t h = *image++; /* Lay chieu cao cua anh */

    for (uint16_t i = 0; i < w; i++)
    {
        for (uint16_t j = 0; j < h; j++)
        {
            if (READBIT(*(image + i + (j / 8) * w), j % 8))
            {
                GLcd_DrawPoint(x + i, y + j, WHITE);
            }
						else
							GLcd_DrawPoint(x + i, y + j, BLACK);
        }
    }
}

/**
 * @brief Ham day du lieu vao GDRAM cua toan man hinh  
 * 
 * @param m_off_buffer -> Mang da duoc truyen du lieu tu cac ham phia tren 
 * @param GLCD_WIDTH -> Do lon man hinh = 128
 * @param GLCD_HEIGHT -> Chieu cao man hinh = 64             
 * @retval NONE 
 */

void GLcd_Flush(void)
{
    for (uint8_t r = 0; r < GLCD_HEIGHT; ++r)
    {
        uint8_t end_col_num = (GLCD_WIDTH + 15) / 16;
        set_graphics_address(r, 0);
        uint8_t *ptr = m_off_buffer + (16 * r);
        for (uint8_t i = 0; i < end_col_num; ++i)
        {
            send_data(*ptr++);
            send_data(*ptr++);
        }
    }
}
/**
 * @brief Ham day du lieu vao GDRAM tu hang bat ky  
 * 
 * @param m_off_buffer -> Mang da duoc truyen du lieu tu cac ham phia tren 
 * @param int32_t y -> Toa do cua hang bat ki
 * @param int32_t height -> Chieu cao cua phan hien thi           
 * @retval NONE 
 */
void GLcd_FlushRegion(int32_t y, int32_t height)
{
    if (y < 0 || y >= GLCD_HEIGHT)
    {
        return;
    }

    for (uint8_t r = y; r < y + height; ++r)
    {
        uint8_t end_col_num = (GLCD_WIDTH + 15) / 16;
        set_graphics_address(r, 0);
        uint8_t *ptr = m_off_buffer + (16 * r);
        for (uint8_t i = 0; i < end_col_num; ++i)
        {
            send_data(*ptr++);
            send_data(*ptr++);
        }
    }
}

/***********PRIVATE FUNCTIONS*******************/

/**
 * @brief Ham day du lieu vao man hinh GLCD
 * 
 * @param uint8_t data -> Du lieu 8 bit can truyen     
 * @retval NONE 
 */
static void send_data(uint8_t data)
{
    send_lcd(0xFA, data); /* Byte dau tien = 0xFA -> Dat lenh ghi toi RAM */
}
/**
 * @brief Ham day command vao man hinh GLCD
 * 
 * @param uint8_t command -> Byte command 
 * @retval NONE 
 */
static void send_cmd(uint8_t command)
{
    send_lcd(0xF8, command); /* Byte dau tien = 0xF8 -> Dat lenh ghi Command */
}

static void send_lcd(uint8_t data1, uint8_t data2)
{
    send_slow(data1);        /* Byte dat lenh */
    send_slow(data2 & 0xF0); /* Byte truyen 4 bit cao cua du lieu*/
    send_slow(data2 << 4);   /* Byte truyen 4 bit thap cua du lieu*/
}

static void send_slow(uint8_t data)
{
	unsigned char s_data[2];
	s_data[0] = data;
		HAL_SPI_Transmit(&hspi2, s_data, 1, 20);
	
//	
//    for (uint8_t i = 0; i < 8; ++i)
//    {
//        /* Set value to DATA Pin. Because the Pin_Write function takes time to execute, we have to 
//           access directly to the registers to speed up the interface */
//        if (data & 0x80)
//        {
////            g_dataPort->PSOR |= (1 << g_dataPinInPort); // DATA Pin = 1
//            m_write_cb(GLCD_LCD_PIN_DATA, 1);
//        }
//        else
//        {
//        	m_write_cb(GLCD_LCD_PIN_DATA, 0); // DATA Pin = 0
//        }

//        /* Set CLOCK Pin to 1. Because the Pin_Write function takes time to execute, we have to 
//           access directly to the registers to speed up the interface */
////        g_clockPort->PSOR |= (1 << g_clockPinInPort);
//        m_write_cb(GLCD_LCD_PIN_CLK, 1);
//        for (volatile uint32_t i = 0; i < DELAY_GPIO_ACTION; i++)
//        {

//        }
////        g_clockPort->PCOR |= (1 << g_clockPinInPort);
//        m_write_cb(GLCD_LCD_PIN_CLK, 0);
//        for (volatile uint32_t i = 0; i < DELAY_GPIO_ACTION; i++)
//        {

//        }

//        data <<= 1;
//    }
}

static void cmd_delay()		// not importance function
{
	volatile uint32_t i = 0x07F;
	while (i--);
}

static void goto_extended_mode(void)
{
    send_cmd(LCD_FUNCTION_SET_EXTENDED_GRAPHIC);
    cmd_delay();
}

static void goto_basic_mode(void)
{
    send_cmd(LCD_FUNCTION_SET_BASIC_ALPHA);
    cmd_delay();
}
/**
 * @brief Set dia chi trong GDRAM
 * 
 * @param uint32_t r -> Dia chi hang 
 * @param uint32_t c -> Dia chi cot
 * @retval NONE 
 */

static void set_graphics_address(uint32_t r, uint32_t c)
{
    send_cmd(LCD_SET_GDRAM_ADDRESS | (r & 31));
    cmd_delay();
    send_cmd(LCD_SET_GDRAM_ADDRESS | c | ((r & 32) >> 2));
    cmd_delay();
}
#endif
///////////////////////////////////////////////////////////////////////////
#ifdef	LCD_COLOR	//LCD color

//*********4_wire_SPI****************************

uint32_t LCD_Index2Color_M332(int Index) {
  unsigned r, g, b;
  b = (Index & 3) * 85;
  g = ((Index >> 2) & 7) * 255 / 7;
  r = ((Index >> 5) & 7) * 255 / 7;
  return r + (g << 8) + (((uint32_t)b) << 16);
}
unsigned LCD_Color2Index_M332(LCD_COLOR Color) {
  unsigned r, g, b;
  r = Color & 255;
  g = (Color >> 8 ) & 255;
  b = Color >> 16;
  r = (r * 7 + 127) / 255;
  g = (g * 7 + 127) / 255;
  b = (b + 42) / 85;
  return b + (g << 2) + (r << 5);
}

#define B_BITS 5
#define G_BITS 6
#define R_BITS 5

#define R_MASK ((1 << R_BITS) -1)
#define G_MASK ((1 << G_BITS) -1)
#define B_MASK ((1 << B_BITS) -1)

unsigned LCD_Color2Index_M565(LCD_COLOR Color) {
  int r,g,b;
  r = (Color>> (8  - R_BITS)) & R_MASK;
  g = (Color>> (16 - G_BITS)) & G_MASK;
  b = (Color>> (24 - B_BITS)) & B_MASK;
  return b + (g << B_BITS) + (r << (G_BITS + B_BITS));
}

void LCD_CmdWrite(uint8_t cmd)
{ 
	//HAL_SPI_Init(&hspi2);
	 uint8_t  buff[2];
   buff[0] = 0x80;
   buff[1] = cmd;	
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
   //SCK = 1; 
   //SDA = 1; 
   //ZCS = 0;   
  // HAL_Delay(1);
	
	 HAL_SPI_Transmit(&hspi2, &buff[0], 2, 0x2);
//	 HAL_SPI_Transmit(&hspi2, &buff[1], 1, 0x1);
	 //HAL_SPI_Transmit(&hspi1, &cmd, 1, 0x1);
   //SPI_Write(0x80); 
   //SPI_Write(cmd);
	
  // HAL_Delay(1);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
//	 HAL_SPI_DeInit(&hspi1);
	//ZCS = 1;
}
 
//----------------------//

void LCD_DataWrite(uint8_t Data)
{ 
	 //HAL_SPI_Init(&hspi2);
	 uint8_t  buff[2];
   buff[0] = 0x00;
   buff[1] = Data;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 2, 20);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
}
void LCD_ColorWrite(uint16_t _color)
{
	 uint8_t data[2];
		data[0] = (_color >>8)&0xFF;
		data[1] = _color &0xFF;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	 HAL_SPI_Transmit(&hspi2, data, 2, 20);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);	
}
 
// void LCD_DataWriteM(uint8_t * pData, int NumWords)
//{ 
//	 uint8_t  buff[2], Data, Data1;
//	 Data =( *pData >> 8);
//	 Data1 = ((*(pData++)) & 0xFF);
//   buff[0] = 0x80;
//	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
////		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
////	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
//	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 0x1);

// while(NumWords--)
// {
//	 HAL_SPI_Transmit(&hspi2, &Data, sizeof(Data), 0x1);
//   HAL_SPI_Transmit(&hspi2, &Data1, sizeof(Data1), 0x1);  
// }
//	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
//}

 void LCD_DataWriteM(uint8_t * pData, int NumWords)
{ 
	 uint8_t  buff[2], Data, Data1;
	 Data =( *pData >> 8);
	 Data1 = ((*(pData++)) & 0xFF);
   buff[0] = 0x80;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 0x1);

 while(NumWords--)
 {
	 HAL_SPI_Transmit(&hspi2, &Data, sizeof(Data), 0x1);
   HAL_SPI_Transmit(&hspi2, &Data1, sizeof(Data1), 0x1);  
 }
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
}


//----------------------//
uint8_t  LCD_DataRead(void)
{
	
	//HAL_SPI_Init(&hspi1);
	 uint8_t  Data;
	 uint8_t buff[2]; 
	 buff[0] = 0x40;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
   HAL_Delay(1);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 2);
   
	 HAL_SPI_Receive(&hspi2, &Data, sizeof(Data), 0x1);
 	
//   HAL_Delay(1);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
	// HAL_SPI_DeInit(&hspi1);
	
  /*uchar Data;
   SCK = 1; 
   SDA = 1; 
   ZCS = 0;
   HAL_Delay(1);
   SPI_Write(0x40);  
   Data = SPI_Read();
   HAL_Delay(1);
   ZCS = 1;
 */
 return Data;
}  
//-----------------------//
uint8_t  LCD_StatusRead(void)
{
	
	//HAL_SPI_Init(&hspi1);
	 uint8_t  Data;
	 uint8_t buff[2]; 
	 buff[0] = 0xc0;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
//   HAL_Delay(1);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 0x1);
   
	 HAL_SPI_Receive(&hspi2, &Data, sizeof(Data), 0x1);
 	
//   HAL_Delay(1);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
	// HAL_SPI_DeInit(&hspi1);
	/*
  	uint8_t  Data;
 
   SCK = 1; 
   SDA = 1; 
   ZCS = 0;
   HAL_Delay(1);
   SPI_Write(0xc0); 
   Data = SPI_Read();
   HAL_Delay(1);
   ZCS = 1;
	 */
 return Data;
}

//REG[20h]

void One_Layer(void)
{
	uint8_t temp;
	LCD_CmdWrite(0x20);//DPCR
	temp = LCD_DataRead();
	temp &= 0x7F ;
	LCD_DataWrite(temp);
}	
				 
void Two_Layers(void)
{	uint8_t temp;
	LCD_CmdWrite(0x20);//DPCR
	temp = LCD_DataRead();
	temp |= 0x80 ;
	LCD_DataWrite(temp);
}

void  Draw_Circle(uint8_t X,uint8_t Y,uint8_t R)
{
	char temp;
    
	temp=X;   
    LCD_CmdWrite(0x99);
	LCD_DataWrite(temp);
	temp=X>>8;   
    LCD_CmdWrite(0x9a);	   
	LCD_DataWrite(temp);  
	  
	temp=Y;   
    LCD_CmdWrite(0x9b);
	LCD_DataWrite(temp);
	temp=Y>>8;   
    LCD_CmdWrite(0x9c);	   
	LCD_DataWrite(temp);

	temp=R;   
    LCD_CmdWrite(0x9d);
	LCD_DataWrite(temp);
}

//--------------------------------------------//
void Text_Background_Color(uint8_t setR,uint8_t setG,uint8_t setB)
{
    LCD_CmdWrite(0x60);//BGCR0
	LCD_DataWrite(setR);
   
    LCD_CmdWrite(0x61);//BGCR0
	LCD_DataWrite(setG);

    LCD_CmdWrite(0x62);//BGCR0
	LCD_DataWrite(setB);
} 

void Text_Foreground_Color(uint8_t setR,uint8_t setG,uint8_t setB)
{	    
    LCD_CmdWrite(0x63);//BGCR0
	LCD_DataWrite(setR);
   
    LCD_CmdWrite(0x64);//BGCR0
	LCD_DataWrite(setG);

    LCD_CmdWrite(0x65);//BGCR0
	LCD_DataWrite(setB);

}

void LCD_PLL_ini(void)
{
    LCD_CmdWrite(0x88);      
    LCD_DataWrite(0x0b);
    HAL_Delay(1);    
    LCD_CmdWrite(0x89);        
    LCD_DataWrite(0x02);  
    HAL_Delay(1);
}

void Text_Mode(void)
{	uint8_t temp;
	LCD_CmdWrite(0x40);//MWCR0
	temp = LCD_DataRead();
	temp |= 0x7F ;
	LCD_DataWrite(temp);	 
}

void Graphic_Mode(void)
{	uint8_t temp;
	LCD_CmdWrite(0x40);//MWCR0
	temp = LCD_DataRead();
	temp &= 0x80 ;
	temp |=0x04;		// R--> L, T--> D
	LCD_DataWrite(temp);	 
}

void Chk_Busy(void)
{
	uint8_t temp; 	
	do
	{
	temp=LCD_StatusRead();
	}while((temp&0x80)==0x80);
}

void Show_String(uint8_t *str, uint8_t n)
{   Text_Mode();
	LCD_CmdWrite(0x02);
	while(*str != '\0')
	{
	 LCD_DataWrite(*str);
	 ++str;	 	
	 Chk_Busy();
     //Chk_Wait();		
	}
   HAL_Delay(n);
}

//--------------------------------------------//
//REG[46h]~REG[49h]
void XY_Coordinate(uint16_t X,uint16_t Y)
{
    LCD_CmdWrite(0x46);
	LCD_DataWrite(X);  
    LCD_CmdWrite(0x47);	   
	LCD_DataWrite(X>>8);
 
    LCD_CmdWrite(0x48);
	LCD_DataWrite(Y);  
    LCD_CmdWrite(0x49);	   
	LCD_DataWrite(Y>>8);
}


void DMA_Start_enable(void)
{  uint8_t temp;
  LCD_CmdWrite(0xBF);
  temp = LCD_DataRead();
  temp |= 0x01;
  LCD_DataWrite(temp);
}

//-------------------------------------------//
//REG[30h]~REG[37h]
void Active_Window(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB)
{
    //setting active window X
  
    LCD_CmdWrite(0x30);//HSAW0
	LCD_DataWrite(XL); 
    LCD_CmdWrite(0x31);//HSAW1	   
	LCD_DataWrite(XL>>8);

    LCD_CmdWrite(0x34);//HEAW0
	LCD_DataWrite(XR);
    LCD_CmdWrite(0x35);//HEAW1	   
	LCD_DataWrite(XR>>8);

    //setting active window Y
  
    LCD_CmdWrite(0x32);//VSAW0
	LCD_DataWrite(YT);  
    LCD_CmdWrite(0x33);//VSAW1	   
	LCD_DataWrite(YT>>8);
  
    LCD_CmdWrite(0x36);//VEAW0
	LCD_DataWrite(YB); 
    LCD_CmdWrite(0x37);//VEAW1	   
	LCD_DataWrite(YB>>8);
}


//---------------------------------------------//
//REG[B0]~REG[B3]
void DMA_Start_address_setting(long set_address)
{ 
  LCD_CmdWrite(0xB0);
  LCD_DataWrite(set_address);

  LCD_CmdWrite(0xB1);
  LCD_DataWrite(set_address>>8);

  LCD_CmdWrite(0xB2);
  LCD_DataWrite(set_address>>16);

  LCD_CmdWrite(0xB3);
  LCD_DataWrite(set_address>>24);

}

void DMA_block_mode_size_setting(uint16_t BWR,uint16_t BHR,uint16_t SPWR)
{
  LCD_CmdWrite(0xB4);
  LCD_DataWrite(BWR);
  LCD_CmdWrite(0xB5);
  LCD_DataWrite(BWR>>8);

  LCD_CmdWrite(0xB6);
  LCD_DataWrite(BHR);
  LCD_CmdWrite(0xB7);
  LCD_DataWrite(BHR>>8);

  LCD_CmdWrite(0xB8);
  LCD_DataWrite(SPWR);
  LCD_CmdWrite(0xB9);
  LCD_DataWrite(SPWR>>8);  
}

void Memory_Clear(void)
{
	uint8_t temp;
	LCD_CmdWrite(0x8e);//MCLR
	
	temp = LCD_DataRead();
	temp |= 0xC0;
	LCD_DataWrite(temp); 
	Chk_Busy();	
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_CmdWrite(0x46);
	LCD_DataWrite(Xpos);  
    LCD_CmdWrite(0x47);	   
	LCD_DataWrite(Xpos>>8);
    LCD_CmdWrite(0x48);
	LCD_DataWrite(Ypos);
    LCD_CmdWrite(0x49);	   
	LCD_DataWrite(Ypos>>8);
}	
void LCD_WriteRAM_Prepare(void)
{
  LCD_CmdWrite(0x02); //
}

void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;
  
  LCD_SetCursor(0,0); 

  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

  for(index = 0; index < 384000; index++)
  {
    LCD_DataWrite(Color);
		Chk_Busy();
  }

 // while(1);  
}

void Internal_CGROM(void)
{
	uint8_t temp;
	LCD_CmdWrite(0x21);//FNCR0
	temp = LCD_DataRead();
	temp &= 0xDF ;
}

void Clear_Active_Window(void)
{
	uint8_t temp;
	LCD_CmdWrite(0x8e);//MCLR
	temp = LCD_DataRead();
	temp |= 0x40 ;
	LCD_DataWrite(temp);  
}
//-------------------------------------------//
//REG[2Ah]REG[2Bh]REG[2Ch]REG[2Dh]
void Font_Coordinate(uint16_t X,uint16_t Y)
{
 
    LCD_CmdWrite(0x2A);//HOFS0
	LCD_DataWrite(X);

    LCD_CmdWrite(0x2B);//HOFS1	   
	LCD_DataWrite(X>>8);

 
    LCD_CmdWrite(0x2C);//VOFS0
	LCD_DataWrite(Y);

    LCD_CmdWrite(0x2D);//VOFS1	   
	LCD_DataWrite(Y>>8); 
}

//-------------------------------------------//
//REG[2Eh]
void Font_size_16x16_8x16(void) 
{
 	uint8_t temp;
	LCD_CmdWrite(0x2E);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x3f;
	LCD_DataWrite(temp);
}


typedef struct BmpFileHeader {
   char bfType[2];	// 2 byte
   unsigned int bfSize;	// 4byte
   unsigned short int __bfReserved1;	// 2 byte
   unsigned short int __bfReserved2;	//2 byte
   unsigned long int bfOffBits;	// 4 byte
}  __attribute__((packed)) BMPFILEHEADER;

typedef struct BmpImageHeader {
   unsigned int biSize;	//4 byte
   unsigned int biWidth;		// 4byte
   unsigned int biHeight;	//4 byte
   unsigned short int biPlanes;	//2byte
   unsigned short int biBitCount;	//2byte bit per pixel
   unsigned int biCompression;		//4
   unsigned int biSizeImage;			//4
   int biXPelsPerMeter;		//4
   int biYPelPerMeter;		//4
   unsigned int biClrUsed;	//4
   unsigned int biClrImportant;	//4  
}  __attribute__((packed)) BMPIMAGEHEADER;

	BMPFILEHEADER header;
  BMPIMAGEHEADER imageHeader;

	FATFS fs;
  FRESULT res;
	FIL myfile;
	DIR dir;
	FILINFO finfo;

void drawBITMAP(void)
{


	uint8_t BUFFER[128];
	uint32_t wr=0;
		// mount the default drive
		res = f_mount(&fs, "", 0);
		if(res != FR_OK) {
				printf("f_mount() failed, res = %d\r\n", res);
		}
		else
			printf("SD_mouted\r\n");
	if(BSP_SD_Init() == MSD_OK)
	{
		printf("SD_init_OK\r\n");
		f_opendir(&dir,"");
		res = f_findfirst(&dir, &finfo, "", "*.bmp");
//		for(int t=0; t<13; t++)
//			if(finfo.fname[t]==',' && finfo.fname[t]==',' && finfo.fname[t]=='P'
		while(finfo.fname[0] && res == FR_OK)
		{
			printf("start Read file: %s %d\r\n", finfo.fname, res);
			res =f_open(&myfile, finfo.fname, FA_READ|FA_WRITE);
			if(res==FR_OK)
			{
				f_read(&myfile,&header, sizeof(BMPFILEHEADER), &wr);
				f_read(&myfile,&imageHeader, sizeof(BMPIMAGEHEADER), &wr);
				printf("sFile: %c%c\r\n", header.bfType[0], header.bfType[1]);
				printf("size bitmap: %dx%d ---%d\r\n", imageHeader.biWidth, imageHeader.biHeight, imageHeader.biBitCount);
//				if(header.bfType[0]=='B' && header.bfType[1]=='M')
				{
					f_lseek(&myfile, header.bfOffBits);// - sizeof(BMPFILEHEADER) - sizeof(BMPIMAGEHEADER) + 54);
					
					Graphic_Mode();
					Active_Window(0,imageHeader.biWidth-1,0,imageHeader.biHeight-1);
					LCD_SetCursor(0,0); 
					LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
					
					uint8_t buff_image[100];
					for(int n=0; n<imageHeader.biHeight; n++)
					{
						//printf("Reading file....\r\n");
		//				LCD_SetCursor(0,n);
						for(int m=0; m<imageHeader.biWidth; m++)
						{
							f_read(&myfile,&buff_image[0], imageHeader.biBitCount/8, &wr);
							uint16_t color;// = buff_image[0];
							if(imageHeader.biBitCount==24)
								color = LCD_Color2Index_M332((uint32_t)(buff_image[0]<<16 | buff_image[1]<<8 | buff_image[2]));
							//color = LCD_Color2Index_M565((uint32_t)(buff_image[0]<<16 | buff_image[1]<<8 | buff_image[2]));
							//LCD_DataWrite(color);
							LCD_ColorWrite(color);
						}
					}
				}
			}
			f_close(&myfile );
			HAL_Delay(2000);
			res = f_findnext(&dir, &finfo);
		}
		f_closedir(&dir);
	}
		else
			printf("SD_init_FAIL\r\n");	
}

void draw_BG(void)
{
  uint32_t index = 0, n;
//	Active_Window(200,439,100,243);
////    DMA_Start_address_setting(768000);
////    DMA_block_mode_size_setting(800,480,800);
////    DMA_Start_enable();	
//  LCD_SetCursor(0,0); 
//  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
//  for(index = 0; index < 144; index++)
//  {
//		for(n=0; n<240; n++)
//		{
//    LCD_DataWrite(ac8bit[index *240 + n]);
//		Chk_Busy();
//		}
//  }
	drawBITMAP();
}


void test_lcd(void)
{
		Graphic_Mode();
    Active_Window(0,799,0,479);
		LCD_background(0x07E0);
		Memory_Clear();
		HAL_Delay(1000);
//	Text_Background_Color(0,0,255);
//		Memory_Clear();
//		HAL_Delay(1000);
//	Clear_Active_Window();
//	Text_Background_Color(0,0,0);
//	Memory_Clear();
//	Graphic_Mode();
	drawBITMAP();
//	LCD_Clear(0xFFF0);

}

void GLcd_Init(glcd_pin_write_cb_t write_callback, glcd_protect_cb_t lock_cb)
{
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);

	LCD_PLL_ini();
	LCD_CmdWrite(0x10);  //SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8bit MPU interface
    LCD_DataWrite(0x00000);   // if 8bit MCU interface   and 256 color display						    
//
;	LCD_DataWrite(0x0008);   // if 8bit MCU interface   and 65k color display		
//    LCD_DataWrite(0x0A); // if 16bit MCU interface   and 65k color display  
	HAL_Delay(1);
	
LCD_CmdWrite(0x04);  //PCLK inverse
 LCD_DataWrite(0x81);
 HAL_Delay(1);
 
 //Horizontal set
 LCD_CmdWrite(0x14); //HDWR//Horizontal Display Width Setting Bit[6:0]                      
 LCD_DataWrite(0x63);//Horizontal display width(pixels) = (HDWR + 1)*8                      
 LCD_CmdWrite(0x15);//Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)   
 LCD_DataWrite(0x00);//Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]               
 LCD_CmdWrite(0x16); //HNDR//Horizontal Non-Display Period Bit[4:0]                         
 LCD_DataWrite(0x03);//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8                
 LCD_CmdWrite(0x17); //HSTR//HSYNC Start Position[4:0]                                      
 LCD_DataWrite(0x03);//HSYNC Start Position(PCLK) = (HSTR + 1)*8                            
 LCD_CmdWrite(0x18); //HPWR//HSYNC Polarity ,The period width of HSYNC.                     
 LCD_DataWrite(0x0B);//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8    
 //Vertical set                             
 LCD_CmdWrite(0x19); //VDHR0 //Vertical Display Height Bit [7:0]                            
 LCD_DataWrite(0xdf);//Vertical pixels = VDHR + 1                                           
 LCD_CmdWrite(0x1a); //VDHR1 //Vertical Display Height Bit [8]                              
 LCD_DataWrite(0x01);//Vertical pixels = VDHR + 1                                           
 LCD_CmdWrite(0x1b); //VNDR0 //Vertical Non-Display Period Bit [7:0]                        
 LCD_DataWrite(0x20);//Vertical Non-Display area = (VNDR + 1)                               
 LCD_CmdWrite(0x1c); //VNDR1 //Vertical Non-Display Period Bit [8]                          
 LCD_DataWrite(0x00);//Vertical Non-Display area = (VNDR + 1)                               
 LCD_CmdWrite(0x1d); //VSTR0 //VSYNC Start Position[7:0]                                    
 LCD_DataWrite(0x16);//VSYNC Start Position(PCLK) = (VSTR + 1)                              
 LCD_CmdWrite(0x1e); //VSTR1 //VSYNC Start Position[8]                                      
 LCD_DataWrite(0x00);//VSYNC Start Position(PCLK) = (VSTR + 1)                              
 LCD_CmdWrite(0x1f); //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]                        
 LCD_DataWrite(0x01);//VSYNC Pulse Width(PCLK) = (VPWR + 1)                                 
  
  //Active window  set 
   //setting active window X
    LCD_CmdWrite(0x30); //Horizontal Start Point 0 of Active Window (HSAW0)
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [7:0]
    LCD_CmdWrite(0x31); //Horizontal Start Point 1 of Active Window (HSAW1)	   
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [9:8]
    LCD_CmdWrite(0x34); //Horizontal End Point 0 of Active Window (HEAW0)
    LCD_DataWrite(0x1F); //Horizontal End Point of Active Window [7:0]
    LCD_CmdWrite(0x35); //Horizontal End Point 1 of Active Window (HEAW1)	   
    LCD_DataWrite(0x03); //Horizontal End Point of Active Window [9:8]

   //setting active window Y
    LCD_CmdWrite(0x32); //Vertical Start Point 0 of Active Window (VSAW0)
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [7:0]
    LCD_CmdWrite(0x33); //Vertical Start Point 1 of Active Window (VSAW1)	   
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [8]
    LCD_CmdWrite(0x36); //Vertical End Point of Active Window 0 (VEAW0)
    LCD_DataWrite(0xdf); //Vertical End Point of Active Window [7:0]
    LCD_CmdWrite(0x37); //Vertical End Point of Active Window 1 (VEAW1)	   
    LCD_DataWrite(0x01); //Vertical End Point of Active Window [8]

		LCD_CmdWrite(0x20);
		LCD_DataWrite(0x80);
		
		LCD_background(0x07E0);
		
		LCD_CmdWrite(0x8a);//PWM setting
	  LCD_DataWrite(0x80);
		
		LCD_CmdWrite(0x8a);//PWM setting
		LCD_DataWrite(0x87);//open PWM
		
		LCD_CmdWrite(0x8b);//Backlight brightness setting
		LCD_DataWrite(32);//Brightness parameter 0xff-0x00
		
		Display_ON();
		Memory_Clear();
//		test_lcd();
}

void Display_ON(void)
{	
  LCD_CmdWrite(0x01);//PWRR
	LCD_DataWrite(0x80);
}

void Display_OFF(void)
{
	LCD_CmdWrite(0x01);//PWRR
	LCD_DataWrite(0x00);
}



//Cor de fundo com 65k colors
void LCD_background(uint16_t color)
{
	LCD_CmdWrite(0x60);
	LCD_DataWrite((color&0xf800)>>11);
  LCD_CmdWrite(0x61);
	LCD_DataWrite((color&0x07e0)>>5);
	LCD_CmdWrite(0x62);
	LCD_DataWrite((color&0x001f));
}

void GLcd_ClearScreen(int32_t color)
{
		LCD_background(color);
}



/**
 * @brief   Ham ve 1 diem tren man hinh graphic (Thao tac bit tren 1 byte thuoc m_off_buffer)
 * 
 * @param   int32_t x -> Toa do x diem hien thi 
 * @param   int32_t y -> Toa do y diem hien thi
 * @param   int32_t color -> Lua chon mau hien thi (theo define)          
 * @retval  
 */

void GLcd_DrawPoint(int32_t x, int32_t y, int32_t color)
{
    if ((x >= 0) && (x < GLCD_WIDTH) && (y >= 0) && (y < GLCD_HEIGHT))
    {
        uint8_t *p = m_off_buffer + ((y * (GLCD_WIDTH / 8)) + (x / 8)); /*m_off_buffer Bo dem duoc su dung lu du lieu cua toan man hinh Graphic truoc khi truyen toi GLCD */
        uint8_t mask = 0x80u >> (x % 8);

        if (color == BLACK)
        {
            *p &= ~mask;
        }
        else
        {
            *p |= mask;
        }
    }
}
/**
 * @brief Ham ve mot ki tu ra man hinh           
 * 
 * @param char c -> Ki tu can hien thi  
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi          
 * @retval NONE  
 */
void GLcd_DrawChar(char c, int32_t x, int32_t y, int32_t color)
{
    uint8_t i, j, k;
    /*Lay so byte tren mot hang, neu chia het cho 8 = ket qua phep chia, khong chia het 8 thi bang 1*/
    uint8_t bytes_per_row = m_font->width % 8 == 0 ? m_font->width / 8 : m_font->width / 8 + 1;
    /* Bien template*/
    uint32_t tmp = 0;
    /*Lay byte dau tien cua ki tu can hien thi
      Vidu: Ki tu '1' = 0x31 (Hex) voi font 6x8, heght = 8, startCharacter = ' ' = 0x20, bytes_per_row = 1;
      p = (uint8_t*)(m_font->content) + 8*(0x31 - 0x20)*1;
      p = uint8_t*)(m_font->content) + 136 = byte dau tien cua trong mang cua ki tu '1' = 0x04;*/
    uint8_t *p = (uint8_t *)(m_font->content) + m_font->height * (c - m_font->startCharacter) * bytes_per_row;

    uint8_t tmp_y = x + m_font->width - 1; /* Diem cuoi cung ben phai cua mot hang, cach quet tu phai -> trai */

    for (i = 0; i < m_font->height; i++) /* Quet toi toan bo chieu cao cua ki tu*/
    {
        for (k = 0; k < bytes_per_row; k++) /* Gui theo so byte cua ki tu tren 1 hang*/
        {
            tmp <<= 8; /* Dua tung byte trong mang vao tmp, toi da 4 byte*/
            tmp |= *(p + bytes_per_row - 1 - k);
        }
        p += bytes_per_row;
        for (j = 0; j < m_font->width; j++) /* Quet de ve tung diem (dot) tren mot hang */
        {
            if (CHECKBIT(tmp, j)) /* Kiem tra tung bit trong tmp */
            {
               GLcd_DrawPoint(tmp_y - j, y + i, color); /* Neu = 1 thi ve dot len man hinh*/
            }
						else
							GLcd_DrawPoint(tmp_y - j, y + i, BLACK); /* Neu = 1 thi ve dot len man hinh*/
        }
    }
}
/**
 * @brief Ham ve mot ki tu tieng viet ra man h�nh(Unicode)<=> quet theo cot           
 * 
 * @param uint8_t* image -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawCharUni(const uint8_t *image, int32_t x, int32_t y, uint8_t w, uint8_t h, int32_t color)
{
    for (uint16_t i = 0; i < w; i++) /* Quet den het do rong ki tu (cot) */
    {
        for (uint16_t j = 0; j < h; j++) /* Quet den het chieu cao ki tu (hang) */
        {
            if (READBIT(*(image + i + (j / 8) * w), j % 8)) /* Lay byte dau tien ghep voi byte thu w trong trong mang cua ki tu */
            {                                               /* Kiem tra tung bit trong byte, neu bang 1 thi chuyen mau diem hien thi */
                GLcd_DrawPoint(x + i, y + j, color);
            }
						else
							GLcd_DrawPoint(x + i, y + j, BLACK);
        }
    }
}
/**
 * @brief Ham ve mot ki tu tieng viet ra man h�nh(Unicode)<=> quet theo cot           
 * 
 * @param uint8_t* image -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawCharDef(const uint8_t *image, int32_t x, int32_t y, uint8_t w, uint8_t h, int32_t color)
{
    for (uint16_t i = 0; i < h; i++) /* Quet den het do cao ki tu (cot) */
    {
			int nbyte = (w-1)/8 +1;
        for (uint16_t j = 0; j < w; j++) /* Quet den het rong ki tu (hang) */
        {
            if (READBIT(*(image + i*nbyte + j/8), 7-(j % 8))) /* Lay byte dau tien ghep voi byte thu w trong trong mang cua ki tu */
            {                                               /* Kiem tra tung bit trong byte, neu bang 1 thi chuyen mau diem hien thi */
                GLcd_DrawPoint(x + j, y + i, color);
            }
        }
    }
}

/**
 * @brief Ham ve mot chuoi ki tu ra man hinh           
 * 
 * @param const char* str -> Chuoi ki tu can hien thi  
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param int32_t color -> Mau hien thi         
 * @retval NONE  
 */
void GLcd_DrawString(const char *str, int32_t x, int32_t y, int32_t color)
{
    while (*str) /* Quet den khi het ki tu */
    {
        GLcd_DrawChar(*str, x, y, color); /* Xuat tung ki tu ra man hinh */
        if (*str == ' ')                  /* Neu ki tu la dau cach, bo qua 4 cot */
        {
            x = x + 4;
        }
        else /* Neu la cac ki tu khac */
        {
            if (m_font->width > 20) /* Neu do lon ki tu > 20 */
            {
                x += m_font->width + 3; /* Toa do x cua ki tu tiep theo tang theo do rong ki tu + 3 */
            }
            else /* Neu do lon ki tu < 20 */
            {
                x += m_font->width; /* Toa do x cua ki tu tiep theo tang theo do rong ki tu */
            }
        }
        str++; /* Chuyen toi ki tu tiep theo */
    }
}
/**
 * @brief Ham ve mot chuoi ki tu tieng viet ra man h�nh (Unicode)          
 * 
 * @param const uint16_t* s -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawStringUni(const char *s, int32_t x, int32_t y, int32_t color)
{
    while (*s != 0) /* Quet den het ki tu */
    {
        if (*s != ' ') /* Neu khong phai la ki tu dau cach */
        {
            uint32_t char_pos = vnfontFindPosition(*s); /* Tra ve vi tri bat dau cua ki tu trong man Vnfont */

            /* Truyen byte thu 2 va byte thu w+1 cua ki tu vao ham ve ra man hinh */
            GLcd_DrawCharUni(&(g_vnfont_8x15[char_pos + 1]), x, y, g_vnfont_8x15[char_pos], 15, color);
            /* Lay do lon w cua ki tu la byte dau tien */
            s++;                                  /* Toi ki tu tiep theo */
            x += g_vnfont_8x15[char_pos] + 1; /* Toa do x ki tu tiep theo tang theo do lon ki tu w */
        }
        else
        {
            s++;    /* Neu la ki tu dau cach */
            x += 3; /* Bo qua 3 cot */
        }
    }
}
/**
 * @brief Ham ve mot chuoi ki tu tieng viet ra man h�nh (Unicode)          
 * 
 * @param const uint16_t* s -> Mang can hien thi 
 * @param int32_t x -> Toa do x diem bat dau can hien thi   
 * @param int32_t y -> Toa do y diem bat dau can hien thi 
 * @param uint8_t w -> Do rong cua anh can hien thi
 * @param uint8_t h -> Chieu cao cua anh can hien thi        
 * @retval NONE  
 */
void GLcd_DrawStringDef(const char *s, int32_t x, int32_t y, int32_t color)
{
	int32_t r=x, c=y;
    while (*s != 0) /* Quet den het ki tu */
    {
			if(*s!='\n')
			{
				uint8_t char_pos = code_mapping_table[*s]; /* Tra ve vi tri bat dau cua ki tu trong man Vnfont */

				/* Truyen byte thu 2 va byte thu w+1 cua ki tu vao ham ve ra man hinh */
				GLcd_DrawCharDef(&(char_table[code_offset_table[char_pos]]), r, c, code_width_table[char_pos], heigh, color);
				/* Lay do lon w cua ki tu la byte dau tien */
				s++;                                  /* Toi ki tu tiep theo */
				r += code_width_table[char_pos] + 1; /* Toa do x ki tu tiep theo tang theo do lon ki tu w */
			}
			else
			{
					s++;
					c +=heigh +1;
					r=0;
			}
    }
}

/**
 * @brief Ham kiem tra do lon cua chuoi ki tu         
 * 
 * @param const char* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t px -> Do lon cua chuoi ki tu can kiem tra  
 */
int32_t GLcd_MeasureString(const char *str)
{
    int32_t px = 0;

    while (*str) /* Quet den het ki tu cua chuoi */
    {
        if (*str == ' ') /* Neu la ki tu dau cach, do lon chuoi tang them 4 */
        {
            px = px + 4;
        }
        else /* Neu la ki tu khac */
        {
            if (m_font->width > 30) /* Neu do lon ki tu > 30, do lon chuoi tang them = do lon ki tu + 3 */
            {
                px += m_font->width + 3;
            }
            else /* Neu do lon ki tu <= 30, do lon chuoi tang them = do lon ki tu */
            {
                px += m_font->width;
            }
        }
        str++; /* Toi ki tu tiep theo */
    }

    return px; /* Tra ket qua do lon chuoi sau khi het ki tu */
}
/**
 * @brief Ham kiem tra do lon cua chuoi ki tu (Voi ki tu tieng viet (Unicode)         
 * 
 * @param const uint16_t* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t px -> Do lon cua chuoi ki tu can kiem tra  
 */

int32_t GLcd_MeasureStringUni(const uint16_t *str)
{
    int32_t px = 0;
    while (*str != 0)
    {
        if (*str != ' ')
        {
            uint32_t char_pos = vnfontFindPosition(*str);
            px += g_vnfont_8x15[char_pos] + 1;
            str++;
        }
        else
        {
            str++;
            px += 3;
        }
    }
    return px;
}
/**
 * @brief Toa do vi tri giua cua chuoi ki tu unicode     
 * 
 * @param const uint16_t* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t -> Toa do vi tri giua cua chuoi ki tu  
 */
int32_t GLcd_CalculateXCenterStringUni(const uint16_t *s)
{
    return (GLCD_WIDTH - GLcd_MeasureStringUni(s)) / 2;
}

/**
 * @brief Toa do vi tri giua cua chuoi ki tu         
 * 
 * @param const uint16_t* str -> Chuoi ki tu can kiem tra           
 * @retval int32_t -> Toa do vi tri giua cua chuoi ki tu  
 */
int32_t GLcd_CalculateXCenterStringNormal(const uint8_t *s)
{
    return (GLCD_WIDTH - GLcd_MeasureString((const char*)s)) / 2;
}

/**
 * @brief Ham ve mot duong thang len man hinh         
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t x1 -> Toa do x diem ket thuc
 * @param int32_t y1 -> Toa do y diem ket thuc
 * @param int32_t color -> Mau sac hien thi                
 * @retval NONE 
 */
void GLcd_DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color)
{
    int32_t delta_x = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t delta_y = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int32_t step_x, step_y, error1, error2;

    step_x = (x0 < x1) ? 1 : (-1);
    step_y = (y0 < y1) ? 1 : (-1);

    error1 = delta_x - delta_y;
    while (1)
    {
        GLcd_DrawPoint(x0, y0, color);
        if ((x0 == x1) && (y0 == y1))
        {
            break;
        }
        error2 = 2 * error1;
        if (error2 > -delta_y)
        {
            error1 = error1 - delta_y;
            x0 = x0 + step_x;
        }
        if (error2 < delta_x)
        {
            error1 = error1 + delta_x;
            y0 = y0 + step_y;
        }
    }
}

/**
 * @brief Ham ve mot hinh chu nhat len man hinh         
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t width -> Do lon hinh 
 * @param int32_t height -> Chieu cao hinh
 * @param int32_t color -> Mau sac hien thi                
 * @retval NONE 
 */
void GLcd_DrawRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color)
{
    uint8_t current_value = 0;
    /* Draw the two horizontal lines */
    for (current_value = 0; current_value < width + 1; current_value++)
    {
        GLcd_DrawPoint(x + current_value, y, color);
        GLcd_DrawPoint(x + current_value, y + height, color);
    }

    /* draw the two vertical lines */
    for (current_value = 0; current_value < height + 1; current_value++)
    {
        GLcd_DrawPoint(x, y + current_value, color);
        GLcd_DrawPoint(x + width, y + current_value, color);
    }
}

/**
 * @brief Ham ve mot hinh tron len man hinh         
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t int32_t radius -> Ban kinh hinh tron
 * @param int32_t color -> Mau sac hien thi                
 * @retval NONE 
 */
void GLcd_DrawCircle(int32_t x, int32_t y, int32_t radius, int32_t color)
{
    int32_t sw = 0, tmp_y = 0, tmp_x = 0;
    uint8_t d;

    d = y - x;
    tmp_y = radius;
    sw = 3 - 2 * radius;
    while (tmp_x <= tmp_y)
    {
        GLcd_DrawPoint(x + tmp_x, y + tmp_y, color);
        GLcd_DrawPoint(x + tmp_x, y - tmp_y, color);

        GLcd_DrawPoint(x - tmp_x, y + tmp_y, color);
        GLcd_DrawPoint(x - tmp_x, y - tmp_y, color);

        GLcd_DrawPoint(y + tmp_y - d, y + tmp_x, color);
        GLcd_DrawPoint(y + tmp_y - d, y - tmp_x, color);
        GLcd_DrawPoint(y - tmp_y - d, y + tmp_x, color);
        GLcd_DrawPoint(y - tmp_y - d, y - tmp_x, color);

        if (sw < 0)
            sw += (4 * tmp_x + 6);
        else
        {
            sw += (4 * (tmp_x - tmp_y) + 10);
            tmp_y--;
        }

        tmp_x++;
    }
}

/**
 * @brief Ham hien thi boi den mot hinh vuong hoac chu nhat      
 * 
 * @param int32_t x0 -> Toa do x diem bat dau  
 * @param int32_t y0 -> Toa do y diem bat dau
 * @param int32_t width -> Do rong cua hinh
 * @param int32_t height -> Chieu cao cua hinh
 * @param int32_t color -> Mau sac cua hinh                 
 * @retval NONE 
 */
void GLcd_FillRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color)
{
    uint8_t i, j;
    for (i = x; i < x + width; i++)
    {
        for (j = y; j < y + height; j++)
            GLcd_DrawPoint(i, j, color);
    }
}
/**
 * @brief Ham ve mot anh len toan man hinh    
 * 
 * @param const uint8_t* img -> Mang chua anh can ve                 
 * @retval NONE 
 */

void GLcd_DrawImage(const uint8_t *img)
{
    int32_t x = 0, y = 0, b = 0;
    int32_t index = 0;

    for (y = 0; y < 8; y++) /* Quet lan luot 8 block, moi block la 128x8 */
    {
        for (x = 0; x < 128; x++) /* Quet tong 128 cot */
        {
            for (b = 0; b < 8; b++) /* Quet theo cot, moi cot 8 bit */
            {
                index = y * 128 + x;
                if (CHECKBIT(img[index], b)) /* Kiem tra neu bit bang 1 */
                {
                    GLcd_DrawPoint(x, y * 8 + b, WHITE); /* Ve diem tren man hinh mau trang */
                }
                else
                {
                    GLcd_DrawPoint(x, y * 8 + b, BLACK); /* Ve diem tren man hinh mau den */
                }
            }
        }
    }
}

/**
 * @brief Ham ve mot anh tai toa do bat ki   
 * 
 * @param const uint8_t *image -> Mang chua anh can ve 
 * @param int32_t x -> Toa do x diem bat dau 
 * @param int32_t y -> Toa do y diem bat dau                
 * @retval NONE 
 */

void GLcd_DrawBitmap(const uint8_t *image, int32_t x, int32_t y)
{
    int32_t w = *image++; /* Lay do rong cua anh */
    int32_t h = *image++; /* Lay chieu cao cua anh */

    for (uint16_t i = 0; i < w; i++)
    {
        for (uint16_t j = 0; j < h; j++)
        {
            if (READBIT(*(image + i + (j / 8) * w), j % 8))
            {
                GLcd_DrawPoint(x + i, y + j, WHITE);
            }
						else
							GLcd_DrawPoint(x + i, y + j, BLACK);
        }
    }
}

/**
 * @brief Ham day du lieu vao GDRAM cua toan man hinh  
 * 
 * @param m_off_buffer -> Mang da duoc truyen du lieu tu cac ham phia tren 
 * @param GLCD_WIDTH -> Do lon man hinh = 128
 * @param GLCD_HEIGHT -> Chieu cao man hinh = 64             
 * @retval NONE 
 */

void GLcd_Flush(void)
{
    for (uint8_t r = 0; r < GLCD_HEIGHT; ++r)
    {
        uint8_t end_col_num = (GLCD_WIDTH + 15) / 16;
        set_graphics_address(r, 0);
        uint8_t *ptr = m_off_buffer + (16 * r);
        for (uint8_t i = 0; i < end_col_num; ++i)
        {
            send_data(*ptr++);
            send_data(*ptr++);
        }
    }
}
/**
 * @brief Ham day du lieu vao GDRAM tu hang bat ky  
 * 
 * @param m_off_buffer -> Mang da duoc truyen du lieu tu cac ham phia tren 
 * @param int32_t y -> Toa do cua hang bat ki
 * @param int32_t height -> Chieu cao cua phan hien thi           
 * @retval NONE 
 */
void GLcd_FlushRegion(int32_t y, int32_t height)
{
    if (y < 0 || y >= GLCD_HEIGHT)
    {
        return;
    }

    for (uint8_t r = y; r < y + height; ++r)
    {
        uint8_t end_col_num = (GLCD_WIDTH + 15) / 16;
        set_graphics_address(r, 0);
        uint8_t *ptr = m_off_buffer + (16 * r);
        for (uint8_t i = 0; i < end_col_num; ++i)
        {
            send_data(*ptr++);
            send_data(*ptr++);
        }
    }
}


/**
 * @brief Ham day command vao man hinh GLCD
 * 
 * @param uint8_t command -> Byte command 
 * @retval NONE 
 */
static void send_cmd(uint8_t command)
{
    send_lcd(0xF8, command); /* Byte dau tien = 0xF8 -> Dat lenh ghi Command */
}

static void send_lcd(uint8_t data1, uint8_t data2)
{
    send_slow(data1);        /* Byte dat lenh */
    send_slow(data2 & 0xF0); /* Byte truyen 4 bit cao cua du lieu*/
    send_slow(data2 << 4);   /* Byte truyen 4 bit thap cua du lieu*/
}

static void cmd_delay()		// not importance function
{
	volatile uint32_t i = 0x07F;
	while (i--);
}
/**
 * @brief Ham day du lieu vao man hinh GLCD
 * 
 * @param uint8_t data -> Du lieu 8 bit can truyen     
 * @retval NONE 
 */
static void send_data(uint8_t data)
{
    send_lcd(0xFA, data); /* Byte dau tien = 0xFA -> Dat lenh ghi toi RAM */
}
/**
 * @brief Set dia chi trong GDRAM
 * 
 * @param uint32_t r -> Dia chi hang 
 * @param uint32_t c -> Dia chi cot
 * @retval NONE 
 */

static void set_graphics_address(uint32_t r, uint32_t c)
{
    send_cmd(LCD_SET_GDRAM_ADDRESS | (r & 31));
    cmd_delay();
    send_cmd(LCD_SET_GDRAM_ADDRESS | c | ((r & 32) >> 2));
    cmd_delay();
}

static void send_slow(uint8_t data)
{
	unsigned char s_data[2];
	s_data[0] = data;
		HAL_SPI_Transmit(&hspi2, s_data, 1, 1);
}
#endif //end lcd_color
