#include "vdm_device_config.h"

vdm_device_config_t m_device_config;

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
