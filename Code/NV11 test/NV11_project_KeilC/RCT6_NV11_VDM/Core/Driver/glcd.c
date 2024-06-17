
#include "glcd.h"
//#include "FreeRTOS.h"
//#include "task.h"
#include "stm32f1xx_hal.h"
#include "glcd_font.h"
#include "main.h"

#define LCD_DISPLAY_CLEAR 0x01
#define LCD_FUNCTION_SET_BASIC_ALPHA 0x20
#define LCD_ENTRY_MODE_SET 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_DISPLAY_OFF 0x08
#define LCD_FUNCTION_SET_BASIC_GRAPHIC 0x22
#define LCD_FUNCTION_SET_EXTENDED_GRAPHIC 0x26
#define LCD_SET_GDRAM_ADDRESS 0x80

#define DELAY_GPIO_ACTION           0//50

static font_t *m_font;
static uint8_t m_off_buffer[(GLCD_WIDTH * GLCD_HEIGHT) / 8];

static void send_data(uint8_t data);
static void send_cmd(uint8_t command);
static void send_lcd(uint8_t data1, uint8_t data2);
static void send_slow(uint8_t data);
static void cmd_delay(void);
static void goto_extended_mode();
static void goto_basic_mode();
static void set_graphics_address(uint32_t r, uint32_t c);

static glcd_pin_write_cb_t m_write_cb = 0;
static glcd_protect_cb_t m_lock = 0;
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
	
//	HAL_GPIO_TogglePin(BUZZ_GPIO_Port,BUZZ_Pin);
    if (output)
    {
        output = GPIO_PIN_SET;
    }
    else
    {
        output = GPIO_PIN_RESET;
    }

    switch (pin)
    {
    case GLCD_LCD_PIN_CS:
				HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, output);
        break;

    case GLCD_LCD_PIN_CLK:
        HAL_GPIO_WritePin(LCD_CLK_PORT, LCD_CLK_PIN, output);
        break;

    case GLCD_LCD_PIN_DATA:
        HAL_GPIO_WritePin(LCD_DATA_PORT, LCD_DATA_PIN, output);
        break;

    default:
        break;
    }
}


void GLcd_Init(glcd_pin_write_cb_t write_callback, glcd_protect_cb_t lock_cb)
{
	
	 
	m_write_cb = write_callback;
    m_lock = lock_cb;

    m_write_cb(GLCD_LCD_PIN_CLK, 0);
    m_write_cb(GLCD_LCD_PIN_DATA, 0);
    m_write_cb(GLCD_LCD_PIN_CS, 1);

    send_cmd(LCD_FUNCTION_SET_BASIC_ALPHA); /* Set dia chi DDRAM ve 0x00 va dua con tro ve goc */
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
void GLcd_DrawStringUni(const uint16_t *s, int32_t x, int32_t y, int32_t color)
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
    for (uint8_t i = 0; i < 8; ++i)
    {
        /* Set value to DATA Pin. Because the Pin_Write function takes time to execute, we have to 
           access directly to the registers to speed up the interface */
        if (data & 0x80)
        {
//            g_dataPort->PSOR |= (1 << g_dataPinInPort); // DATA Pin = 1
            m_write_cb(GLCD_LCD_PIN_DATA, 1);
        }
        else
        {
        	m_write_cb(GLCD_LCD_PIN_DATA, 0); // DATA Pin = 0
        }

        /* Set CLOCK Pin to 1. Because the Pin_Write function takes time to execute, we have to 
           access directly to the registers to speed up the interface */
//        g_clockPort->PSOR |= (1 << g_clockPinInPort);
        m_write_cb(GLCD_LCD_PIN_CLK, 1);
        for (volatile uint32_t i = 0; i < DELAY_GPIO_ACTION; i++)
        {

        }
//        g_clockPort->PCOR |= (1 << g_clockPinInPort);
        m_write_cb(GLCD_LCD_PIN_CLK, 0);
        for (volatile uint32_t i = 0; i < DELAY_GPIO_ACTION; i++)
        {

        }

        data <<= 1;
    }
}

static void cmd_delay()		// not importance function
{
	volatile uint32_t i = 0x0FF;
	while (i--);
}

static void goto_extended_mode()
{
    send_cmd(LCD_FUNCTION_SET_EXTENDED_GRAPHIC);
    cmd_delay();
}

static void goto_basic_mode()
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
