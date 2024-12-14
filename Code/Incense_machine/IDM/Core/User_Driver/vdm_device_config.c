
/*
 * vdm_device_config.c
 *
 *  Created on: Aug 2, 2021
 *      Author: huybk
 */
#include "vdm_device_config.h"
//#include "FreeRTOS.h"
//#include "semphr.h"
#include "vdm_language.h"
#include <string.h>
#include "app_debug.h"
#include <stddef.h>
#include "utilities.h"
#include "nv11.h"
//#include "fsl_common.h"
//#include "vdm_memory.h"

vdm_device_config_t m_device_config;
//static SemaphoreHandle_t m_mutex_lock = NULL;
static vdm_device_memory_callback_t m_callback;
static bool m_store_current_data = false;



void vdm_device_config_initialize(void)
{
//    if (m_mutex_lock == NULL)
//    {
//        m_mutex_lock = xSemaphoreCreateMutex();
//        xSemaphoreGive(m_mutex_lock);
//    }

}

void vdm_device_config_load(vdm_device_memory_callback_t callback)
{
    // vdm_device_config_lock_resource(portMAX_DELAY);
    uint32_t size_of_device_config = sizeof(vdm_device_config_t);
    m_callback = callback;
    if (m_callback.read)
    {
    	m_callback.read(0, (uint8_t *)&m_device_config, size_of_device_config);
    }
    DEBUG_INFO("device_config size %u bytes\r\n", size_of_device_config);
    if (m_device_config.header_valid != VDM_DEVICE_CONFIG_VALID_FLAG)
    {
        memcpy(m_device_config.phone.phone, VDM_DEVICE_CONFIG_DEFAULT_OPERATOR_NUMBER, VDM_DEVICE_CONFIG_PHONE_SIZE);
        m_device_config.peripheral.feature_enable.name.refund_enable = 1;
        /* Dat muc tien tra lai ve 5000 */
        m_device_config.refund_note = 0x00;
        m_device_config.refund_note |= (1 << 2);
        m_device_config.accept_cash_max = 0;
        vdm_device_config_store_default_param();
    }
    // vdm_device_config_unlock_resource();
}

bool vdm_device_config_lock_resource(uint32_t timeout_ms)
{
	return 0;
 //   return xSemaphoreTake(m_mutex_lock, timeout_ms);
}

void vdm_device_config_unlock_resource(void)
{
//    xSemaphoreGive(m_mutex_lock);
}

vdm_device_config_t *vdm_get_device_config(void)
{
    return &m_device_config;
}

void vdm_device_config_sync_error_to_fram(void)
{
	if (!m_callback.write)
	{
		return;
	}
    uint32_t addr = offsetof(vdm_device_config_t, sensor_error);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.sensor_error, sizeof(m_device_config.sensor_error));
#if 0
    vdm_device_config_sensor_error_t readback;
    m_callback.read(addr,
					(uint8_t*)&readback,
					sizeof(readback));
    if (memcmp(&readback, &m_device_config.sensor_error, sizeof(readback)))
    {
    	DEBUG_ERROR("Store current device error to flash failed\r\n");
    }
#endif
}

void vdm_device_config_sync()
{
}

void vdm_device_config_sync_qrm_suplier_to_fram(void)
{
	if (!m_callback.write)
	{
		return;
	}
    uint32_t addr = offsetof(vdm_device_config_t, qrm_supplier);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.qrm_supplier, sizeof(((vdm_device_config_t *)0)->qrm_supplier));
}

void vdm_device_config_sync_uv_to_fram(void)
{
	if (!m_callback.write)
	{
		return;
	}
    uint32_t addr = offsetof(vdm_device_config_t, uv);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.uv, sizeof(vdm_device_config_peripheral_uv_t));
}

void vdm_device_config_sync_server_addr_to_fram(void)
{
	if (!m_callback.write)
	{
		return;
	}
    uint32_t addr = offsetof(vdm_device_config_t, server);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.server, sizeof(vdm_device_config_server_addr_t));
}

uint32_t vdm_device_config_get_item_price(uint32_t item_id)
{
    uint32_t retval = 0;
    vdm_device_config_lock_resource(VDM_DEVICE_CONFIG_LOCK_TIMEOUT);
    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        if (m_device_config.item[i].id == item_id)
        {
            retval = m_device_config.item[i].price;
            break;
        }
    }
    vdm_device_config_unlock_resource();
    return retval;
}

uint32_t vdm_device_config_get_item_remain(uint32_t item_id)
{
    uint32_t retval = 0;
    vdm_device_config_lock_resource(VDM_DEVICE_CONFIG_LOCK_TIMEOUT);
    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        if (m_device_config.item[i].id == item_id)
        {
            retval = m_device_config.item[i].available;
            break;
        }
    }
    vdm_device_config_unlock_resource();
    return retval;
}

void vdm_device_config_set_item_remain(uint32_t item_id, uint32_t remain)
{
    vdm_device_config_lock_resource(VDM_DEVICE_CONFIG_LOCK_TIMEOUT);
    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        if (m_device_config.item[i].id == item_id)
        {
            m_device_config.item[i].available = remain;
            break;
        }
    }
    vdm_device_config_unlock_resource();
}

/**
 * @brief  Ham ghi them so tien vua nhan vao (dua vao may)
 *
 * @param  uint32_t amount -> So tien vua dua vao
 * @retval NONE
 */
void vdm_device_config_add_holding_cash_input(uint32_t amount) // Perh_ AddNoteIn
{
    /* Lay tong so tien da dua vao truoc do luu trong ROM */
    vdm_device_config_lock_resource(portMAX_DELAY);
    m_device_config.closing_balance.hold_cash += amount;
    vdm_device_config_sync_balance_to_fram();
    vdm_device_config_unlock_resource();
}

void vdm_device_config_add_temporary_cash_out(uint32_t amount) // Perh_ AddNoteOut
{
    /* Lay tong so tien da dua vao truoc do luu trong ROM */
    vdm_device_config_lock_resource(portMAX_DELAY);
    m_device_config.closing_balance.temporary_out_cash += amount;
    vdm_device_config_sync_balance_to_fram();
    vdm_device_config_unlock_resource();
}

void vdm_device_config_save_total_credit(uint32_t credit)
{
    /* Lay tong so tien da dua vao truoc do luu trong ROM */
    vdm_device_config_lock_resource(portMAX_DELAY);
    m_device_config.closing_balance.total_credit = credit;
    vdm_device_config_sync_balance_to_fram();
    vdm_device_config_unlock_resource();
}

void vdm_device_config_save_qrm_balance(uint32_t balance)
{
    /* Lay tong so tien da dua vao truoc do luu trong ROM */
    vdm_device_config_lock_resource(portMAX_DELAY);
    m_device_config.closing_balance.qrm_balance = balance;
    vdm_device_config_sync_balance_to_fram();
    vdm_device_config_unlock_resource();
}

void vdm_device_save_new_closing_balance(uint32_t new_closing_balance) // Perh_ AddNoteIn
{
    /* Lay tong so tien da dua vao truoc do luu trong ROM */
    vdm_device_config_lock_resource(portMAX_DELAY);
    m_device_config.closing_balance.closing_balance = new_closing_balance;
    vdm_device_config_sync_balance_to_fram();
    vdm_device_config_unlock_resource();
}

void vdm_device_config_sync_items_to_fram(void)
{
    vdm_device_config_lock_resource(portMAX_DELAY);
    uint32_t addr = offsetof(vdm_device_config_t, item);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.item, VDM_DEVICE_CONFIG_MAX_ITEMS * sizeof(vdm_device_config_item_t));
    vdm_device_config_unlock_resource();
}

bool vdm_device_config_is_item_id_exist(uint32_t id)
{
    bool retval = false;
    vdm_device_config_lock_resource(portMAX_DELAY);
    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        if (m_device_config.item[i].id == id)
        {
            retval = true;
            break;
        }
    }
    vdm_device_config_unlock_resource();
    return retval;
}

void vdm_device_config_sync_glass_warmup_data_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, glass_warmup);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.glass_warmup, sizeof(vdm_device_config_peripheral_glass_warmup_t));
}

void vdm_device_config_sync_refund_cash_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, refund_note);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.refund_note, sizeof(refund_note_t));
}

void vdm_device_config_sync_machine_id_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, peripheral.machine_id);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.peripheral.machine_id, VDM_DEVICE_CONFIG_MACHINE_ID_SIZE);
}

void vdm_device_config_sync_password_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, password);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.password, sizeof(vdm_device_config_password_t));
}

void vdm_device_config_sync_phone_num_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, phone.phone);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.phone.phone[0], VDM_DEVICE_CONFIG_PHONE_SIZE);
}

void vdm_device_config_sync_temperature_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, peripheral.temperature);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.peripheral.temperature, 1);
}

void vdm_device_config_save_motor_row_and_colume(uint32_t row, uint32_t column)
{
    m_device_config.peripheral.motor_row = row;
    m_device_config.peripheral.motor_column = column;

    uint32_t addr = offsetof(vdm_device_config_t, peripheral.motor_row);
    if (m_callback.write) m_callback.write(addr,
                     (uint8_t *)&m_device_config.peripheral.motor_row,
                     sizeof(m_device_config.peripheral.motor_row));

    addr = offsetof(vdm_device_config_t, peripheral.motor_column);
    if (m_callback.write) m_callback.write(addr,
                     (uint8_t *)&m_device_config.peripheral.motor_column,
                     sizeof(m_device_config.peripheral.motor_column));
}

void vdm_device_config_sync_peripheral_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, peripheral);
    vdm_device_config_peripheral_enable_t read_back;

    if (m_callback.write) m_callback.write(addr,
                     (uint8_t *)&m_device_config.peripheral,
                     sizeof(vdm_device_config_peripheral_enable_t));
#if 0
    m_callback.read(addr,
					(uint8_t*)&read_back,
					sizeof(vdm_device_config_peripheral_enable_t));
    if (memcmp(&read_back, &m_device_config.peripheral, sizeof(vdm_device_config_peripheral_enable_t)))
    {
    	DEBUG_ERROR("Store peripheral error\r\n");
    }
#endif
}

void vdm_device_config_update_items_id(void)
{
    uint32_t i, j;
    uint32_t count = 0;
    uint8_t rows = m_device_config.peripheral.motor_row;
    uint8_t columns = m_device_config.peripheral.motor_column;

    for (i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        m_device_config.item[i].id = 0;
    }

    if (rows > 9)
        rows = 9;
    if (columns > 10)
        columns = 10;

    for (i = 0; i < rows; i++)
    {
        for (j = 1; j <= columns; j++)
        {
            m_device_config.item[count].id = count + 1;
            count++;
        }
    }
}

bool vdm_device_config_is_item_availble(uint32_t id)
{
    bool retval = false;
    vdm_device_config_lock_resource(portMAX_DELAY);
    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        if (m_device_config.item[i].id == id)
        {
            retval = (m_device_config.item[i].available ? true : false);
            break;
        }
    }
    vdm_device_config_unlock_resource();
    return retval;
}

/**
 * @brief  Ham luu menh gia tien lon nhat duoc chap nhan toi ROM
 *
 * @param  uint8_t note -> Menh gia tien ( 0 - 50.000VND, 1 - 100.000VND, 2 - 200.000VND,)
 * @retval NONE
 */
void vdm_device_config_save_acceptable_cash(vdm_device_config_accept_cash_t cash)
{
    uint32_t addr = offsetof(vdm_device_config_t, accept_cash_max);
    vdm_device_config_lock_resource(VDM_DEVICE_CONFIG_LOCK_TIMEOUT);

    m_device_config.accept_cash_max = cash;
    if (m_callback.write) m_callback.write(addr,
                     (uint8_t *)&m_device_config.accept_cash_max,
                     sizeof(vdm_device_config_accept_cash_t));

    vdm_device_config_unlock_resource();
}

void vdm_device_config_lock_item(uint32_t item_id, bool lock)
{
    vdm_device_config_lock_resource(portMAX_DELAY);
    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        if (m_device_config.item[i].id == item_id)
        {
            m_device_config.item[i].slot_error = lock ? 1 : 0;
            break;
        }
    }

    vdm_device_config_unlock_resource();
}

bool vdm_device_config_is_item_locked(uint32_t item_id)
{
    bool retval = false;
    vdm_device_config_lock_resource(portMAX_DELAY);
    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        if (m_device_config.item[i].id == item_id)
        {
            retval = m_device_config.item[i].slot_error ? true : false;
            break;
        }
    }

    vdm_device_config_unlock_resource();
    return retval;
}

void vdm_device_config_sync_balance_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, closing_balance);

    uint16_t crc = utilities_calculate_crc16((uint8_t *)&m_device_config.closing_balance,
                                             offsetof(vdm_device_config_account_balance_t, crc));
    m_device_config.closing_balance.crc = crc;
    if (m_callback.write) m_callback.write(addr,
                     (uint8_t *)&m_device_config.closing_balance,
                     sizeof(vdm_device_config_account_balance_t));
}

void vdm_device_config_sync_volume_to_fram(void)
{
    uint32_t addr = offsetof(vdm_device_config_t, peripheral.volume);
    if (m_callback.write) m_callback.write(addr,
                     (uint8_t *)&m_device_config.peripheral.volume,
                     sizeof(vdm_device_config_volume_t));
}

//hiendv
void vdm_device_config_sync_DropSensorEnable_to_fram(bool enable)
{
    uint32_t addr = offsetof(vdm_device_config_t, peripheral.feature_enable.name);
    if(enable)
    {
    	m_device_config.peripheral.feature_enable.name.drop_sensor_enable = 1;
		DEBUG_INFO("Device >> Enabled Drop sensor\r\n");
    }
    else
    {
    	m_device_config.peripheral.feature_enable.name.drop_sensor_enable = 0;
		DEBUG_INFO("Device >> Disabled Drop sensor\r\n");
    }
    if (m_callback.write) m_callback.write(addr,
                     (uint8_t *)&m_device_config.peripheral.feature_enable.name,
                     sizeof(vdm_device_config_io_enable_t));
}

//end hiendv

void vdm_device_config_store_default_param()
{
    uint32_t i = 0;
    char old_phone[VDM_DEVICE_CONFIG_PHONE_SIZE];
    vdm_device_config_lock_resource(VDM_DEVICE_CONFIG_LOCK_TIMEOUT);

    // We keep old phone number, refund state, refund value, accepte cash
    bool old_refund_enable = m_device_config.peripheral.feature_enable.name.refund_enable;
    memcpy(old_phone, m_device_config.phone.phone, VDM_DEVICE_CONFIG_PHONE_SIZE);
    refund_note_t old_refund_value = m_device_config.refund_note;
    vdm_device_config_accept_cash_t old_accept_cash = m_device_config.accept_cash_max;

    memset(&m_device_config, 0, sizeof(m_device_config));

    m_device_config.header_valid = VDM_DEVICE_CONFIG_VALID_FLAG;
    if (m_callback.write) m_callback.write(offsetof(vdm_device_config_t, header_valid),
                     (uint8_t *)&m_device_config.header_valid,
                     sizeof(m_device_config.header_valid));

    // Set default temperature
    m_device_config.peripheral.temperature = VDM_DEVICE_CONFIG_DEFAULT_TEMPERATURE;

    // Enable Drop sensor and Led
    m_device_config.peripheral.feature_enable.name.led_enable = 1;
    m_device_config.peripheral.feature_enable.name.drop_sensor_enable = 1;
    m_device_config.peripheral.feature_enable.name.rgb_enable = 1;
    m_device_config.peripheral.feature_enable.name.refund_enable = old_refund_enable ? 1 : 0;

    // Default motor 6 row and 10 colume
    m_device_config.peripheral.motor_row = 6;
    m_device_config.peripheral.motor_column = 10;

    // Reset audio volume
    m_device_config.peripheral.feature_enable.name.audio_enable = 1;
    m_device_config.peripheral.volume.call_volume = 100;
    m_device_config.peripheral.volume.media_volume = 100;
    m_device_config.peripheral.volume.mic_volume = 100;

    // Set default language
    m_device_config.peripheral.language = VDM_LANG_VNI;

    // Commit changed to FRAM
    vdm_device_config_sync_peripheral_to_fram();

    /* Delete sensor error code */
    m_device_config.sensor_error.error = 0;
    vdm_device_config_sync_error_to_fram();

    /* Delete glass warmup */
    memset(&m_device_config.glass_warmup, 0, sizeof(m_device_config.glass_warmup));
    vdm_device_config_sync_glass_warmup_data_to_fram();

    /* Delete UV */
    memset(&m_device_config.uv, 0, sizeof(m_device_config.uv));
    vdm_device_config_sync_uv_to_fram();

    for (uint32_t i = 0; i < VDM_DEVICE_CONFIG_MAX_ITEMS; i++)
    {
        memset(&m_device_config.item[i], 0, sizeof(vdm_device_config_item_t));
    }
    vdm_device_config_update_items_id();

    uint32_t addr = offsetof(vdm_device_config_t, item);
    if (m_callback.write) m_callback.write(addr, (uint8_t *)&m_device_config.item[0], VDM_DEVICE_CONFIG_MAX_ITEMS * sizeof(vdm_device_config_item_t));

    /* Reset total cash in, cash out, closing_balance */
    m_device_config.closing_balance.total_credit = 0;
    m_device_config.closing_balance.total_debit = 0;
    m_device_config.closing_balance.closing_balance = 0;
    m_device_config.closing_balance.crc = 0;
    vdm_device_config_sync_balance_to_fram();

    // Keep old acceptable cash
    m_device_config.accept_cash_max = old_accept_cash;
    if (m_callback.write) m_callback.write(offsetof(vdm_device_config_t, accept_cash_max),
                     (uint8_t *)&m_device_config.accept_cash_max,
                     sizeof(vdm_device_config_accept_cash_t));

    /* Reset server addr to default */
    m_device_config.server.addr = VDM_DEVICE_CONFIG_DEFAULT_SERVER_ADDR;
    m_device_config.server.port = VDM_DEVICE_CONFIG_DEFAULT_SERVER_PORT;
    vdm_device_config_sync_server_addr_to_fram();

    // Phone
    memcpy(m_device_config.phone.phone, old_phone, strlen(old_phone));
    vdm_device_config_sync_phone_num_to_fram();

    /* Reset shop password */
    memcpy(m_device_config.password.store_owner, VDM_DEVICE_CONFIG_DEFAULT_SHOP_PASSWORD, 6);
    memcpy(m_device_config.password.technical, VDM_DEVICE_CONFIG_DEFAULT_TECH_PASSWORD, 6);

    vdm_device_config_sync_password_to_fram();

    // Erase all gprs log in flash
//hiendv    vdm_flash_log_erase();	

    // Keep old refund value
    m_device_config.refund_note = old_refund_value;
    vdm_device_config_sync_refund_cash_to_fram();

    vdm_device_config_unlock_resource();

    /* luu toan bo thong tin hang hoa */
    // vdm_device_config_sync_items_to_fram();

    DEBUG_INFO("Factory reset done\r\n");
}

uint32_t vdm_device_config_get_flash_memory_size(void)
{
	if (!m_callback.size)
	{
		return 0;
	}
    vdm_device_config_lock_resource(portMAX_DELAY);
    uint32_t size = m_callback.size();
    vdm_device_config_unlock_resource();
    return size;
}

bool vdm_device_config_read_memory(uint32_t start_addr, uint8_t *data, uint32_t length)
{

    bool retval = false;
	if (m_callback.read)
	{
	    vdm_device_config_lock_resource(portMAX_DELAY);

	    retval = m_callback.read(start_addr, data, length);

	    vdm_device_config_unlock_resource();
	}
    return retval;
}

bool vdm_device_config_write_memory(uint32_t start_addr, uint8_t *data, uint32_t length)
{
    bool retval = false;
    if (m_callback.write)
    {
		vdm_device_config_lock_resource(portMAX_DELAY);

		retval = m_callback.write(start_addr, data, length);

		vdm_device_config_unlock_resource();
    }
    return retval;
}

bool vdm_device_config_erase_memory(uint32_t start_addr, uint32_t length)
{
    bool retval = false;
    vdm_device_config_lock_resource(portMAX_DELAY);

    retval = m_callback.erase(start_addr, length);

    vdm_device_config_unlock_resource();
    return retval;
}

