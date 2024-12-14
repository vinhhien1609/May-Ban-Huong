/*
 * vdm_app_gsm.c
 *
 *  Created on: Aug 11, 2021
 *      Author: huybk
 */
#include "vdm_app_gsm.h"
#include "vdm_fw.h"
#include "crc_calculator.h"
#include "RTC_time.h"
#include "sys_ctx.h"
#include "vdm_device_config.h"
#include "nv11.h"
#include "IDM.h"
#include "app_debug.h"
#include "GSM_APP.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define SET_MESSAGE_TIME(message, time) \
    {                                   \
        message.year = time.year;       \
        message.month = time.month;     \
        message.day = time.day;         \
        message.hour = time.hour;       \
        message.minutes = time.minute;  \
        message.sec = time.second;      \
    }

		
static vdm_app_gsm_start_frame_t m_start_frame =
{
		.header = "###",
		.cmd_str = "CMD=",
		.cmd = VDM_APP_GSM_MSG_ID_REBOOT,
		.split_0 = '|',
		.split_1 = '|',
		.split_2 = '|',
		.split_3 = '|',
		.split_4 = '|',
		.TS			 = '1',
		.end = "&&&",
};
static vdm_app_gsm_door_open_frame_t m_door_open_frame =
    {
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_MSG_ID_DOOR_OPEN,
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

static vdm_app_gsm_notify_error_frame_t m_error_frame =
    {
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_MSG_ID_NOTIFY_DEVICE_ERROR,
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

 vdm_app_gsm_door_close_frame_t m_door_close_frame =
    {
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_MSG_ID_DOOR_CLOSE,
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

static vdm_app_gsm_selling_error_frame_t m_selling_error_frame =
    {
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_MSG_ID_ACTION_BUY_ERROR,
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

static vdm_app_gsm_released_item_frame_t m_released_item_frame =
    {
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_MSG_ID_ACTION_BUY_SUCCESS,
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

static vdm_app_gsm_paid_out_item_frame_t m_paid_out_item_frame =
    {
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_MSG_ID_RETURN_CASH_TO_CUSTOMER,
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

vdm_app_gsm_heartbeat_frame_t m_heartbeat_frame =
    {
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_MSG_ID_PERIODIC_UPDATE_DATA,
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .firmware_version[0] = VDM_FIRMWARE_VERSION_MAJOR,
        .firmware_version[1] = VDM_FIRMWARE_VERSION_MINOR,
        .firmware_version[2] = VDM_FIRMWARE_VERSION_BUILD,
        .end = "&&&",
};

static vdm_app_gsm_qr_sell_success_frame_t m_qrm_sale_success_frame =
    {
        // static uint8_t g_FrameSellQrSuccess[]       = "###|TPA0123456|CMD=|q|111000000|00&&&";  //0x71
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_ID_QRM_SELL_OK,
        .index = '0', // HuyTV : Code cu nguoi truoc ko thay su dung bien nay
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

static vdm_app_gsm_qr_sell_fail_frame_t m_qrm_sale_error_frame =
    {
        // static uint8_t g_FrameSellQrSuccess[]       = "###|TPA0123456|CMD=|q|111000000|00&&&";  //0x71
        .header = "###",
        .cmd_str = "CMD=",
        .cmd = VDM_APP_GSM_ID_QRM_ERROR,
        .index = '0', // Code cu nguoi truoc ko thay su dung bien nay
        .split_0 = '|',
        .split_1 = '|',
        .split_2 = '|',
        .split_3 = '|',
        .split_4 = '|',
        .end = "&&&",
};

extern IDM_PARA IDM;
extern IDM_HARDWARE IDM_Status;

static uint8_t m_device_id[10];

static void send_frame(vdm_app_gsm_server_type_t server_type, uint8_t cmd, uint8_t *data, uint32_t length)
{
	TCP_send(data, length);
}

//static void send_frame(vdm_app_gsm_server_type_t server_type, uint8_t cmd, uint8_t *data, uint32_t length)
//{
//    sys_ctx_t *ctx = sys_ctx();
//    uint32_t fram0_is_err = ctx->status.sys_err.name.fram0;
//    uint8_t modem_status= gsm_state();

//    DEBUG_INFO("GSM =>> send frame id 0x%02X\r\n", cmd);

//    if (fram0_is_err)
//    {
//        DEBUG_ERROR("Send GSM frame : FRAM error\r\n");
//    }
//    /*
//     *  1. If FRAM error =>> We cannot store data into memory
//     *  =>> Send data directly to cloud
//     *  2. If messgage id = VDM_APP_GSM_MSG_ID_PERIODIC_UPDATE_DATA
//     *  	or messgage id = VDM_APP_GSM_MSG_ID_UPDATE_TIMESTAMP
//     *  	or messgage id = VDM_APP_GSM_MSG_ID_NOTIFY_DEVICE_ERROR
//     *  	=>> Send data to cloud
//     *  3. Other message will be store in FRAM
//     */
//    if ((fram0_is_err || cmd == VDM_APP_GSM_MSG_ID_DOOR_OPEN || cmd== VDM_APP_GSM_MSG_ID_DOOR_CLOSE || cmd == VDM_APP_GSM_MSG_ID_PERIODIC_UPDATE_DATA || cmd == VDM_APP_GSM_MSG_ID_UPDATE_TIMESTAMP || cmd == VDM_APP_GSM_MSG_ID_NOTIFY_DEVICE_ERROR || cmd == VDM_APP_GSM_MSG_ID_ACTION_BUY_SUCCESS || cmd == VDM_APP_GSM_MSG_ID_ACTION_BUY_ERROR || cmd == VDM_APP_GSM_MSG_ID_RETURN_CASH_TO_CUSTOMER || cmd == VDM_APP_GSM_ID_QRM_SELL_OK || cmd == VDM_APP_GSM_ID_QRM_ERROR))
//    {

//        gsm_tcp_msq_t queue;
//        queue.length = length;
//        queue.data = (uint8_t *)pvPortMalloc(length);
//        if (cmd == VDM_APP_GSM_MSG_ID_PERIODIC_UPDATE_DATA)
//        {
//            queue.need_confirm = GSM_TCP_MESSAGE_NEED_CONFIRM_BUT_NOT_STORE_DATA_IF_FALL;
//        }
//        else
//        {
//            queue.need_confirm = GSM_TCP_MESSAGE_NEED_CONFIRM_AND_STORE_DATA_IF_FALL;
//        }

//        if (queue.data)
//        {

//            memcpy(queue.data, data, length);
////            DEBUG_WARN("[%s] Test Put message to flash\r\n", __FUNCTION__);
////            vdm_flash_store_new_message(queue.data, queue.length);
//            if (!gsm_tcp_msq_put(&queue, 1))
////            if (gsm_tcp_msq_put(&queue, 1)==0 || modem_status!=0)
//            {
//                // gsm_tcp_msq_delete_oldest_msg();
//                // gsm_tcp_msq_put(&queue, 1);
//                DEBUG_WARN("[%s] Put message to tcp queue error\r\n", __FUNCTION__);

//                // Log to flash and retransmit later
//                vdm_flash_store_new_message(queue.data, queue.length);

//                rtc_date_time_t time;
//                get_time(&time);

//                char file[24];
//                sprintf(file, "%04u_%02u_%02u.txt", time.year + 2000, time.month, time.day);
//                char *hex = pvPortMalloc(2 * queue.length + 32); // mot so ban tin co data ko phai ascii =>> dump to hex de con nguoi de doc
//                if (hex)
//                {
//                    char *p = hex;
//                    for (uint32_t i = 0; i < queue.length; i++)
//                    {
//                        if (isprint(queue.data[i]))
//                        {
//                            *p = queue.data[i];
//                            p++;
//                            *p = 0x00; // insert null
//                        }
//                        else
//                        {
//                            p += sprintf(p, "<%02x>", queue.data[i]);
//                        }
//                    }
//                    p += sprintf(p, "\r\n");
//                    vdm_sdcard_log_to_file("tcp_error", file, hex);
//                    vPortFree(hex);
//                }
//                vPortFree(queue.data);
//            }
//        }
//        else
//        {
//            vPortFree(queue.data);
//            DEBUG_ERROR("[%s] No memory\r\n", __FUNCTION__);
//            assert(0);
//        }
//    }

//    if (VDM_APP_GSM_MSG_ID_DOOR_OPEN == cmd)
//    {
//        vdm_sdcard_log(VDM_SDCARD_LOG_LEVEL_INFO, "Door open\r\n");
//    }
//    else if (VDM_APP_GSM_MSG_ID_DOOR_CLOSE == cmd)
//    {
//        vdm_sdcard_log(VDM_SDCARD_LOG_LEVEL_INFO, "Door close\r\n");
//    }
//    else if (VDM_APP_GSM_MSG_ID_ACTION_BUY_SUCCESS == cmd || VDM_APP_GSM_MSG_ID_ACTION_BUY_ERROR == cmd)
//    {
//        rtc_date_time_t time;
//        get_time(&time);
//        // Insert timestamp to message
//        char folder[24];
//        sprintf(folder, "%04u_%02u_%02u", time.year + 2000, time.month, time.day);
//        vdm_sdcard_log_to_file(folder, "log.txt",
//                               "%s =>> input cash %u, slot %u\r\n",
//                               VDM_APP_GSM_MSG_ID_ACTION_BUY_SUCCESS == cmd ? "OK" : "FAILED",
//                               m_released_item_frame.input_cash,
//                               m_released_item_frame.slot_number);
//    }
//    else if (VDM_APP_GSM_ID_QRM_SELL_OK == cmd || VDM_APP_GSM_ID_QRM_ERROR == cmd)
//    {
//        rtc_date_time_t time;
//        get_time(&time);
//        // Insert timestamp to message
//        char folder[24];
//        sprintf(folder, "%04u_%02u_%02u", time.year + 2000, time.month, time.day);
//        vdm_sdcard_log_to_file(folder, "log.txt",
//                               "%s =>> QRM sell %u, slot %u\r\n",
//                               VDM_APP_GSM_ID_QRM_SELL_OK == cmd ? "OK" : "FAILED",
//                               m_released_item_frame.input_cash,
//                               m_released_item_frame.slot_number);
//    }
//    else if (VDM_APP_GSM_MSG_ID_NOTIFY_DEVICE_ERROR == cmd)
//    {
//        rtc_date_time_t time;
//        get_time(&time);
//        // Insert timestamp to message
//        char folder[24];
//        sprintf(folder, "%04u_%02u_%02u", time.year + 2000, time.month, time.day);

//        char *hex = pvPortMalloc(2 * length + 64); // mot so ban tin co data ko phai ascii =>> dump to hex de con nguoi de doc
//        if (hex)
//        {
//            char *p = hex;
//            p += sprintf(p, "Device error\r\n");
//            for (uint32_t i = 0; i < length; i++)
//            {
//                if (isprint(data[i]))
//                {
//                    *p = data[i];
//                    p++;
//                    *p = 0x00; // insert null
//                }
//                else
//                {
//                    p += sprintf(p, "<%02x>", data[i]);
//                }
//            }
//            p += sprintf(p, "\r\n");
//            vdm_sdcard_log_to_file(folder, "log.txt", hex);
//            vPortFree(hex);
//        }
//    }
//    else if (VDM_APP_GSM_MSG_ID_PERIODIC_UPDATE_DATA != cmd)
//    {
//        DEBUG_WARN("TODO : Store message id %02X to sdcard\r\n", cmd);
//    }
//}

void vdm_app_gsm_send_door_frame(bool door_close)
{
#if 1
    rtc_date_time_t time;
    get_time(&time);

    if (door_close == false)
    {
        uint32_t size_of_frame = sizeof(vdm_app_gsm_door_open_frame_t);

        // Insert timestamp to message
        SET_MESSAGE_TIME(m_door_open_frame, time);

        /* Calculate crc */
        uint32_t crc_size = offsetof(vdm_app_gsm_door_open_frame_t, crc) - offsetof(vdm_app_gsm_door_open_frame_t, split_0);
        m_door_open_frame.crc = crc_calculator_xmodem((uint8_t *)&m_door_open_frame.split_0,
                                                      crc_size);

        /* This is not an critical frame, store in FRAM */
        send_frame(VDM_APP_GSM_MAIN_SERVER,
                   m_door_open_frame.cmd,
                   (uint8_t *)&m_door_open_frame,
                   size_of_frame);
    }
    else
    {
        uint32_t size_of_frame = sizeof(vdm_app_gsm_door_close_frame_t);
        sys_ctx_t *ctx = sys_ctx();
        vdm_device_config_t *device_config = vdm_get_device_config();

        /* Led & Temperature & Drop sensor config */

        m_door_close_frame.relay1_state = IDM_Status.isHeater//device_config->peripheral.feature_enable.name.led_enable
                                     ? VDM_APP_RELAY1_ON
                                     : VDM_APP_RELAY1_OFF;
        m_door_close_frame.huminity_set = IDM.HumidityMAX;//device_config->peripheral.temperature;
        m_door_close_frame.drop_sensor = VDM_APP_GSM_DROP_SENSOR_IS_ON;//device_config->peripheral.feature_enable.name.drop_sensor_enable
                                         //    ? VDM_APP_GSM_DROP_SENSOR_IS_ON
                                         //   : VDM_APP_GSM_DROP_SENSOR_IS_OFF;

        /* Item price */
        for (uint32_t i = 0; i < sizeof(m_door_close_frame.item_price); i++)
        {
            //	    	m_door_close_frame.item_price[i] = vdm_device_config_get_item_price(i+1)/1000;
            m_door_close_frame.item_price[i] = (vdm_device_config_get_item_price(i) / 1000)&0xFF;
        }

        /* Number of item per rack */
        for (uint32_t i = 0; i < sizeof(m_door_close_frame.nb_of_items_rack); i++)
        {
            //	    	m_door_close_frame.nb_of_items_rack[i] = vdm_device_config_get_item_remain(i+1);
            m_door_close_frame.nb_of_items_rack[i] = 0;//vdm_device_config_get_item_remain(i);
        }
				//added hiendv
				if(IDM.currentNumberBuyMore >0)
					m_door_close_frame.nb_of_items_rack[0] = 0x01;
				else
					m_door_close_frame.nb_of_items_rack[0] = 0x00;
				if(!IDM_Status.isEmptyIsenseSW)	m_door_close_frame.nb_of_items_rack[0] = 0xFF;
				// end added
				
        // NV11
        // Clear all return money
        memset(m_door_close_frame.money_payback, 0, sizeof(m_door_close_frame.money_payback));

        if (NV11_GetLatestEvent(true) != NV11_ERROR)
        {
            /* Neu la dau doc NV11, byte dia chi 27 se chua so to tien duoc luu tru trong Floatnote = tong so tien/ menh gia */
            /* byte |  0  |  1   |   2  |   3   |   4   |   5   |   6   |    7   |    8  | */
            /* Note | 1000| 2000 | 5000 | 10000 | 20000 | 50000 | 100000| 200000 | 500000 | */
            uint8_t note_number = 0;
            if (NV11_GetUnitType(true) == NV_TYPE_NV11)
            {
                /* Tinh so to tien tren FloatNote (tra lai) luu vao byte dia chi 27 */
                note_number = NV11_GetTotalCashInNoteFloat(true) / NV11_GetDenominationForChange(true);
                switch (NV11_GetDenominationForChange(true))
                {
                case 1000:
                    m_door_close_frame.money_payback[0] = note_number;
                    break;
                case 2000:
                    m_door_close_frame.money_payback[1] = note_number;
                    break;
                case 5000:
                    m_door_close_frame.money_payback[2] = note_number;
                    break;
                case 10000:
                    m_door_close_frame.money_payback[3] = note_number;
                    break;
                case 20000:
                    m_door_close_frame.money_payback[4] = note_number;
                    break;
                case 50000:
                    m_door_close_frame.money_payback[5] = note_number;
                    break;
                case 100000:
                    m_door_close_frame.money_payback[6] = note_number;
                    break;
                case 200000:
                    m_door_close_frame.money_payback[7] = note_number;
                    break;
                case 500000:
                    m_door_close_frame.money_payback[8] = note_number;
                    break;

                default:
                    break;
                }
            }
            /* Neu la dau doc NV200, byte tu 25 - 33 se luu tru so to tien tung menh gia trong Floatnote */
            else if (NV11_GetUnitType(true) == NV_TYPE_NV200)
            {
                for (uint32_t channel_index = 0; channel_index < 9; channel_index++)
                {
                    m_door_close_frame.money_payback[channel_index] = NV11_GetStoredNoteByChannel(channel_index); /* The number of change notes */
                }
            }
						
						//added Hiendv
						for (uint32_t channel_index = 0; channel_index < 9; channel_index++)
						{
								m_door_close_frame.money_payback[channel_index] = 0;//NV11_GetStoredNoteByChannel(channel_index); /* The number of change notes */
						}						
        }
        else
        {
            DEBUG_WARN("[%s] Last nv11 event is error\r\n", __FUNCTION__);
        }

        // Current date time
        SET_MESSAGE_TIME(m_door_close_frame, time);

        /* Calculate crc */
        uint32_t crc_size = offsetof(vdm_app_gsm_door_close_frame_t, crc) - offsetof(vdm_app_gsm_door_close_frame_t, split_0);
        m_door_close_frame.crc = crc_calculator_xmodem((uint8_t *)&m_door_close_frame.split_0,
                                                       crc_size);

        /* This is not an critical frame, store in FRAM */
        send_frame(VDM_APP_GSM_MAIN_SERVER,
                   m_door_close_frame.cmd,
                   (uint8_t *)&m_door_close_frame,
                   size_of_frame);
    }
#endif
}

//void vdm_app_gsm_send_motor_clear_all_error_frame(void) // Send Motor Resume Frame
//{
//    rtc_date_time_t time;
//    get_time(&time);

//    m_error_frame.id = VDM_APP_GSM_ID_MOTOR(1);
//    m_error_frame.error_status = VDM_APP_GSM_FIXED_ERROR;
//    memcpy(m_error_frame.machine_id, "TPA", 3);

//    // Insert timestamp to message
//    SET_MESSAGE_TIME(m_error_frame, time);

//    uint32_t crc_size = offsetof(vdm_app_gsm_notify_error_frame_t, crc) - offsetof(vdm_app_gsm_notify_error_frame_t, split_0);
//    m_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_error_frame.split_0,
//                                              crc_size);

//    /* Luu tru du lieu vao ROM */
//    send_frame(VDM_APP_GSM_MAIN_SERVER, m_error_frame.cmd, (uint8_t *)&m_error_frame, sizeof(m_error_frame));
//}

//void vdm_app_gsm_send_motor_error_frame(uint8_t motor_id) // SendMotorErrorFrame
//{
//    rtc_date_time_t time;
//    get_time(&time);
//    // Insert timestamp to message

//    m_error_frame.id = motor_id;
//    m_error_frame.error_status = VDM_APP_GSM_NOT_FIXED_ERROR;

//    SET_MESSAGE_TIME(m_error_frame, time);

//    uint32_t crc_size = offsetof(vdm_app_gsm_notify_error_frame_t, crc) - offsetof(vdm_app_gsm_notify_error_frame_t, split_0);

//    m_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_error_frame.split_0,
//                                              crc_size);

//    /* Luu du lieu vao ROM */
//    send_frame(VDM_APP_GSM_MAIN_SERVER, m_error_frame.cmd, (uint8_t *)&m_error_frame, sizeof(m_error_frame));
//}

///**
// * @brief   Ham tao frame bao da sua RTC va luu tru du lieu EEPROM
// * @param   NONE
// * @retval  NONE
// */
void vdm_app_gsm_send_rtc_resume_frame()
{
    rtc_date_time_t time;
    get_time(&time);

    m_error_frame.id = VDM_APP_GSM_ID_RTC;
    m_error_frame.error_status = VDM_APP_GSM_FIXED_ERROR;

    // Insert timestamp to message
    SET_MESSAGE_TIME(m_error_frame, time);

    uint32_t crc_size = offsetof(vdm_app_gsm_notify_error_frame_t, crc) - offsetof(vdm_app_gsm_notify_error_frame_t, split_0);

    m_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_error_frame.split_0,
                                              crc_size);

    /* Luu du lieu vao ROM */
    send_frame(VDM_APP_GSM_MAIN_SERVER,
               m_error_frame.cmd,
               (uint8_t *)&m_error_frame,
               sizeof(m_error_frame));
}

///**
// * @brief   Ham tao frame bao sua xong EEPROM va luu tru du lieu RAM
// * @param   NONE
// * @retval  NONE
// */
//void vdm_app_gsm_send_storage_resume_frame()
//{
//    rtc_date_time_t time;
//    get_time(&time);

//    // Insert timestamp to message
//    m_error_frame.id = VDM_APP_GSM_ID_STORAGE;
//    m_error_frame.error_status = VDM_APP_GSM_FIXED_ERROR;

//    SET_MESSAGE_TIME(m_error_frame, time);

//    uint32_t crc_size = offsetof(vdm_app_gsm_notify_error_frame_t, crc) - offsetof(vdm_app_gsm_notify_error_frame_t, split_0);

//    m_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_error_frame.split_0,
//                                              crc_size);

//    /* Luu du lieu vao RAM */
//    send_frame(VDM_APP_GSM_MAIN_SERVER, m_error_frame.cmd, (uint8_t *)&m_error_frame, sizeof(m_error_frame));
//}

///**
// * @brief   Ham tao frame bao loi the nho va luu tru du lieu ROM
// * @param   NONE
// * @retval  NONE
// */
void vdm_app_gsm_send_sdcard_frame(bool error)
{
    rtc_date_time_t time;
    get_time(&time);

    // Insert timestamp to message
    m_error_frame.id = VDM_APP_GSM_ID_SDCARD;
    m_error_frame.error_status = error ? VDM_APP_GSM_NOT_FIXED_ERROR : VDM_APP_GSM_FIXED_ERROR;

    SET_MESSAGE_TIME(m_error_frame, time);

    uint32_t crc_size = offsetof(vdm_app_gsm_notify_error_frame_t, crc) - offsetof(vdm_app_gsm_notify_error_frame_t, split_0);
    m_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_error_frame.split_0,
                                              crc_size);

    /* Luu du lieu vao RAM */
    send_frame(VDM_APP_GSM_MAIN_SERVER, m_error_frame.cmd, (uint8_t *)&m_error_frame, sizeof(m_error_frame));
}

static vdm_app_gsm_server_confirm_cb_t m_confirm_cb = NULL;
//void vdm_app_gsm_on_register_confirm_callback(vdm_app_gsm_server_confirm_cb_t callback)
//{
//    m_confirm_cb = callback;
//}

bool process_confirm_msg(char *data)
{
    vdm_app_server_confirm_frame_t *confirm_msg = (vdm_app_server_confirm_frame_t *)data;
    uint32_t crc_size = offsetof(vdm_app_server_confirm_frame_t, crc) - offsetof(vdm_app_server_confirm_frame_t, split_0);
    uint16_t crc = crc_calculator_xmodem((uint8_t *)&confirm_msg->split_0,
                                         crc_size);
    // Swap CRC
    uint16_t tmp = crc & 0xFF;
    tmp <<= 8;
    tmp |= crc >> 8;
    crc = tmp;
    bool retval = false;
    if (crc == confirm_msg->crc)
    {
        // TODO check more argument, due to me laziness, i dont check it
        if (confirm_msg->cmd == VDM_APP_GSM_SERVER_CONFIRM_SUCCESS)
        {
            DEBUG_VERBOSE("Server confirmed on %04u:%02u:%02u %02u:%02u:%02u\r\n",
                          confirm_msg->year + 2000,
                          confirm_msg->month,
                          confirm_msg->day,
                          confirm_msg->hour,
                          confirm_msg->minutes,
                          confirm_msg->sec);
            if (m_confirm_cb)
            {
                m_confirm_cb(confirm_msg, sizeof(vdm_app_server_confirm_frame_t));
            }
            retval = true;
        }
        else if (confirm_msg->cmd == VDM_APP_GSM_SERVER_CONFIRM_FAILED)
        {
            DEBUG_WARN("Wrong message, maybe machine send wrong data to server\r\n");
            retval = true;
        }
        else
        {
            DEBUG_WARN("Unhandled confirm id %u\r\n", confirm_msg->cmd);
        }
    }
    else
    {
        DEBUG_WARN("confirm msg wrong crc %04X-%04X\r\n", confirm_msg->crc, crc);
    }
    return retval;
}

//bool process_ota(char *data)
//{
//    bool retval = true;
//    vdm_app_server_ota_frame_t *ota_msg = (vdm_app_server_ota_frame_t *)data;

//    uint32_t crc_size = offsetof(vdm_app_server_ota_frame_t, crc) - offsetof(vdm_app_server_ota_frame_t, split_0);
//    uint16_t crc = crc_calculator_xmodem((uint8_t *)&ota_msg->split_0,
//                                         crc_size);
//    // Swap CRC
//    uint16_t tmp = crc & 0xFF;
//    tmp <<= 8;
//    tmp |= crc >> 8;
//    crc = tmp;
//    if (crc == ota_msg->crc && ota_msg->cmd == VDM_APP_GSM_SERVER_REQUEST_OTA && sys_ctx()->status.ota_file_download_to_gsm_rom == 0) // If firmware already downloaded to ROM, we donot need to download again
//    {
//        uint32_t old_version = VDM_FIRMWARE_VERSION_MAJOR * 100 + VDM_FIRMWARE_VERSION_MINOR * 10 + VDM_FIRMWARE_VERSION_BUILD;
//        uint32_t new_version = ((uint32_t)ota_msg->version[0]) * 100 + ((uint32_t)ota_msg->version[1]) * 10 + ((uint32_t)ota_msg->version[2]);
//        if (new_version > old_version)
//        {
//            sprintf(sys_ctx()->status.ota_url, "VM_%u.%u.%u.bin", ota_msg->version[0], ota_msg->version[1], ota_msg->version[2]);
//            sys_ctx()->status.ota_update_method = SYS_CTX_OTA_METHOD_FTP; // test
//            DEBUG_INFO("OTA file %s\r\n", sys_ctx()->status.ota_url);
//        }
//        else
//        {
//            DEBUG_WARN("OTA version is invalid, old version %u, new version %u\r\n", old_version, new_version);
//        }
//    }
//    else
//    {
//        retval = false;
//    }
//    return retval;
//}

//bool process_turnoff_warning(char *data)
//{
//    bool retval = true;
//    vdm_app_server_turn_off_warning_t *warning_msg = (vdm_app_server_turn_off_warning_t *)data;

//    uint32_t crc_size = offsetof(vdm_app_server_turn_off_warning_t, crc) - offsetof(vdm_app_server_turn_off_warning_t, split_0);
//    uint16_t crc = crc_calculator_xmodem((uint8_t *)&warning_msg->split_0,
//                                         crc_size);
//    // Swap CRC
//    uint16_t tmp = crc & 0xFF;
//    tmp <<= 8;
//    tmp |= crc >> 8;
//    crc = tmp;
//    if (crc == warning_msg->crc && warning_msg->crc == VDM_APP_GSM_SERVER_TURN_OFF_WARNING)
//    {
//        DEBUG_INFO("Valid message turn off warning, implement later\r\n");
//    }
//    else
//    {
//        retval = false;
//    }
//    return retval;
//}

//void vdm_app_gsm_parse_new_server_msg(uint8_t *data, uint32_t length)
//{
//    // Hien tai do dai cac ban tin server khac nhau
//    // Do minh luoi nen minh se parse message dua vao do dai ban tin
//    // No co kha nang loi khi tuong lai co cac ban tin giong nhau ve do dai
//    // Cach sua la parse header truoc
//    if (!data || !length)
//    {
//        DEBUG_WARN("Invalid server msg\r\n");
//        return;
//    }
//    DEBUG_INFO("Server cmd len %u\r\n", length);

//    if (length == sizeof(vdm_app_server_confirm_frame_t))
//    {
//        process_confirm_msg(data);
//    }
//    else if (length == sizeof(vdm_app_server_turn_off_warning_t))
//    {
//        process_turnoff_warning(data);
//    }
//    else if (length == sizeof(vdm_app_server_ota_frame_t))
//    {
//        DEBUG_INFO("OTA\r\n");
//        process_ota(data);
//    }
//    else
//    {
//        DEBUG_WARN("Unhandle server frame, maybe server send more than 1 msg in 1 frame\r\n");
//        uint32_t search_len = length;
//        char header[19];
//        memset(header, 0, sizeof(header));
//        sprintf(header, "%s", "###|");
//        memcpy(header + 4, m_device_id, 10);
//        char *p = data;
//        char *q = data;
//        while (p && q)
//        {
//            // Find header ###TPA|xxxxxx|CMD=|
//            p = strstr(p, header);
//            if (p == NULL)
//            {
//                break;
//            }
//            search_len -= strlen(header);
//            q = strstr(p + strlen(header), "CMD=|");
//            if (q == NULL)
//            {
//                break;
//            }
//            q += strlen("CMD=|");
//            uint32_t msg_id = *q;
//            switch (msg_id)
//            {
//            case VDM_APP_GSM_SERVER_REQUEST_OTA:
//            {
//                DEBUG_INFO("Server request OTA\r\n");
//                process_ota(p);
//                p += sizeof(vdm_app_server_ota_frame_t);
//                if ((uint8_t *)p - (uint8_t *)data >= length)
//                {
//                    DEBUG_INFO("No more data\r\n");
//                    p = NULL;
//                    q = NULL;
//                }
//            }
//            break;

//            case VDM_APP_GSM_SERVER_CONFIRM_SUCCESS:
//            {
//                DEBUG_INFO("Server confirm success\r\n");
//                process_confirm_msg(p);
//                p += sizeof(vdm_app_server_confirm_frame_t);
//                if ((uint8_t *)p - (uint8_t *)data >= length)
//                {
//                    DEBUG_INFO("No more data\r\n");
//                    p = NULL;
//                    q = NULL;
//                }
//            }
//            break;
//            case VDM_APP_GSM_SERVER_CONFIRM_FAILED:
//                DEBUG_INFO("Server confirm failed\r\n");
//                process_confirm_msg(p);
//                p += sizeof(vdm_app_server_confirm_frame_t);
//                if ((uint8_t *)p - (uint8_t *)data >= length)
//                {
//                    DEBUG_INFO("No more data\r\n");
//                    p = NULL;
//                    q = NULL;
//                }
//                break;

//            case VDM_APP_GSM_SERVER_TURN_OFF_WARNING:
//                process_turnoff_warning(p);
//                p += sizeof(vdm_app_server_turn_off_warning_t);
//                if ((uint8_t *)p - (uint8_t *)data >= length)
//                {
//                    DEBUG_INFO("No more data\r\n");
//                    p = NULL;
//                    q = NULL;
//                }
//                break;
//            default:
//                DEBUG_ERROR("Unhandled server cmd %02x\r\n", msg_id);
//                p = NULL;
//                q = NULL;
//                break;
//            }
//        }
//    }
//}



void vdm_app_gsm_set_device_id(uint8_t *device_id)
{
    memcpy(m_device_id, "TPA", 3);
    for (uint32_t i = 0; i < 7; i++)
    {
        m_device_id[3 + i] = device_id[i] + '0';
    }

    memcpy(m_error_frame.machine_id, m_device_id, 10);
    memcpy(m_door_close_frame.machine_id, m_device_id, 10);
    memcpy(m_door_open_frame.machine_id, m_device_id, 10);
    memcpy(m_paid_out_item_frame.machine_id, m_device_id, 10);
    memcpy(m_heartbeat_frame.machine_id, m_device_id, 10);
    memcpy(m_released_item_frame.machine_id, m_device_id, 10);
    memcpy(m_selling_error_frame.machine_id, m_device_id, 10);
		memcpy(m_start_frame.machine_id, m_device_id, 10);
		memcpy(m_qrm_sale_error_frame.machine_id, m_device_id, 10);
		printf("IDM>> ID Machine: %s\r\n",m_device_id);
}

void vdm_app_gsm_send_start_frame(void)
{
    uint32_t crc_size = offsetof(vdm_app_gsm_start_frame_t, crc) - offsetof(vdm_app_gsm_start_frame_t, split_0);
    m_start_frame.crc = crc_calculator_xmodem((uint8_t *)&m_start_frame.split_0,
                                                      crc_size);

    send_frame(VDM_APP_GSM_MAIN_SERVER, m_start_frame.cmd, (uint8_t *)&m_start_frame, sizeof(m_start_frame));
}

void vdm_app_gsm_send_selling_error_frame(uint8_t received_cash, uint8_t error_slot_number)
{
    rtc_date_time_t time;
    get_time(&time);

    m_selling_error_frame.input_cash = received_cash;
    m_selling_error_frame.slot_error_number = error_slot_number;

    // Insert timestamp to message
    SET_MESSAGE_TIME(m_selling_error_frame, time);

    uint32_t crc_size = offsetof(vdm_app_gsm_selling_error_frame_t, crc) - offsetof(vdm_app_gsm_selling_error_frame_t, split_0);
    m_selling_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_selling_error_frame.split_0,
                                                      crc_size);

    send_frame(VDM_APP_GSM_MAIN_SERVER, m_selling_error_frame.cmd, (uint8_t *)&m_selling_error_frame, sizeof(m_selling_error_frame));
}

//sell susscess
void vdm_app_gsm_send_released_item_frame(uint8_t received_cash, uint8_t slot_number)
{
    DEBUG_INFO("Send release frame\r\n");
    rtc_date_time_t time;
    get_time(&time);

    m_released_item_frame.input_cash = received_cash;
    m_released_item_frame.slot_number = slot_number;

    // Insert timestamp to message
    SET_MESSAGE_TIME(m_released_item_frame, time);

    uint32_t crc_size = offsetof(vdm_app_gsm_released_item_frame_t, crc) - offsetof(vdm_app_gsm_released_item_frame_t, split_0);
    m_released_item_frame.crc = crc_calculator_xmodem((uint8_t *)&m_released_item_frame.split_0,
                                                      crc_size);

    send_frame(VDM_APP_GSM_MAIN_SERVER, m_released_item_frame.cmd, (uint8_t *)&m_released_item_frame, sizeof(m_released_item_frame));
}

//void vdm_app_gsm_send_paid_out_item_frame(uint8_t paid_out)
//{
//    DEBUG_INFO("Send paidout frame\r\n");
//    rtc_date_time_t time;
//    get_time(&time);

//    m_paid_out_item_frame.refund_cash = paid_out;

//    // Insert timestamp to message
//    SET_MESSAGE_TIME(m_paid_out_item_frame, time);

//    uint32_t crc_size = offsetof(vdm_app_gsm_paid_out_item_frame_t, crc) - offsetof(vdm_app_gsm_paid_out_item_frame_t, split_0);
//    m_paid_out_item_frame.crc = crc_calculator_xmodem((uint8_t *)&m_paid_out_item_frame.split_0,
//                                                      crc_size);

//    send_frame(VDM_APP_GSM_MAIN_SERVER, m_paid_out_item_frame.cmd, (uint8_t *)&m_paid_out_item_frame, sizeof(m_paid_out_item_frame));
//}


extern IDM_HARDWARE IDM_Status;
extern float Humidity, Temperature;

void vdm_app_gsm_send_heartbeat()
{
#if 1
    rtc_date_time_t time;
    get_time(&time);

    bool nv11_error = (NV11_GetLatestEvent(true) == NV11_ERROR) ? 1 : 0;

    m_heartbeat_frame.door = IDM_Status.isDoorOpen ? VDM_APP_GSM_DOOR_OPEN : VDM_APP_GSM_DOOR_CLOSE;
    m_heartbeat_frame.huminity = (int8_t)Humidity;
    m_heartbeat_frame.nv11_error = nv11_error ? VDM_APP_GSM_ERROR : VDM_APP_GSM_OK;
    memset(m_heartbeat_frame.cash_info, 0, 9); // If nv11 error  =>> Set all cash value to zero
    if (nv11_error == false)                   // NV11 ok, get all value
    {
        /* Neu dau doc la NV11 -> Chi co 1 menh gia tra lai */
        /* byte |  25 |  26  |  27  |   28  |   29  |  30   |   31  |   32   |    33  | */
        /* Note | 1000| 2000 | 5000 | 10000 | 20000 | 50000 | 100000| 200000 | 500000 |*/
        if (NV11_GetUnitType(true) == NV_TYPE_NV11)
        {
            /* Tinh so to tien tren FloatNote (tra lai) luu vao byte dia chi 27 */
            uint8_t note_number = NV11_GetAvailableChange(true) / NV11_GetDenominationForChange(true);
            switch (NV11_GetDenominationForChange(true))
            {
            case 1000:
                m_heartbeat_frame.cash_info[0] = note_number;
                break;
            case 2000:
                m_heartbeat_frame.cash_info[1] = note_number;
                break;
            case 5000:
                m_heartbeat_frame.cash_info[2] = note_number;
                break;
            case 10000:
                m_heartbeat_frame.cash_info[3] = note_number;
                break;
            case 20000:
                m_heartbeat_frame.cash_info[4] = note_number;
                break;
            case 50000:
                m_heartbeat_frame.cash_info[5] = note_number;
                break;
            case 100000:
                m_heartbeat_frame.cash_info[6] = note_number;
                break;
            case 200000:
                m_heartbeat_frame.cash_info[7] = note_number;
                break;
            case 500000:
                m_heartbeat_frame.cash_info[8] = note_number;
                break;
            default:
                break;
            }
        }
        /* Neu dau doc la NV200 -> Co nhieu menh gia tra lai */
        else if (NV11_GetUnitType(true) == NV_TYPE_NV200)
        {
            /* Tinh so to tien cua moi menh gia tren FloatNote (tra lai) luu vao dia chi tu 25 ~ 33 */
            for (uint32_t channel = 0; channel < 9; channel++)
            {
                m_heartbeat_frame.cash_info[channel] = NV11_GetStoredNoteByChannel(channel); /* The number of change notes */
            }
        }
				//added hiendv = 0 for insence
				for (uint32_t channel = 0; channel < 9; channel++)
				{
						m_heartbeat_frame.cash_info[channel] = 0;//NV11_GetStoredNoteByChannel(channel); /* The number of change notes */
				}
				//end added
    }

    /* So hang hoa hien tai tren tung khay */
    for (uint32_t i = 0; i < 60; i++)
    {
        /* Lay so luong hang tren tung khay luu vao (60 khay)*/
        m_heartbeat_frame.item_remain[i] = vdm_device_config_get_item_remain(i);
    }
		
		//added hiendv
				if(IDM.currentNumberBuyMore >0)
					m_door_close_frame.nb_of_items_rack[0] = 0x01;
				else
					m_door_close_frame.nb_of_items_rack[0] = 0x00;
				if(!IDM_Status.isEmptyIsenseSW)	m_door_close_frame.nb_of_items_rack[0] = 0xFF;
		// and added

    // Insert timestamp to message
    SET_MESSAGE_TIME(m_heartbeat_frame, time);

    uint32_t crc_size = offsetof(vdm_app_gsm_heartbeat_frame_t, crc) - offsetof(vdm_app_gsm_heartbeat_frame_t, split_0);
    uint16_t tmp_crc = crc_calculator_xmodem((uint8_t *)&m_heartbeat_frame.split_0,
                                             crc_size);

    // Expected crc = = 0x9D62 =>> but due to memory organization, m_heartbeat_frame struct has crc = 0x629D
    // =>> We need to swap crc low and high byte
    m_heartbeat_frame.crc = (tmp_crc & 0xFF) << 8;
    m_heartbeat_frame.crc |= tmp_crc >> 8;

    send_frame(VDM_APP_GSM_MAIN_SERVER, m_heartbeat_frame.cmd, (uint8_t *)&m_heartbeat_frame, sizeof(m_heartbeat_frame));
#else
    rtc_date_time_t time;
    get_time(&time);

    bool nv11_error = (NV11_GetLatestEvent(true) == NV11_ERROR) ? 1 : 0;
    memcpy(m_heartbeat_frame.machine_id, "TPA0123456", 10);
    m_heartbeat_frame.door = 0x31;
    m_heartbeat_frame.temp = 27;
    m_heartbeat_frame.nv11_error = 0x30;
    memset(m_heartbeat_frame.cash_info, 0, 9); // If nv11 error  =>> Set all cash value to zero
    if (0)                                     // NV11 ok, get all value
    {
        /* Neu dau doc la NV11 -> Chi co 1 menh gia tra lai */
        /* byte |  25 |  26  |  27  |   28  |   29  |  30   |   31  |   32   |    33  | */
        /* Note | 1000| 2000 | 5000 | 10000 | 20000 | 50000 | 100000| 200000 | 500000 |*/
        if (NV11_GetUnitType(true) == NV_TYPE_NV11)
        {
            /* Tinh so to tien tren FloatNote (tra lai) luu vao byte dia chi 27 */
            uint8_t note_number = NV11_GetAvailableChange(true) / NV11_GetDenominationForChange(true);
            switch (NV11_GetDenominationForChange(true))
            {
            case 1000:
                m_heartbeat_frame.cash_info[0] = note_number;
                break;
            case 2000:
                m_heartbeat_frame.cash_info[1] = note_number;
                break;
            case 5000:
                m_heartbeat_frame.cash_info[2] = note_number;
                break;
            case 10000:
                m_heartbeat_frame.cash_info[3] = note_number;
                break;
            case 20000:
                m_heartbeat_frame.cash_info[4] = note_number;
                break;
            case 50000:
                m_heartbeat_frame.cash_info[5] = note_number;
                break;
            case 100000:
                m_heartbeat_frame.cash_info[6] = note_number;
                break;
            case 200000:
                m_heartbeat_frame.cash_info[7] = note_number;
                break;
            case 500000:
                m_heartbeat_frame.cash_info[8] = note_number;
                break;
            default:
                break;
            }
        }
        /* Neu dau doc la NV200 -> Co nhieu menh gia tra lai */
        else if (NV11_GetUnitType(true) == NV_TYPE_NV200)
        {
            /* Tinh so to tien cua moi menh gia tren FloatNote (tra lai) luu vao dia chi tu 25 ~ 33 */
            for (uint32_t channel = 0; channel < 9; channel++)
            {
                m_heartbeat_frame.cash_info[channel] = NV11_GetStoredNoteByChannel(channel); /* The number of change notes */
            }
        }
    }

    /* So hang hoa hien tai tren tung khay */
    for (uint32_t i = 0; i < 60; i++)
    {
        /* Lay so luong hang tren tung khay luu vao (60 khay)*/
        m_heartbeat_frame.item_remain[i] = 0;
    }

    // Insert timestamp to message
    m_heartbeat_frame.year = 21;
    m_heartbeat_frame.month = 9;
    m_heartbeat_frame.day = 22;
    m_heartbeat_frame.hour = 17;
    m_heartbeat_frame.minutes = 30;
    m_heartbeat_frame.sec = 15;

    uint32_t crc_size = offsetof(vdm_app_gsm_heartbeat_frame_t, crc) - offsetof(vdm_app_gsm_heartbeat_frame_t, split_0);
    uint16_t tmp_crc = crc_calculator_xmodem((uint8_t *)&m_heartbeat_frame.split_0,
                                             crc_size);

    // Expected crc = = 0x9D62 =>> but due to memory organization, m_heartbeat_frame struct has crc = 0x629D
    // =>> We need to swap crc low and high byte
    m_heartbeat_frame.crc = (tmp_crc & 0xFF) << 8;
    m_heartbeat_frame.crc |= tmp_crc >> 8;

    send_frame(VDM_APP_GSM_MAIN_SERVER, m_heartbeat_frame.cmd, (uint8_t *)&m_heartbeat_frame, sizeof(m_heartbeat_frame));
#endif
}

//void vdm_app_gsm_send_qrm_error_frame(void)
//{
//    m_error_frame.id = VDM_APP_GSM_ID_QRM_ERROR;
//    m_error_frame.error_status = VDM_APP_GSM_FIXED_ERROR;
//    rtc_date_time_t time;
//    get_time(&time);

//    // Insert timestamp to message
//    SET_MESSAGE_TIME(m_error_frame, time);

//    uint32_t crc_size = offsetof(vdm_app_gsm_notify_error_frame_t, crc) - offsetof(vdm_app_gsm_notify_error_frame_t, split_0);

//    m_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_error_frame.split_0,
//                                              crc_size);

//    /* Luu du lieu vao ROM */
//    send_frame(VDM_APP_GSM_MAIN_SERVER, m_error_frame.cmd, (uint8_t *)&m_error_frame, sizeof(m_error_frame));
//}

//void vdm_app_gsm_send_qrm_resume_frame(void)
//{
//    m_error_frame.id = VDM_APP_GSM_ID_QRM_ERROR;
//    m_error_frame.error_status = VDM_APP_GSM_NOT_FIXED_ERROR;
//    rtc_date_time_t time;
//    get_time(&time);

//    // Insert timestamp to message
//    SET_MESSAGE_TIME(m_error_frame, time);

//    uint32_t crc_size = offsetof(vdm_app_gsm_notify_error_frame_t, crc) - offsetof(vdm_app_gsm_notify_error_frame_t, split_0);

//    m_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_error_frame.split_0,
//                                              crc_size);

//    /* Luu du lieu vao ROM */
//    send_frame(VDM_APP_GSM_MAIN_SERVER, m_error_frame.cmd, (uint8_t *)&m_error_frame, sizeof(m_error_frame));
//}

//void vdm_app_gsm_send_qrm_sale_success_frame(uint8_t money, uint8_t supplier, uint8_t index)
//{
//    rtc_date_time_t time;
//    get_time(&time);

//    // Insert timestamp to message
//    m_qrm_sale_success_frame.money = money;
//    m_qrm_sale_success_frame.supplier = supplier;
//    m_qrm_sale_success_frame.index = index;

//    SET_MESSAGE_TIME(m_qrm_sale_success_frame, time);

//    uint32_t crc_size = offsetof(vdm_app_gsm_qr_sell_success_frame_t, crc) - offsetof(vdm_app_gsm_qr_sell_success_frame_t, split_0);

//    m_qrm_sale_success_frame.crc = crc_calculator_xmodem((uint8_t *)&m_qrm_sale_success_frame.split_0,
//                                                         crc_size);

//    /* Luu du lieu vao ROM */
//    send_frame(VDM_APP_GSM_MAIN_SERVER,
//               m_qrm_sale_success_frame.cmd,
//               (uint8_t *)&m_qrm_sale_success_frame,
//               sizeof(m_qrm_sale_success_frame));
//}

void vdm_app_gsm_send_qrm_sale_fail_frame(uint8_t money, uint8_t supplier, uint8_t index)
{
    rtc_date_time_t time;
    get_time(&time);

    // Insert timestamp to message
    m_qrm_sale_error_frame.money = money;
    m_qrm_sale_error_frame.supplier = supplier;
    m_qrm_sale_error_frame.index = index;

    SET_MESSAGE_TIME(m_qrm_sale_error_frame, time);

    uint32_t crc_size = offsetof(vdm_app_gsm_qr_sell_fail_frame_t, crc) - offsetof(vdm_app_gsm_qr_sell_fail_frame_t, split_0);

    m_qrm_sale_error_frame.crc = crc_calculator_xmodem((uint8_t *)&m_qrm_sale_error_frame.split_0,
                                                       crc_size);

    /* Luu du lieu vao ROM */
    send_frame(VDM_APP_GSM_MAIN_SERVER, m_qrm_sale_error_frame.cmd, (uint8_t *)&m_qrm_sale_error_frame, sizeof(m_qrm_sale_error_frame));
}

//static AT_NONCACHEABLE_SECTION_INIT(uint8_t m_uart_rx[64]) = {0};
//// uint8_t m_uart_rx[64] = {0};
//static void gsm_thread(void *arg)
//{
//    GPIO_WritePinOutput(BOARD_INITPINS_GSM_PWR_KEY_PORT, BOARD_INITPINS_GSM_PWR_KEY_GPIO_PIN, 0);
//#if GSM_HAS_POWER_CTRL
//    GPIO_WritePinOutput(BOARD_INITPINS_GSM_PWR_EN_PORT, BOARD_INITPINS_GSM_PWR_EN_PIN, 0); // gsm power 1 : turn on, 0 turn off
//#endif
//    gsm_change_state(GSM_STATE_RESET);
//    sys_ctx_t *m_sys_ctx = sys_ctx();
//    for (;;)
//    {
//        // Read data in gsm rx buffer
//        uint32_t nb_of_rx;
//        nb_of_rx = gsm_uart_read(m_uart_rx, 64);

//        if (m_sys_ctx->status.gsm_transparency == 0)
//        {
//            // debug only
//            for (uint32_t i = 0; i < nb_of_rx; i++)
//            {
//                gsm_uart_handler(m_uart_rx[i]);
//                if (sys_ctx()->status.ota_update_method == SYS_CTX_OTA_METHOD_INVALID)
//                {
//                    if (isprint(m_uart_rx[i]))
//                    {
//                        DEBUG_RAW("%c", m_uart_rx[i]);
//                    }
//                    else
//                    {
//                        DEBUG_RAW("<%02X>", m_uart_rx[i]);
//                    }
//                }
//            }
//            if (nb_of_rx > 2 && m_uart_rx[nb_of_rx - 2] == '\r' && m_uart_rx[nb_of_rx - 1] == '\n')
//            {
//                DEBUG_RAW("\r\n");
//            }
//            gsm_hw_polling_task();
//            gsm_state_polling_task();
//        }
//        else
//        {
//            static uint8_t found_plus = 0;
//            for (uint32_t i = 0; i < nb_of_rx; i++)
//            {
//                // gsm_uart_handler(m_uart_rx[i]);
//                // DEBUG_RAW("%c", m_uart_rx[i]);
//            }

//            int32_t tmp = kStatus_Success;
//            //            while (tmp == kStatus_Success)
//            //            {
//            //                uint8_t ch;
//            ////                tmp = DbgConsole_TryGetchar(&ch);
//            //                if (tmp == kStatus_Success)
//            //                {
//            //                    gsm_uart_transfer(&ch, 1, 1);
//            //                    DEBUG_RAW("%c", ch);
//            //                    if (ch == '+')
//            //                    {
//            //                        found_plus++;
//            //                    }
//            //                    else
//            //                    {
//            //                        found_plus = 0;
//            //                    }
//            //                }
//            //            }

//            if (found_plus > 2)
//            {
//                DEBUG_INFO("Exit gsm transparency mode\r\n");
//                m_sys_ctx->status.gsm_transparency = 0;
//                gsm_uart_debug_enable(true);
//            }
//        }
//        if (sys_ctx()->status.ota_update_method != SYS_CTX_OTA_METHOD_INVALID)
//        {
//            vTaskDelay(1);
//        }
//        else
//        {
//            vTaskDelay(2);
//        }
//    }
//}

//void vdm_app_gsm_initialize()
//{
//    // Initialize gsm msq
//    gsm_msq_initialize(32);
//    gsm_tcp_msq_initialize(6);

//    xTaskCreate(gsm_thread, "gsm_task", 1024, NULL, 7, NULL);
//}
