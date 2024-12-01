#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

//#include "SEGGER_RTT.h"
#include "stdint.h"
#include "stdbool.h"

typedef enum
{
    SYS_CTX_BUFFER_STATE_BUSY = 0x01,       // Trang thai dang them du lieu
    SYS_CTX_BUFFER_STATE_IDLE = 0x02,       // Trang thai cho
    SYS_CTX_BUFFER_STATE_PROCESSING = 0x03 // Trang thai du lieu dang duoc xu ly
} sys_ctx_buffer_state_t;

#define SYS_CTX_OTA_METHOD_INVALID              0
#define SYS_CTX_OTA_METHOD_FTP                  1
#define SYS_CTX_OTA_METHOD_HTTP                 2

#define LARGE_BUFFER_SIZE 1024
#define SMALL_BUFFER_SIZE (512+128)

#define GSM_IMEI_MAX_LENGTH (16)
#define GSM_CSQ_INVALID (99)

#define NUM_OF_GPRS_BUFFER 3

typedef union
{
    float value;
    uint8_t bytes[4];
} Float_t;

typedef union
{
    uint32_t value;
    uint8_t bytes[4];
} Long_t;

typedef union
{
    uint16_t value;
    uint8_t bytes[2];
} Uint16_t;

typedef struct
{
    uint8_t year;       // From 2000
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} date_time_t;

typedef struct
{
    float longitude;
    float latitude;
} gps_location_t;


typedef struct
{
    uint8_t buffer[LARGE_BUFFER_SIZE];
    uint16_t buffer_idx;
    sys_ctx_buffer_state_t state;
} sys_ctx_large_buffer_t;

typedef struct
{
    uint8_t buffer[SMALL_BUFFER_SIZE];
    uint16_t buffer_idx;
    sys_ctx_buffer_state_t state;
} sys_ctx_small_buffer_t;

typedef struct _driver_rtc_t
{
    void (*init)(void);
    void (*set_date_time)(date_time_t input, int32_t gsm_adjust);
    date_time_t (*get_date_time)(void);
    uint32_t (*get_counter)(void);
    void (*tick)(uint32_t diff_sec);
    bool (*valid_date_time)(date_time_t date_time);
} const driver_rtc_t;

typedef struct _driver_gps_t
{
    gps_location_t (*get_position)(void);
    uint8_t (*get_speed)(void);
    date_time_t (*get_date_time)(void);
    float (*get_course)(void);
    float (*get_DOP)(void);
    float (*get_EPE)(void);
    uint8_t (*get_sv_count)(void);
    uint8_t (*get_jamming)(void);
    uint8_t (*is_valid)(void);
    uint8_t (*is_error)(void);
    void (*mark_invalid)(void);
} const driver_gps_t;

typedef struct
{
    uint8_t url[128];
    uint16_t port;
    uint32_t size;
    uint32_t size_downloaded;
} sys_ctx_file_transfer_t;


typedef struct
{
    char phone_number[15];
    char message[160];
    uint8_t need_to_send;
    uint8_t retry_count;
} sys_ctx_sms_struct_t;

typedef enum
{
	CALLING_STATE_NONE = 0,
	CALLING_STATE_SEND_ATC_DIAL,
	CALLING_STATE_IN_DIALING,
	CALLING_STATE_WAIT_ANSWER_BY_PRESS_KEY,		// Received RING from GSM module, wait KEY to confirm
	CALLING_STATE_WAIT_ANSWER_BY_SEND_ATC,
	CALLING_STATE_WAIT_CANCEL,
    CALLING_STATE_WAIT_CALL_INFO,
	CALLING_STATE_IN_CALLING_STATE,
	CALLING_STATE_CANCEL
} sys_ctx_calling_fsm_t;

typedef struct
{
	sys_ctx_calling_fsm_t state;
    char number[24];
} sys_ctx_calling_data_t;

typedef union
{
    struct
    {
        uint16_t battery_low : 1;
        uint16_t battery_high : 1;
        uint16_t accon_detect : 1;
        uint16_t vibrate_detect : 1;
        uint16_t always_tracking : 1;        // always send data with low duty cycle, for ex every 5s, device will update data to cloud
        uint16_t send_location_before_sleep : 1;
        uint16_t silence_detect_turn_off_only_relay_in_sms_fsm : 1;     // if device received SMS command turn off motor and sleep gsm
        uint16_t silence_detect_turn_off_relay_and_gsm_in_sms_fsm : 1; // if device received SMS command turn off motor and shutdown gsm
        uint16_t silence_detect_sleep_gsm : 1;
        uint16_t silence_detect_turn_off_gsm : 1;
        uint16_t process_new_key_plug_event : 1;
        uint16_t forgot_key : 1;
        uint16_t reserve : 4;
    } name;
    uint16_t value;
} gpio_status_t;

typedef union
{
	struct
	{
		uint16_t audio : 1;
		uint16_t refund : 1;
		uint16_t temperture : 8;
		// uint16_t drop_sensor : 1;
		uint16_t reserve	: 6;
	} name;
	uint16_t value;
} sys_ctx_vdm_feature_enable_t;

typedef union
{
	struct
	{
		uint16_t nv11 : 1;
		uint16_t temperature : 1;
		uint16_t drop_sensor : 1;
		uint16_t gsm	: 1;
		uint16_t sim	: 1;
		uint16_t internet : 1;
		uint16_t flash : 1;
		uint16_t fram0 : 1;
		uint16_t fram1 : 1;
        uint16_t motor : 1;
        uint16_t codec : 1;
		uint16_t reserve : 5;
	} name;
	uint16_t value;
} sys_ctx_err_t;

typedef struct
{
	uint8_t gsm_call_volume;
	uint8_t gsm_mic_volume;
	uint8_t media_volume;
} __attribute__((packed)) sys_ctx_audio_volume_t;

typedef struct
{
    uint8_t gsm_csq;
    sys_ctx_vdm_feature_enable_t feature_enable;
    sys_ctx_err_t sys_err;
    uint8_t money_acceptable;		// Menh gia tien co the nhan
    uint8_t motor_column;
    uint8_t motor_row;
    uint8_t machine_id[7];
    sys_ctx_audio_volume_t volume;
    uint8_t gsm_transparency;
    uint8_t ota_update_method;
    uint8_t ota_url[128];
    uint8_t ota_file_download_to_gsm_rom;
    uint32_t ota_file_download_file_size;
} sys_ctx_status_t;

typedef struct
{
    char gsm_imei[GSM_IMEI_MAX_LENGTH];
    char sim_imei[GSM_IMEI_MAX_LENGTH];
    uint8_t access_technology;
    uint32_t device_id;
    uint32_t heart_beat_at_running_s; // second
    uint32_t heart_beat_at_stoping_s;
} sys_ctx_param_t;

typedef struct
{
    sys_ctx_large_buffer_t gprs_buffer[NUM_OF_GPRS_BUFFER];
    uint8_t new_data_to_send;
    uint16_t data_length_to_send;
} gsm_tcp_client_data_t;

typedef enum
{
    GSM_CTX_SERVER_NOT_CONNECTED = 0, // Chua khoi dong xong module GSM
    GSM_CTX_SERVER_ESTABLISHING,      // Da mo ket noi PPP thanh cong toi module GSM
    GSM_CTX_SERVER_ESTABLISHED        // Da mo socket thanh cong toi module GSM
} sys_ctx_server_state_t;

typedef struct
{
    sys_ctx_status_t status;
    sys_ctx_file_transfer_t file_transfer;
    sys_ctx_param_t parameters;
//    gsm_tcp_client_data_t tcp_data;
    sys_ctx_server_state_t server_state;
    sys_ctx_calling_data_t calling_status;
//    driver_gps_t *gps;
//    driver_rtc_t *rtc;
} sys_ctx_t;

typedef enum
{
    AT_CMD_EVENT_OK = 0,
    AT_CMD_EVENT_TIMEOUT,
    AT_CMD_EVENT_ERROR,
    AT_CMD_EVENT_NOTHING        // dont care to this event
} at_cmd_response_evt_t;

/**
 * @brief Get gsm context
 * @retval Pointer to gsm context 
 */
sys_ctx_t *sys_ctx(void);

/**
 * @brief Get call buffer size to hold data
 */
uint32_t sys_ctx_get_call_number_buffer_size(void);

#endif /* SYSTEM_CONTEXT_H */
