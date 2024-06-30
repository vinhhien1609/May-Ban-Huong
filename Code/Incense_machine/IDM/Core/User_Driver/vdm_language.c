#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "vdm_language.h"
#include "app_debug.h"
#include "vdm_device_config.h"

/**
 * @brief  Mang 2 chieu chua chuoi ki tu hien thi cua 2 ngon ngu Tieng anh vA Tieng Viet
 *
 * @param  LANG_ID -> Vi tri theo hang (Ki tu can hien thi)
 * @param  VDM_LANG_MAX -> Vi tri theo cot (Ngon ngu can hien thi)
 * @retval uint16_t* m_lang_text -> Chuoi ki tu trich xuat tu mang
 */

uint16_t MANG[15] = (uint16_t)"ABCDEF0";//"NH\x1EACP M\x1EACT KH\x1EA8U";

uint16_t *m_lang_text[VDM_LANG_ID_MAX][VDM_LANG_MAX] =
{

};


//const uint16_t *m_lang_text[VDM_LANG_ID_MAX][VDM_LANG_MAX] =
//{
//    {   u"NH\x1EACP M\x1EACT KH\x1EA8U",                            u"ENTER PASSWORD"},
//    {   u"NH\x1EACP M\x1EACT KH\x1EA8U M\x1EDAI",                   u"ENTER NEW PASSWORD"},
//    {   u"NH\x1EACP L\x1EA0I M\x1EACT KH\x1EA8U",                   u"ENTER IT AGAIN"},
//    {   u"M\x1EACT KH\x1EA8U KH\xD4NG KH\x1EDAP",                   u"PASSWORD NOT MATCH"},
//    {   u"XONG",                                                    u"DONE"},
//    {   u"T\x1ED4NG DOANH THU",                                     u"TOTAL SALES"},
//    {   u"THEO NG\xC0Y",                                            u"DAILY SALES"},
//    {   u"THEO TH\xC1NG",                                           u"MONTHLY SALES"},
//    {   u"THEO N\x102M",                                            u"YEARLY SALES"},
//    {   u"KEY WITH SLOT",                                           u"KEY WITH SLOT"},
//    {   u"\x110\x1A0N GI\xC1 & S\x1ED0 L\x1AF\x1EE2NG",             u"PRICE & CAPACITY"},
//    {   u"\x110\x1A0N GI\xC1",                                      u"PRICE"},
//    {   u"S\x1ED0 L\x1AF\x1EE2NG",                                  u"CAPACITY"},
//    {   u"\x42\x1EACT T\x1EAET L\x45\x44",                          u"LED ON/OFF"},
//    {   u"C\x1EA2M BI\x1EBEN R\x1A0I",                              u"SET DROP SENSOR"},
//    {   u"TH\xD4NG TIN L\x1ED6I",                                   u"ERROR LOG"},
//    {   u"L\x1ED6I MOTOR",                                          u"MOTOR ERROR"},
//    {   u"L\x1ED6I TH\x41NH TO\xC1N",                               u"PAYMENT SYSERR"},
//    {   u"\x58\xD3\x41 L\x1ED6I \x110\x1ED8NG \x43\x1A0",           u"CLEAR MOTOR ERROR"},
//    {   u"\x58\xD3\x41 L\x1ED6I PH\x1EA6N \x43\x1EE8NG",            u"CLEAR HWARE ERROR"},
//    {   u"KH\xD4NG \x43\xD3 L\x1ED6I",                              u"NO ERROR"},
//    {   u"\x110\x1EA6U \x110\x1ECC\x43 \x42\x1ECA L\x1ED6I",        u"PAYMENT SYS IS ERROR"},
//    {   u"KI\x1EC2M TRA KHAY H\xC0NG",                              u"TEST SLOT"},
//    {   u"TI\x1EC0N V\xC0O RA",                                     u"CASH TOTAL DATA"},
//    {   u"T\x1ED4NG TI\x1EC0N V\xC0O",                              u"TOTAL CASH IN"},
//    {   u"T\x1ED4NG TI\x1EC0N RA",                                  u"TOTAL CASH OUT"},
//    {   u"\x58\xD3\x41 S\x1ED0 \x44\x1AF",                          u"CLEAR BALANCE"},
//    {   u"C\xC0I \x110\x1EB6T NHI\x1EC6T \x110\x1ED8",              u"SET TEMPERATURE"},
//    {   u"C\xC0I \x110\x1EB6T TH\x1EDCI GIAN",                      u"SET TIME"},
//    {   u"TI\x1EC0N TR\x1EA2 L\x1EA0I",                             u"BILL FOR CHANGE"},
//    {   u"B\x1EACT/T\x1EAET",                                       u"ON/OFF"},
//    {   u"\x110\x1AF\x41 TI\x1EC0N V\xC0O",                         u"BILL FILL"},
//    {   u"L\x1EA4Y TI\x1EC0N RA",                                   u"EMPTY RECYCLER"},
//    {   u"XEM TH\xD4NG TIN",                                        u"VIEW INFO"},
//    {   u"S\x1ED0 L\x1AF\x1EE2NG \x110\x1ED8NG C\x1A0",             u"SET SLOT NUMBER"},
//    {   u"S\x1ED0 C\x1ED8T:",                                       u"COLUMNS:"},
//    {   u"S\x1ED0 H\xC0NG:",                                        u"ROWS:"},
//    {   u"M\x1EC6NH GI\xC1 CH\x1EA4P NH\x1EACN",                    u"ACCEPTABLE NOTE"},
//    {   u"M\x1EC6NH GI\xC1 TR\x1EA2 L\x1EA0I",                      u"REFUND NOTE"},
//    {   u"C\xC0I \x110\x1EB6T L\x1ED6I",                            u"SET NV11 ERROR"},
//    {   u"NG\xD4N NG\x1EEE",                                        u"LANGUAGE"},
//    {   u"TI\x1EBENG VI\x1EC6T",                                    u"VIETNAMESE"},
//    {   u"TI\x1EBENG ANH",                                          u"ENGLISH"},
//    {   u"\x110\x1ED4I M\x1EACT KH\x1EA8U",                         u"CHANGE PASSWORD"},
//    {   u"XEM ID M\xC1Y",                                           u"VIEW ID"},
//    {   u"XEM IP S\x45RV\x45R",                                     u"VIEW SERVER INFOR"},
//    {   u"C\xC0I \x110\x1EB6T N\xC2NG CAO",                         u"ADVANCE SETTING"},
//    {   u"S\x1ED0 L\x1AF\x1EE2NG:",                                 u"QUANTITY:"},
//    {   u"T\x1ED4NG TI\x1EC0N:",                                    u"AMOUNT:"},
//    {   u"B\x1EACT",                                                u"TURN ON"},
//    {   u"T\x1EAET",                                                u"TURN OFF"},
//    {   u"RESET V\x1EC0 M\x1EB6\x43 \x110\x1ECANH",                 u"FACTORY RESET"},
//    {   u"\x110\x110ANG G\x1ECCI",                                  u"CALLING"},
//    {   u"KH\xD4NG TR\x1EA2 L\x1EDCI",                              u"NO ANSWER"},
//    {   u"M\xC1Y \x110\x41NG \x42\x1EACN",                          u"NUMBER IS BUSY"},
//    {   u"M\x1EA4T T\xCDN HI\x1EC6U",                               u"SIGNAL IS LOST"},
//    {   u"\x43U\x1ED8\x43 G\x1ECCI \x42\x1ECA L\x1ED6I",            u"CALLING ERROR"},
//    {   u"S\x1ED0 M\xC1Y T\x1ED4NG \x110\xC0I",                     u"OPERATOR NUMBER"},
//    {   u"C\xC0I \x110\x1EB6T \xC2M THANH",                         u"AUDIO SETTING"},
//    {   u"\x42\x1EACT/T\x1EAET",                                    u"ON/OFF"},
//    {   u"\xC2M L\x1AF\x1EE2NG CH\xCDNH",                           u"MAIN VOLUME"},
//    {   u"\xC2M L\x1AF\x1EE2NG CU\x1ED8\x43 G\x1ECCI",              u"CALL VOLUME"},
//    {   u"\x110\x1ED8 NH\x1EA0Y MIC",                               u"MIC GAIN LEVEL"},
//    {   u"TI\x1EC0N B\x1ECA K\x1EB8T",                              u"NOTE JAMMED"},
//    /* */
//    {   u"CH\x1EA0Y NG\x102N",                                      u"TESTING SLOT"},
//    {   u"NG\x102N L\x1ED6I",                                       u"SLOT ERROR"},
//    /* */
//    {   u"PHI\xCAN B\x1EA2N PH\x1EA6N M\x1EC0M",                    u"VIEW FIRMWARE"},
//    {   u"PHI\xCAN B\x1EA2N HI\x1EC6N T\x1EA0I",                    u"CURRENT FIRMWARE VERSION"},
//    /* */
//    {   u"KH\x1EDEI T\x1EA0O THAM S\x1ED0",                         u"INIT PARAMETER"},
//    {   u"\x110\x1ED4I M\x1EACT KH\x1EA8U I",                       u"CHANGE PWORD I"},
//    {   u"\x110\x1ED4I M\x1EACT KH\x1EA8U II",                      u"CHANGE PWORD II"},
//    /* */
//    {   u"L\x1ED6I C\x1EA2M BI\x1EBEN R\x1A0I",                     u"DROP SENSOR ERROR"},
//    {   u"\x58\xD3\x41 L\x1ED6I C\x1EA2M BI\x1EBEN",                u"CLEAR DSENSOR ERROR"},
//    /* */
//    /* khong thu hien chinh sua duoi dong nay */
//    {   u"C\xD3         KH\xD4NG",                                  u"YES           NO"},
//    {   u"B\x1EA0N MU\x1ED0N TI\x1EBEP T\x1EE4\x43 ?",              u"CONTINUE RESET ?"},

//    /* Glass setting */
//    {   u"C\xC0I \x110\x1EB6T S\x1AF\x1EDEI K\xCDNH",               u"GLASS WARMUP"},
//    {   u"CH\x1EBE \x110\x1ED8 HI\x1EC6N T\x1EA0I",                 u"CURRENT CONFIG"}, // Che do hien tai
//    {   u"CH\x1EBE \x110\x1ED8 HO\x1EA0T \x110\x1ED8NG",            u"MODE"},      // Che do hoat dong
//    {   u"TH\x1EDCI GIAN H\x110\x1ED8NG",                           u"ACTIVE TIME"},              // Thoi gian hoat dong
//    {   u"KH\xD4NG C\xD3",                                          u"NO DATA"},                                 // Khong cai dat
//    {   u"B\x1EACT",                                                u"TON"},
//    {   u"T\x1EAET",                                                u"TOFF"},
//    {   u"T\x1EAET S\x1AF\x1EDEI K\xCDNH",                          u"DISABLE GLASS WARMUP"},
//    {   u"NH\x1EACP NG\xC0Y",                                       u"INSERT DATE"},                  // Lap lai theo thu (2-cn)
//    {   u"L\x1EB6P L\x1EA0I M\xC3I M\xC3I",                         u"INFINITIVE"},       // Mai mai
//    {   u"T\xD9Y CH\x1EC8NH",                                       u"CUSTOMIZE"},                       // Tuy chinh
//    {   u"L\x1EB6P L\x1EA0I THEO GI\x1EDC",                         u"REPEAT BY HOUR"}, // Lap lai theo gio
//    {   u"T\x1EEA:",                                                u"FROM:"},                                 // From
//    {   u"D\x1EBEN:",                                               u"TO:"},                                  // To
//    {   u"KH\xD4NG THAY \x110\x1ED4I",                              u"NO CHANGE"},           // To}
//    {   u"GI\x1EDC B\x1EACT",                                       u"ON"},                           // Thoi gian bat
//    {   u"GI\x1EDC T\x1EAET",                                       u"OFF"},                          // Thoi gian tat
//    {   u"B\x1EACT/T\x1EAET",                                       u"ON/OFF"},                       // ON/OFF
//    {   u"C\xC0I \x110\x1EB6T TH\x1EDCI GIAN",                      u"UV TIME SETTING"},
//    {   u"C\xC0I \x110\x1EB6T UV",                                  u"UV CONTROL"}, // UV CONTROL
//    {   u"NH\x1EACP TH\x1EDCI GIAN",                                u"SELECT TIME"},
//    {   u"\x42\x1EACT T\x1EAET RGB",                                u"RGB ON/OFF"},
//    {   u"M\x1EACT KH\x1EA8U TR\xD9NG",                             u"THIS IS OLD PWD"},
//    {   u"\x110ANG C\x1EACP NH\x1EACT PH\x1EA6N M\x1EC0M",          u"UPDATING FIRMWARE"},
//    {   u"RESET PH\x1EA6N M\x1EC0M",                    u"RESTORE FIRMWARE"},
//};

static vdm_language_t m_current_language = VDM_LANG_VNI; /* Ngon ngu khai bao ban dau: Tieng Viet*/

/**
 * @brief  Ham doc ngon ngu cai dat trong EEPROM
 *
 * @param  LANGUAGE_EEP_ADDRESS -> Dia chi EEPROM chu du lieu cai dat ngon ngu
 * @retval uint16_t* m_lang_text -> Chuoi ki tu trich xuat tu mang
 */
void vdm_language_restore_from_storage()
{
    int8_t data;
    /* Lay du lieu tu EEPROM */
//    vdm_device_config_lock_resource(0xFFFFFFFF);
//    int32_t lang = vdm_get_device_config()->peripheral.language;
		uint32_t lang = VDM_LANG_VNI;
//    vdm_device_config_unlock_resource();
    /* Neu doc thanh cong va du lieu = 0*/
    if (lang == VDM_LANG_VNI)
    {
        m_current_language = VDM_LANG_VNI; /* Ngon ngu Tieng Viet */
    }
    else
    {
        m_current_language = VDM_LANG_ENGLISH; /* Ngon ngu Tieng Anh */
    }
}
/**
 * @brief  Ham cai dat ngon ngu vao EEPROM
 *
 * @param  LANGUAGE_EEP_ADDRESS -> Dia chi EEPROM chu du lieu cai dat ngon ngu
 * @param  vdm_language_t language -> Du lieu cua ngon ngu cai dat
 * @retval NONE
 */
void vdm_language_set_language(vdm_language_t language)
{
    if (language < VDM_LANG_MAX)
    {
        m_current_language = language;

//        vdm_device_config_lock_resource(0xFFFFFFFF);
//        vdm_get_device_config()->peripheral.language = m_current_language;
//        vdm_device_config_sync_peripheral_to_fram();
//        vdm_device_config_unlock_resource();
    }
}
/**
 * @brief  Ham lay gia tri ngon ngu hien tai
 *
 * @param  NONE
 * @retval vdm_language_t vdm_language_get_current_lang -> Ngon ngu hien tai
 */
vdm_language_t vdm_language_get_current_lang()
{
    return m_current_language;
}

/**
 * @brief  Ham lay chuoi ki tu trong mang 2 chieu theo ngon ngu hien tai
 *
 * @param  vdm_language_id_t language_id -> Dia chi hang chua chuoi ki tu
 * @param  m_current_language -> Vi tri cot ngon ngu hien tai
 * @retval uint16_t* vdm_language_get_text -> Chuoi ki tu can doc
 */
const uint16_t *vdm_language_get_text(vdm_language_id_t language_id)
{
    return m_lang_text[language_id][m_current_language];
}

#if 0
void vdm_language_dump_text(vdm_language_id_t language_id, const uint16_t **vni, const uint16_t **eng)
{
    DEBUG_INFO("Lang id %u\r\n", language_id);
    *vni = m_lang_text[language_id][VDM_LANG_VNI];
    *eng = m_lang_text[language_id][VDM_LANG_ENGLISH];
}

#endif

