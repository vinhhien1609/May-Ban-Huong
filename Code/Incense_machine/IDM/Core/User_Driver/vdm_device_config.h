/*
 * vdm_device_config.h
 *
 *  Created on: Aug 2, 2021
 *      Author: huybk
 */

#ifndef VDM_DEVICE_CONFIG_H_
#define VDM_DEVICE_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

#define VDM_DEVICE_CONFIG_LOCK_TIMEOUT 0xFFFFFFFF

#define VDM_DEVICE_CONFIG_FRAM_ADDR (0)
#define VDM_DEVICE_CONFIG_MAX_ITEMS 100
#define VDM_DEVICE_CONFIG_MACHINE_ID_SIZE 7
#define VDM_DEVICE_CONFIG_PHONE_SIZE 12

#define VDM_DEVICE_CONFIG_MOTOR_MAX_COLUMN 10
#define VDM_DEVICE_CONFIG_MOTOR_MAX_ROW 6
#define VDM_DEVICE_CONFIG_REFUND_CHANNEL_ENABLE(src, channel) (src->refund_note & (1 << channel))

#define VDM_DEVICE_CONFIG_DEFAULT_SERVER_ADDR 0xCBAB143E // 203.171.20.62
#define VDM_DEVICE_CONFIG_DEFAULT_SERVER_PORT 9201

#define VDM_DEVICE_CONFIG_DEFAULT_TEMPERATURE 10

#define VDM_DEVICE_CONFIG_VALID_FLAG 0xCBAB143E
#define VDM_DEVICE_CONFIG_DEFAULT_TECH_PASSWORD "125890"
#define VDM_DEVICE_CONFIG_DEFAULT_SHOP_PASSWORD "000000"
#define VDM_DEVICE_CONFIG_DEFAULT_OPERATOR_NUMBER "0944355706"
#define VDM_DEVICE_CONFIG_DEFAULT_FTP_SERVER "203.171.20.62"//"203.171.20.53"
#define VDM_DEVICE_CONFIG_DEFAULT_FTP_PORT 21
#define VDM_DEVICE_CONFIG_DEFAULT_FTP_USERNAME "vms"
#define VDM_DEVICE_CONFIG_DEFAULT_FTP_PASSWORD "Vending@123!"

#define VDM_DEVICE_CONFIG_SALE_START_ADDR(flash_end, size_of_sale) (flash_end - size_of_sale)

/* Memory organization of FRAM

    -------------------------------------
    Config
    -------------------------------------
    Reverse 256 byte for configuration
    -------------------------------------
    -------------------------------------

    -------------------------------------
    Sale data : 4KB
    ------------End of FRAM -------------

*/
///* The supported language */
//typedef enum
//{
//    VDM_LANG_VNI = 0x00,   /* Vietnamese */
//    VDM_LANG_ENGLISH = 0x01,   /* English */
//    VDM_LANG_MAX    /* The end of languages */
//} vdm_language_t;

typedef enum
{
		FIX_MODE = 0x00,						/* cap phat co dinh so huong */	
    MERIT_MODE,				   /* cong duc */
    SALES_MODE,   /* ban hang*/	
    VDM_MODE_MAX    /* The end of languages */
} vdm_run_mode_t;

typedef union
{
    struct
    {
        uint8_t channel0 : 1;
        uint8_t channel1 : 1;
        uint8_t channel2 : 1;
        uint8_t channel3 : 1;
        uint8_t channel4 : 1;
        uint8_t channel5 : 1;
        uint8_t channel6 : 1;
        uint8_t channel7 : 1;
    }__attribute__((packed)) name;
    uint8_t data;
} __attribute__((packed)) vdm_config_refund_channel_t;

typedef struct
{
    /* Volume from 0 to 100 */
    uint8_t media_volume;
    uint8_t call_volume;
    uint8_t mic_volume;
} __attribute__((packed)) vdm_device_config_volume_t;

typedef union __attribute__((packed))
{
    struct __attribute__((packed))
    {
        uint8_t led_enable : 1;
        uint8_t rgb_enable : 1;
        uint8_t audio_enable : 1;
        uint8_t refund_enable : 1;
        uint8_t drop_sensor_enable : 1;
        uint8_t warning_enable : 1;
        uint8_t reserve : 2;
    }__attribute__((packed)) name;
    uint8_t value;
} __attribute__((packed)) vdm_device_config_io_enable_t;

typedef struct
{
    vdm_device_config_io_enable_t feature_enable;
    uint8_t temperature;
    uint8_t money_acceptable;
    uint8_t motor_column;
    uint8_t motor_row;
    uint8_t machine_id[7];
    vdm_device_config_volume_t volume;
    uint8_t language;
} __attribute__((packed)) vdm_device_config_peripheral_enable_t;

typedef struct
{
    uint8_t store_owner[6];
    uint8_t technical[6];
    uint8_t factory[6];
} __attribute__((packed)) vdm_device_config_password_t;

typedef struct
{
    uint8_t phone[VDM_DEVICE_CONFIG_PHONE_SIZE];
} __attribute__((packed)) vdm_device_cfg_mobile_num_t;

typedef struct
{
    int32_t closing_balance;    // So du tai khoan
    uint32_t total_credit;      // Tong so tien nhan vao dau doc tien
    uint32_t total_debit;       // Tong so tien tra lai
    uint32_t temporary_in_cash; // Tong so tien nhan vao cua thiet bi, khong bi xoa
    int32_t temporary_out_cash; // Tong so tra ra vao cua thiet bi, khong bi xoa
    int32_t hold_cash;
    uint32_t qrm_balance;
    uint16_t crc;
} __attribute__((packed)) vdm_device_config_account_balance_t;

typedef struct
{
    uint32_t addr;
    uint16_t port;
} __attribute__((packed)) vdm_device_config_server_addr_t;

/* bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0 */
/* 200k  |  100k |   50k |  20k  |   10k |   5k  |   2k  |   1k  */
typedef uint16_t vdm_device_config_accept_cash_t;

typedef union
{
    struct
    {
        uint8_t drop : 1;
        uint8_t payout : 1;
        uint8_t motor : 1;
        uint8_t reserve : 5;
    }__attribute__((packed)) name;
    uint8_t error;
} __attribute__((packed)) vdm_device_config_sensor_error_t;

typedef uint16_t refund_note_t;

typedef struct
{
    uint8_t id;
    uint32_t price;
    uint8_t available; // Number of item remain in rack
    uint8_t slot_error;
} __attribute__((packed)) vdm_device_config_item_t;

typedef union
{
    struct
    {
        uint8_t mon : 1;
        uint8_t tue : 1;
        uint8_t wed : 1;
        uint8_t thu : 1;
        uint8_t fri : 1;
        uint8_t sat : 1;
        uint8_t sun : 1;
        uint8_t forever : 1;
    }__attribute__((packed)) name;
    uint8_t value;
} __attribute__((packed)) vdm_device_config_peripheral_glass_repeat_by_date_t;

typedef struct __attribute__((packed))
{
    uint8_t begin;
    uint8_t end;
    uint8_t enable;
} vdm_device_config_peripheral_glass_repeat_by_hour_t;

typedef struct
{
    uint32_t flag;
    uint32_t on_time_min;  // minutes
    uint32_t off_time_min; // minutes
    vdm_device_config_peripheral_glass_repeat_by_hour_t repeat_by_hour[3];
    vdm_device_config_peripheral_glass_repeat_by_date_t repeat_by_date;
} __attribute__((packed)) vdm_device_config_peripheral_glass_warmup_t;

typedef struct
{
    uint32_t timeout_s;
    uint32_t valid_flag;
    uint32_t enable;
} __attribute__((packed)) vdm_device_config_peripheral_uv_t;

typedef struct
{
    uint16_t header;
    uint8_t duty_max;
    uint8_t duty_min;
    uint32_t cycle;
} __attribute__((packed)) vdm_device_config_motor_calibrate_t;

typedef struct
{
    uint32_t header_valid;
    vdm_device_config_peripheral_enable_t peripheral;
    vdm_device_config_password_t password;
    vdm_device_cfg_mobile_num_t phone;
    vdm_device_config_account_balance_t closing_balance;
    vdm_device_config_server_addr_t server;
    vdm_device_config_accept_cash_t accept_cash_max;
    vdm_device_config_sensor_error_t sensor_error;
    refund_note_t refund_note;
    vdm_device_config_item_t item[VDM_DEVICE_CONFIG_MAX_ITEMS];
    vdm_device_config_peripheral_glass_warmup_t glass_warmup;
    vdm_device_config_peripheral_uv_t uv;
    uint8_t qrm_supplier;
    vdm_device_config_motor_calibrate_t motor_calib;
		vdm_run_mode_t run_mode;
    uint8_t reserve[10]; // Reverse for feature use
} __attribute__((packed)) vdm_device_config_t;

typedef struct
{
    bool (*write)(uint32_t addr, uint8_t *data, uint32_t length);
    bool (*read)(uint32_t addr, uint8_t *data, uint32_t length);
    bool (*erase)(uint32_t addr, uint32_t length);
    uint32_t (*size)(void); // Get memory size of flash driver
} vdm_device_memory_callback_t;

/**
 * @brief			Init vdm device config
 */
void vdm_device_config_initialize(void);

/**
 * @brief			Load config in flash
 */
void vdm_device_config_load(vdm_device_memory_callback_t callback);

/**
 * @brief			Lock device config resource when used multi tasks
 * @param[in]		timeout_ms	Max timeout in ms
 * @retval			TRUE Lock resource success
 * 					FALSE Lock resource failed
 */
bool vdm_device_config_lock_resource(uint32_t timeout_ms);

/**
 * @brief			Unlock device config resource when used multi tasks
 */
void vdm_device_config_unlock_resource(void);

/**
 * @brief			Get current device configuration
 * @retval			Pointer to device config
 */
vdm_device_config_t *vdm_get_device_config(void);

/**
 * @brief			Store error code to FRAM
 */
 
vdm_run_mode_t vdm_device_get_mode(void);
	 
void vdm_device_config_sync_error_to_fram(void);

/**
 * @brief			Write all pending data to FRAM
 */
void vdm_device_config_sync(void);

/**
 * @brief			Get item price by id
 * @param[in]		item_id Item id
 * @retval			Item price
 */
uint32_t vdm_device_config_get_item_price(uint32_t item_id);

/**
 * @brief			Get number of item remain in rack
 * @param[in]		item_id Item id
 * @retval			Item remain
 * @note            This function is threadsafe
 */
uint32_t vdm_device_config_get_item_remain(uint32_t item_id);

/**
 * @brief			Set number of item remain in rack
 * @param[in]		item_id Item id
 * @param[in]		Item remain
 * @note            This function is threadsafe
 */
void vdm_device_config_set_item_remain(uint32_t item_id, uint32_t remain);

/**
 * @brief			Lock items
 * @param[in]		item_id Item id
 * @param[in]		lock TRUE - Lock resources, FALSE - unlock resources
 * @retval			Item remain
 * @note            This function is threadsafe
 */
void vdm_device_config_lock_item(uint32_t item_id, bool lock);

/**
 * @brief           Check item is locked
 * @retval          TRUE Item is locked
 *                  FALSE Item is not locked
 */
bool vdm_device_config_is_item_locked(uint32_t item_id);

/**
 * @brief           Store glass warmup configuration information to FRAM
 */
void vdm_device_config_sync_glass_warmup_data_to_fram(void);

/**
 * @brief           Sync refund cash config to FRAM
 */
void vdm_device_config_sync_refund_cash_to_fram(void);

/**
 * @brief           Sync machine configuration information to FRAM
 */
void vdm_device_config_sync_machine_id_to_fram(void);

/**
 * @brief           Sync password configuration information to FRAM
 */
void vdm_device_config_sync_password_to_fram(void);

/**
 * @brief           Sync phone number into FRAM
 */
void vdm_device_config_sync_phone_num_to_fram(void);

/**
 * @brief           Sync temperature number into FRAM
 */
void vdm_device_config_sync_temperature_to_fram(void);

/**
 * @brief           Sync items number into FRAM
 */
void vdm_device_config_sync_items_to_fram(void);

/**
 * @brief           Check item exits
 * @retval          true Item existed
 *                  false Item not exited
 * @note            This function is threadsafe
 */
bool vdm_device_config_is_item_id_exist(uint32_t id);

/**
 * @brief           Save motor row and colume to FRAM
 * @param[in]       row Motor row
 * @param[in]       colume Motor colume
 */
void vdm_device_config_save_motor_row_and_colume(uint32_t row, uint32_t column);

/**
 * @brief           Update all item id
 */
void vdm_device_config_update_items_id(void);

/**
 * @brief           Check item availble
 * @retval          TRUE Item availble
 *                  FALSE Item note availble
 */
bool vdm_device_config_is_item_availble(uint32_t id);

/**
 * @brief           USync supplier to FRAM
 */
void vdm_device_config_sync_qrm_suplier_to_fram(void);

/**
 * @brief           Update all uv config to FRAM
 */
void vdm_device_config_sync_uv_to_fram(void);

/**
 * @brief           Update server addr to fram
 */
void vdm_device_config_sync_server_addr_to_fram(void);

/**
 * @brief           Store peripheral config to FRAM
 */
void vdm_device_config_sync_peripheral_to_fram(void);

/**
 * @brief           Store account balance to FRAM
 */
void vdm_device_config_sync_balance_to_fram(void);

/**
 * @brief           Store holding cash in to FRAM
 * @param[in]       amount Input holding, new holding = old value + amount
 */
void vdm_device_config_add_holding_cash_input(uint32_t amount);

/**
 * @brief           Store temporary cash out to FRAM
 * @param[in]       amount Cash output, new value = old value + amount
 */
void vdm_device_config_add_temporary_cash_out(uint32_t amount);

/**
 * @brief           Store current credit
 * @param[in]       credit New credit value
 */
void vdm_device_config_save_total_credit(uint32_t credit);

/**
 * @brief           Store new QRM balance value
 * @param[in]       balance New balance value
 */
void vdm_device_config_save_qrm_balance(uint32_t balance);

/**
 * @brief           Store current closing balance to FRAM
 * @param[in]       new_closing_balance New closing balance
 */
void vdm_device_save_new_closing_balance(uint32_t new_closing_balance);

/**
 * @brief           Reset to default value
 */
void vdm_device_config_store_default_param(void);

/**
 * @brief           Sync volume to FRAM
 */
void vdm_device_config_sync_volume_to_fram(void);

/**
 * @brief           Get memory size of flash
 */
uint32_t vdm_device_config_get_flash_memory_size(void);

/**
 * @brief           Read flash memory
 * @param[in]       start_addr Start address of data
 * @param[in]       data Buffer hold data
 * @param[in]	    length Data length in bytes
 * @retval          TRUE Operation succeeded
 *                  FALSE Operation failed
 */
bool vdm_device_config_read_memory(uint32_t start_addr, uint8_t *data, uint32_t length);

/**
 * @brief           Write flash memory
 * @param[in]       start_addr Start address of data
 * @param[in]       data Buffer hold data
 * @param[in]	    length Data length in bytes
 * @retval          TRUE Operation succeeded
 *                  FALSE Operation failed
 */
bool vdm_device_config_write_memory(uint32_t start_addr, uint8_t *data, uint32_t length);

/**
 * @brief           Erase flash memory
 * @param[in]       start_addr Start address to erase
 * @param[in]	    length Erase flash size
 * @retval          TRUE Operation succeeded
 *                  FALSE Operation failed
 */
bool vdm_device_config_erase_memory(uint32_t start_addr, uint32_t length);

/**
 * @brief           Save acceptable cash into memory
 * @param[in]       cash Acceptable value
 */

void vdm_device_config_sync_DropSensorEnable_to_fram(bool enable);


void vdm_device_config_save_acceptable_cash(vdm_device_config_accept_cash_t cash);

#endif /* VDM_DEVICE_CONFIG_H_ */
