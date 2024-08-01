#include "sys_ctx.h"
//#include "FreeRTOS.h"
//#include "task.h"

static sys_ctx_t m_sys_ctx =
{
        .status.sys_err.name.gsm = 1,
        .status.sys_err.name.nv11 = 1,
};

sys_ctx_t *sys_ctx(void)
{
    return &m_sys_ctx;
}

uint32_t sys_ctx_get_call_number_buffer_size()
{
    return sizeof(m_sys_ctx.calling_status.number);
}
