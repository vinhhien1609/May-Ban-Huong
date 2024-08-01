/*
 * vdm_app_gsm.h
 *
 *  Created on: Aug 11, 2021
 *      Author: huybk
 */

#ifndef VDM_APP_GSM_H_
#define VDM_APP_GSM_H_

/**
 * \defgroup        app_send
 * \brief           Send data to cloud, and stored frame if any error occurs during tcp connect
 * \{
 */

#include <stdint.h>
#include <stdbool.h>

#define VDM_APP_GSM_MSG_ID_REBOOT 0x61
#define VDM_APP_GSM_MSG_ID_PERIODIC_UPDATE_DATA 0x62
#define VDM_APP_GSM_MSG_ID_DOOR_OPEN 0x63
#define VDM_APP_GSM_MSG_ID_DOOR_CLOSE 0x64
#define VDM_APP_GSM_MSG_ID_ACTION_BUY_SUCCESS 0x65
#define VDM_APP_GSM_MSG_ID_ACTION_BUY_ERROR 0x66
#define VDM_APP_GSM_MSG_ID_NOTIFY_DEVICE_ERROR 0x67
#define VDM_APP_GSM_MSG_ID_NOTIFY_TOTAL_MONEY 0x68
#define VDM_APP_GSM_MSG_ID_RETURN_CASH_TO_CUSTOMER 0x69
#define VDM_APP_GSM_MSG_ID_UPDATE_TIMESTAMP 0x70

#define VDM_APP_GSM_DEVICE_REBOOT_OK 0x31
#define VDM_APP_GSM_LED_ON 0x31
#define VDM_APP_GSM_LED_OFF 0x30
#define VDM_APP_GSM_DROP_SENSOR_IS_ON 0x31
#define VDM_APP_GSM_DROP_SENSOR_IS_OFF 0x30
#define VDM_APP_GSM_NOT_FIXED_ERROR 0x30
#define VDM_APP_GSM_FIXED_ERROR 0x31

#define VDM_APP_GSM_ID_MOTOR(x) (0x00 + x)
#define VDM_APP_GSM_ID_SDCARD 0x69
#define VDM_APP_GSM_ID_STORAGE 0x70
#define VDM_APP_GSM_ID_QRM_SELL_OK 0x71
#define VDM_APP_GSM_ID_RTC 0x72
#define VDM_APP_GSM_ID_QRM_ERROR 0x73
#define VDM_APP_GSM_SERVER_REQUEST_OTA 0x75
#define VDM_APP_GSM_SERVER_CONFIRM_SUCCESS 0x76
#define VDM_APP_GSM_SERVER_CONFIRM_FAILED 0x77
#define VDM_APP_GSM_SERVER_TURN_OFF_WARNING 0x78

#define VDM_APP_GSM_ERROR 0x30
#define VDM_APP_GSM_OK 0x31

#define VDM_APP_GSM_DOOR_CLOSE 0x30
#define VDM_APP_GSM_DOOR_OPEN 0x31

#define MSG_HEADER_STR "###|"
#define MSG_HEADER_LEN 4
#define MAX_MONEY_PAYBACK_LEN 9

/* Note CRC calculate from slit_0 to split_4 (before crc)*/
/*
 * Format of data send to server
 * {
        char header[3];			// = "###"
        char split_0;			// = '|'
        char machine_id[10];	// = "TPA0123456"
        char split_1;			// = '|'
        char cmd[3];			// = "CMD"
        char split_2;			// = '|'
        uint8_t cmd;
        uint8_t split_3;		// symbol '|'

        uint8_t data[Size]

        uint8_t split_4;		// symbol '|'

        uint8_t crc;
        uint8_t crc_low;

        char end[3];			// = "&&&"
    }
 *
 **/
 
 typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t TS;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_start_frame_t;
 
typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_door_open_frame_t;

typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;         // 0x67
    uint8_t split_3;     // symbol '|'
    /* Data */
    uint8_t id;
    uint8_t error_status; // 0x30 or 0x31
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    uint8_t split_4; // symbol '|'
    /* End of data */
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_notify_error_frame_t;

typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_server_confirm_frame_t;

typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t version[3];
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_server_ota_frame_t;

typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_server_turn_off_warning_t;

typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t led;
    uint8_t temperature;
    uint8_t drop_sensor;
    uint8_t money_payback[MAX_MONEY_PAYBACK_LEN];
    uint8_t item_price[60];
    uint8_t nb_of_items_rack[60];
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_door_close_frame_t;

typedef struct
{
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t input_cash;
    uint8_t slot_number;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_released_item_frame_t;

typedef struct
{
    // static uint8_t g_FrameSellingError[] = "###|TPA0123456|CMD=|i|11000000|00&&&";
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
    /* Data */
    uint8_t refund_cash;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_paid_out_item_frame_t;

typedef struct
{
    // static uint8_t g_FrameSellingError[] = "###|TPA0123456|CMD=|f|11000000|00&&&";
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'

    /* Data */
    uint8_t input_cash;
    uint8_t slot_error_number;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */

    uint8_t split_4; // symbol '|'

    uint16_t crc;

    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_selling_error_frame_t;

typedef struct
{
    // static uint8_t g_FrameCycle[] = "###|TPA0123456|CMD=|b|101111111111000000000000000000000000000000000000000000000000000000000000000000000|00&&&"; // 0x62
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3; // symbol '|'
                     /* Data */
    uint8_t door;
    uint8_t temp;
    uint8_t nv11_error;      // Loi cua dau doc tien
    uint8_t cash_info[9];    // So luong to tien cua moi menh gia, vi du 10 to 1k, 15 to 20k....
                             // Byte 1 -> byte n tuong ung 1k-500k
    uint8_t item_remain[60]; // So luong hang con lai o moi khay hang
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    uint8_t firmware_version[3]; // "1.0.2" =>> byte[] = {0x01, 0x00, 0x03}
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_heartbeat_frame_t;

typedef struct
{
    // static uint8_t g_FrameCycle[] = "###|TPA0123456|CMD=|b|101111111111000000000000000000000000000000000000000000000000000000000000000000000|00&&&"; // 0x62
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3;  // symbol '|'
                      /* Data */
    uint8_t money;    // 20.000 =>> Money = 20
    uint8_t supplier; // Dich vu thanh toan
    uint8_t index;    // O dc chon
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_qr_sell_success_frame_t;

typedef struct
{
    // static uint8_t g_FrameCycle[] = "###|TPA0123456|CMD=|b|101111111111000000000000000000000000000000000000000000000000000000000000000000000|00&&&"; // 0x62
    char header[3];      // = "###"
    char split_0;        // = '|'
    char machine_id[10]; // = "TPA0123456"
    char split_1;        // = '|'
    char cmd_str[4];     // = "CMD="
    char split_2;        // = '|'
    uint8_t cmd;
    uint8_t split_3;  // symbol '|'
                      /* Data */
    uint8_t money;    // 20.000 =>> Money = 20
    uint8_t supplier; // Dich vu thanh toan
    uint8_t index;    // O dc chon
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t sec;
    /* End of data */
    uint8_t split_4; // symbol '|'
    uint16_t crc;
    char end[3]; // = "&&&"
} __attribute__((packed)) vdm_app_gsm_qr_sell_fail_frame_t;

typedef enum
{
    VDM_APP_GSM_MAIN_SERVER,
    VDM_APP_GSM_DEBUG_SERVER,
} vdm_app_gsm_server_type_t;

typedef struct
{
    uint8_t id;
    uint32_t length;
    uint8_t *data;
} vdm_app_gsm_frame_t;

typedef void (*vdm_app_gsm_server_confirm_cb_t)(void *data, uint32_t len);

/**
 * @brief		Register server confirm callback
 * @param[in]	callback Pointer to callback
 */
void vdm_app_gsm_on_register_confirm_callback(vdm_app_gsm_server_confirm_cb_t callback);

/**
 * @brief		Initialize app gsm for vending machine
 */
void vdm_app_gsm_initialize(void);

// bool vdm_app_gsm_insert_buffer(vdm_app_gsm_frame_t *buffer, uint32_t timeout_ms);

/**
 * @brief		Send start frame to server
 * @param[in]	NULL
 * 						   
 */
void vdm_app_gsm_send_start_frame(void);

/**
 * @brief		Send door frame to server
 * @param[in]	door_close TRUE door close
 * 						   FALSE door open
 */
void vdm_app_gsm_send_door_frame(bool door_close);

/**
 * @brief       Send motor error status to cloud, this function will send motor status = ok
 */
void vdm_app_gsm_send_motor_clear_all_error_frame(void);

/**
 * @brief       Send motor error status to cloud
 * @param[in]    motor_id Motor id
 */
void vdm_app_gsm_send_motor_error_frame(uint8_t motor_id);

/**
 * @brief       Send RTC resume frame to cloud
 */
void vdm_app_gsm_send_rtc_resume_frame(void);

/**
 * @brief       Send storage resume frame to cloud
 */
void vdm_app_gsm_send_storage_resume_frame(void);

/**
 * @brief       Send sdcard error frame to cloud
 * @param[in]   error TRUE sdcard error
 *                    FALSE sdcard good
 */
void vdm_app_gsm_send_sdcard_frame(bool error);

/**
 * @brief		Set device id
 * @param[in]	id Device id
 */
void vdm_app_gsm_set_device_id(uint8_t *device_id);

/**
 * @brief		Handle messsage from server
 * @param[in]	data Raw data from server
 * @param[in]	length Size of data
 */
void vdm_app_gsm_parse_new_server_msg(uint8_t *data, uint32_t length);

/**
 * @brief   Ham tao frame bao mua hang loi va luu tru du lieu EEPROM
 * @param   uint8_t receivedNote -> Gia tri tien da nhan
 * @param   uint8_t errorSlot -> Vi tri khay hang bi loi
 * @retval  NONE
 */

/**
 * @brief       Send selling error frame to cloud
 * @param[in]   received_cash Received cash value
 * @param[in]   error_slot_number Slot value
 */
void vdm_app_gsm_send_selling_error_frame(uint8_t received_cash, uint8_t error_slot_number);

/**
 * @brief       Send released item frame to cloud
 * @param[in]   received_cash Received cash value
 * @param[in]   slot_number Slot value
 */
void vdm_app_gsm_send_released_item_frame(uint8_t received_cash, uint8_t slot_number);

/**
 * @brief       Send paid out cash frame to cloud
 * @param[in]   paid_out Cash value
 */
void vdm_app_gsm_send_paid_out_item_frame(uint8_t paid_out);

/**
 * @brief       Send heartbeat message to server
 */
void vdm_app_gsm_send_heartbeat(void);

/**
 * @brief       Send QRM error frame to server
 */
void vdm_app_gsm_send_qrm_error_frame(void);

/**
 * @brief       Send QRM resume frame to server
 */
void vdm_app_gsm_send_qrm_resume_frame(void);

/**
 * @brief       Send QRM sell frame success frame to cloud
 * @param[in]   money Received cash value
 * @param[in]   supplier Supplier (MOMO, Viettel...)
 * @param[in]   index Slot value
 */
void vdm_app_gsm_send_qrm_sale_success_frame(uint8_t money, uint8_t supplier, uint8_t index);

/**
 * @brief       Send QRM sell frame failed to cloud
 * @param[in]   money Received cash value
 * @param[in]   supplier Supplier (MOMO, Viettel...)
 * @param[in]   index Slot value
 */
void vdm_app_gsm_send_qrm_sale_fail_frame(uint8_t money, uint8_t supplier, uint8_t index);

/**
 * \}
 */



#endif /* VDM_APP_GSM_H_ */
