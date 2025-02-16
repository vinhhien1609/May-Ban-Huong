#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "vdm_language.h"


/**
 * @brief  Mang 2 chieu chua chuoi ki tu hien thi cua 2 ngon ngu Tieng anh vA Tieng Viet
 *
 * @param  LANG_ID -> Vi tri theo hang (Ki tu can hien thi)
 * @param  VDM_LANG_MAX -> Vi tri theo cot (Ngon ngu can hien thi)
 * @retval uint16_t* m_lang_text -> Chuoi ki tu trich xuat tu mang
 */


const char *m_lang_text[VDM_LANG_ID_MAX][VDM_LANG_MAX] =
{
    /* khong thu hien chinh sua duoi dong nay */
    {   "CÓ         KHÔNG",                                  "YES           NO"},
    {   "B\x82N MU\xA3N TI\x92P T\xA9\x28 ?",              "CONTINUE RESET ?"},
		{		"NH\x81P M\x81T KH\x83U",												 "PASSWORD"},
		
    /* Glass setting */
    {   "CH\x92 \xAE\x98 HI\xCDN T\x82I",                 "CURRENT CONFIG"}, // Che do hien tai
		{		"T\x9ENG DOANH THU",															"NONE"},						// TONG DOANH THU
		{		"CÀI \xAE\x80T S\xA3 L\xAA\xA4NG",										"NONE"},				// cai dat so luong
		{		"THÔNG TIN L\xA1I",																"ERROR INFO"},	//THONG TIN LOI
		{		"B\x81T T\x89T \xAEK \xAE\x98 \x83M", "NONE"},													// DAT DK DO AM
		{		"CÀI \xAE\x80T \xAE\x98 \x83M",				"SETTING HUMIDITY"},							// Cai do am
		{		"M\x94NH GIÁ CH\x8BP NH\x81N", "NONE"},														// menh gia chap nhan
		{		"TGIAN C\x8BP H\xAA\x9FNG", "NONE"},															// TG CAP HUONG
		{		"S\xA3 H\xAA\x9FNG BÁN", "NONE"},																	// SO HUONG BÁN
		{		"S\xA3 H\xAA\x9FNG BTHÊM", "NONE"},																// SO HUONG BÁN THEM
		{		"S\xA3 L\x8AN B\xC1N L\xA1I","NONE"},									// So LAN BAN THEM KHI HET
		{		"XÓA L\xA1I CELL", "NONE"},																				// XOA LOI
    {   "CH\x1E \x1\x1E HO\x1ET \x11\x1ENG",            "MODE"},      // Che do hoat dong
    {   "RESET PH\x8AN M\x91M",                    						 "RESTORE FIRMWARE"},
		{		"THEO NG\xC0Y", "NONE"},
		{		"THEO TH\xC1NG", "NONE"},
		{		"THEO N\x8CM", "NONE"},
		{		"T\x9ENG DOANH THU","NONE"},
		{		"S\xA3 L\xAA\xA4NG","NONE"},		//so luong
		{		"T\x9ENG TI\x91N","NONE"},			// tong tien
		{		"TH\xD4NG TIN L\xA1I","NONE"},
		{		"L\xA1I MOTOR","NONE"},
		{		"L\xA1I THANH TO\xC1N","NONE"},
		{   "L\xA1I C\x85M BI\x92N R\x9FI","NONE"},
		{		"X\xD3\x41 L\xA1I MOTOR","NONE"},
		{		"X\xD3\x41 L\xA1I PH\x8AN C\xA8NG","NONE"},
		{   "X\xD3\x41 L\xA1I C\x85M BI\x92N","NONE"},
		{		"B\x81T","NONE"},
		{		"T\x89T","NONE"},
		{		"C\xC0I \xAE\x80T \xAE\x98 \x83M","NONE"},	// cai dat do am
		{		"TGIAN C\x8BP H\xAA\xA4NG","NONE"},
		{		"KH\xD4NG C\xD3 L\xA1I","NO ERROR"},
		{		"XONG", "DONE"},
		{		"CH\x9B NH\x81N NHANG", "NONE"},
		{		"XIN L\xA1I", "NONE"},
		{		"\xAE\xC3 H\x92T NHANG", "NONE"},
		{		"M\x9BI NH\x81N NHANG","NONE"},
		{		"C\x85M \x9FN Q\xDAY KH\xC1\x43H","NONE"},
		{		"\xAE\xC3 NH\x81N:",	"NONE"},
		{		"\xAE\xC3 XONG",	"DONE"},
		{		"T\x82M NG\xAANG PH\xA9\x43 V\xA9", "DO NOT SERVICE"},
		{		"ID M\xC1Y", "ID MACHINE"},
		{		"IP SERVER", "SERVER IP"},
};

const char *m_lang_txt[VDM_LANG_ID_MAX][VDM_LANG_MAX] =
{
    /* khong thu hien chinh sua duoi dong nay */
    {   "CÓ         KHÔNG",                                  "YES           NO"},						//VDM_LANG_YES_NO
    {   "BẠN MUỐN TIẾP TỤC ?",              "CONTINUE RESET ?"},														//VDM_LANG_CONFIRM_RESET
		{		"NHẬP MẬT KHẨU",												 "PASSWORD"},																//VDM_LANG_PASSWORD
		
    /* Glass setting */
    {   "CHẾ ĐỘ HIỆN TẠI",                 "CURRENT CONFIG"}, // Che do hien tai		//VDM_LANG_CURRNT_CONFIG
		{		"TỔNG DOANH THU",															"NONE"},						// TONG DOANH THU		//VDM_LANG_TOTAL_MOUNT
		{		"CÀI ĐẶT SỐ LƯỢNG",										"NONE"},				// cai dat so luong			//VDM_LANG_TOTAL_SETTING
		{		"THÔNG TIN LỖI",																"ERROR INFO"},	//THONG TIN LOI			//VDM_LANG_ERROR_INFO
		{		"BẬT TẮT ĐỘ ẨM", "NONE"},													// DAT DK DO AM					//VDM_LANG_EN_CONTROL_HUMIDITY
		{		"CÀI ĐẶT ĐỘ ẨM",				"SETTING HUMIDITY"},							// Cai do am			//VDM_LANG_HUMUDITY_SETTING
		{		"MỆNH GIÁ CHẤP NHẬN", "NONE"},														// menh gia chap nhan		//VDM_LANG_ACCEPT_MOUNT
		{		"THỜI GIAN CẤP HƯƠNG", "NONE"},															// TG CAP HUONG			//VDM_LANG_TIME_RUN_INSENSE
		{		"SỐ HƯƠNG BÁN", "NONE"},																	// SO HUONG BÂŽ		//VDM_LANG_TOTAL_INSENSE_CELL
		{		"SỐ HƯƠNG BÁN THÊM", "NONE"},																// SO HUONG BÂŽ THEM		//VDM_LANG_INSENSE_CELL_MORE
		{		"SỐ LẦN BÁN THÊM KHI HƯƠNG HẾT","NONE"},									// So LAN BAN THEM KHI HET		//VDM_LANG_NUMBER_CELL_MORE
		{		"XÓA LỖI CELL", "NONE"},																				// XOA LOI		//VDM_LANG_DELETE_ERROR_CELL
    {   "CHẾ ĐỘ HOẠT ĐỘNG",            "MODE"},      // Che do hoat dong		//VDM_LANG_MODE
    {   "CÀI ĐẶT MẶC ĐỊNH",                    						 "RESTORE FACTORY"},		//VDM_LANG_ID_RESTORE_FACTORY_FIRMWARE
		{		"THEO NGÀY", "NONE"},			//VDM_LANG_DAY
		{		"THEO THÁNG", "NONE"},			//VDM_LANG_MONTH
		{		"THEO NĂM", "NONE"},				//VDM_LANG_YEAR
		{		"TỔNG DOANH THU","NONE"},		//VDM_LANG_TOTAL
		{		"SỐ LƯỢNG","NONE"},		//so luong		//VDM_LANG_NUMBER
		{		"TỔNG TIỀN","NONE"},			// tong tien		//VDM_LANG_TOTAL_MONEY
		{		"THÔNG TIN LỖI","NONE"},		//VDM_LANG_ERR_INFO
		{		"L\xA1I MOTOR","NONE"},
		{		"L\xA1I THANH TO\xC1N","NONE"},
		{   "L\xA1I C\x85M BI\x92N R\x9FI","NONE"},
		{		"X\xD3\x41 L\xA1I MOTOR","NONE"},
		{		"X\xD3\x41 L\xA1I PH\x8AN C\xA8NG","NONE"},
		{   "X\xD3\x41 L\xA1I C\x85M BI\x92N","NONE"},
		{		"B\x81T","NONE"},
		{		"T\x89T","NONE"},
		{		"C\xC0I \xAE\x80T \xAE\x98 \x83M","NONE"},	// cai dat do am
		{		"TGIAN C\x8BP H\xAA\xA4NG","NONE"},
		{		"KH\xD4NG C\xD3 L\xA1I","NO ERROR"},
		{		"XONG", "DONE"},
		{		"CH\x9B NH\x81N NHANG", "NONE"},
		{		"XIN L\xA1I", "NONE"},
		{		"\xAE\xC3 H\x92T NHANG", "NONE"},
		{		"M\x9BI NH\x81N NHANG","NONE"},
		{		"C\x85M \x9FN Q\xDAY KH\xC1\x43H","NONE"},
		{		"\xAE\xC3 NH\x81N:",	"NONE"},
		{		"ĐÃ XONG",	"DONE"},
		{		"THIẾT BỊ BẢO TRÌ", "DO NOT SERVICE"},
		{		"ID THIẾT BỊ", "ID MACHINE"},									//VDM_LANG_ID_MACHINE
		{		"IP SERVER", "SERVER IP"},
		{		"ĐƠN GIÁ", "UNIT PRICE"},					//VDM_LANG_UNIT_PRICE
		{		"SỐ HƯƠNG MUA", "QUANLITY"},				//VDM_LANG_QUANLITY
		{		"SỐ DƯ TÀI KHOẢN", "BALANCE CLOSING"},		//VDM_LANG_BALANCE
		{		"SỐ TIỀN CÒN THIẾU", "MISSING AMOUNT"},		//VDM_LANG_MISSING_AMOUNT
		{		"SỐ ĐIỆN THOẠI LIÊN HỆ", "PHONE NUMBER"},
		{		"SỐ LẦN BÁN LỖI", "SALE ERROR NUMBER"},	//VDM_LANG_NUMBER_SALE_ERROR
		{		"XÁC NHẬN", "ACCEPT"},	//VDM_LANG_ACCEPT
		{		"THÔNG TIN THIẾT BỊ", "HARDWARE INFO"},	//VDM_LANG_VERION_INFO
		{		"BẢO VỆ ĐỘNG CƠ", "PROTECT MOTOR"},	//VDM_PROTECT_MOTOR
		{		"BĂNG TẢI"	, "CONVEYOR"},	
		{		"ĐẢO HƯƠNG"	, "SWAP"},
};

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
//    return m_current_language;
	return VDM_LANG_VNI;
}

/**
 * @brief  Ham lay chuoi ki tu trong mang 2 chieu theo ngon ngu hien tai
 *
 * @param  vdm_language_id_t language_id -> Dia chi hang chua chuoi ki tu
 * @param  m_current_language -> Vi tri cot ngon ngu hien tai
 * @retval uint16_t* vdm_language_get_text -> Chuoi ki tu can doc
 */
const char *vdm_language_get_text(vdm_language_id_t language_id)
{
    //return m_lang_text[language_id][m_current_language];
		return m_lang_text[language_id][VDM_LANG_VNI];
}

const char *vdm_language_text(vdm_language_id_t language_id)
{
    //return m_lang_text[language_id][m_current_language];
		return m_lang_txt[language_id][VDM_LANG_VNI];
}

#if 0
void vdm_language_dump_text(vdm_language_id_t language_id, const uint16_t **vni, const uint16_t **eng)
{
    DEBUG_INFO("Lang id %u\r\n", language_id);
    *vni = m_lang_text[language_id][VDM_LANG_VNI];
    *eng = m_lang_text[language_id][VDM_LANG_ENGLISH];
}

#endif

