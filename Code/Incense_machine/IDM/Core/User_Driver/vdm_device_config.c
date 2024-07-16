#include "vdm_device_config.h"
#include <stddef.h>
#include <string.h>

#define AT_NONCACHEABLE_SECTION_INIT(var) __attribute__((section("NonCacheable.init"))) var

AT_NONCACHEABLE_SECTION_INIT(vdm_device_config_t m_device_config);
static vdm_device_memory_callback_t m_callback;

vdm_device_config_t m_device_config;


void vdm_device_config_load(vdm_device_memory_callback_t callback)
{
    // vdm_device_config_lock_resource(portMAX_DELAY);
    uint32_t size_of_device_config = sizeof(vdm_device_config_t);
    m_callback = callback;
    if (m_callback.read)
    {
    	m_callback.read(0, (uint8_t *)&m_device_config, size_of_device_config);
    }
//    printf("device_config size %u bytes\r\n", size_of_device_config);
    if (m_device_config.header_valid != VDM_DEVICE_CONFIG_VALID_FLAG)
    {
        memcpy(m_device_config.phone.phone, VDM_DEVICE_CONFIG_DEFAULT_OPERATOR_NUMBER, VDM_DEVICE_CONFIG_PHONE_SIZE);
        m_device_config.peripheral.feature_enable.name.refund_enable = 1;
        /* Dat muc tien tra lai ve 5000 */
        m_device_config.refund_note = 0x00;
        m_device_config.refund_note |= (1 << 2);
        m_device_config.accept_cash_max = 0;
//        vdm_device_config_store_default_param();
    }
    // vdm_device_config_unlock_resource();
}

vdm_device_config_t *vdm_get_device_config(void)
{
    return &m_device_config;
}
bool vdm_device_config_lock_resource(uint32_t timeout_ms)
{
//    return xSemaphoreTake(m_mutex_lock, timeout_ms);
	return false;//add_by_duclq
}

void vdm_device_config_unlock_resource(void)
{
//    xSemaphoreGive(m_mutex_lock);//cmt_by_duclq
}
