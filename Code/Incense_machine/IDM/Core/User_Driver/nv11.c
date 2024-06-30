
#include "nv11.h"
#include "ITLSSPProc.h"
#include "SSPComs.h"
#include "ssp_helpers.h"
//#include "fsl_debug_console.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h"
//#include "queue.h"
#include "vdm_device_config.h"
#include "app_debug.h"
//#include "vdm_sdcard.h"
#include "serialfunc.h"
//#include "vdm_selling_task.h"

#define NV11_AUDIO_ENABLE 0

#if NV11_AUDIO_ENABLE
#include "wav_player.h"
#include "peripheral.h"
#endif /* NV11_AUDIO_ENABLE */

#define portMAX_DELAY   ( uint32_t ) 0xffffffffUL
//#define NV11_LOCK(timeout) (xSemaphoreTake(m_lock_nv11, timeout))//author:cmt_by_duclq
//#define NV11_UNLOCK() xSemaphoreGive(m_lock_nv11)//author:cmt_by_duclq

static SSP_COMMAND m_ssp_cmd;
static bool m_nv11_initialized = false; // Trang thai hoan thanh viec cai dat
static bool m_is_holding = false; // Trang thai giu (ngam) tien duoc xac lap hay khong
static nv11_event_t m_last_n11_event = NV11_EVENT_NONE;
static uint32_t m_lastest_note = 0; // Menh gia to tien gan nhat
static uint8_t m_unit_type = 0;    // Loai dau doc tien

static SSP6_SETUP_REQUEST_DATA m_setup_req;

static int32_t m_denominations[9] = {1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000};

//static void parse_poll(SSP_COMMAND *sspC, SSP_POLL_DATA6 * poll, bool lock);

static uint32_t m_channel[10]; // Menh gia tien trong tu kenh tien te
static uint8_t m_channel_num = 0;
static uint32_t m_stored_notes[10];  // Tong so tien cua tung menh gia trong FloatNote -> NV200
static bool m_refund_channels[10];   // Kenh tien te cho phep tra lai -> voi NV200
static uint32_t m_refund_note = 0;   // Kenh tien te cho phep tra lai -> voi NV11
static uint16_t m_count_init_payout_fail = 0; // Dem so lan init dau doc tien bi fail (init khi khoi dong may)
static bool m_init_payout_fault = false;
static bool m_action_payout_fault = false;
static uint32_t m_num_note_payout;
static uint32_t m_last_num_note_payout;
static uint32_t m_error_payout;

__WEAK void display_payout_countdown(uint8_t value)
{

}

//static SemaphoreHandle_t m_lock_nv11;//author:cmt_by_duclq

/**
 * @brief  Ham phan tich cac su kien trong dau doc tien
 *
 * @param  NONE
 * @retval
 */
static void parse_poll(SSP_COMMAND *sspC, SSP_POLL_DATA6 *poll, bool lock)
{
    vdm_device_config_t *device_config = vdm_get_device_config();
    int32_t i;
    SSP_RESPONSE_ENUM error_code;
    for (i = 0; i < poll->event_count; ++i) /* Vong quet toan bo cac su kien tra ve co trong poll */
    {
        switch (poll->events[i].event) /* Kiem tra su kien thu i*/
        {
			/* Su kien Reset -> Xay ra khi thiet bi duoc cap nguon và da tra qua qua trinh reset */
			case SSP_POLL_RESET:
				DEBUG_INFO("NV11 >> Unit Reset");
				/* Neu su kien nay xay ra, phai chac chan Vesion SSP dang su dung la V6 */
                error_code = ssp6_host_protocol(sspC, 0x06);
				if (error_code != SSP_RESPONSE_OK)
				{
					DEBUG_ERROR("NV11 >> Host Protocol Failed code %u\r\n", error_code); /* Neu ko phai V6 -> Bao loi len man hinh Debug */
					return;
				}
				break;

			/* Su kien Read -> Xay ra khi dau doc dang doc tien giay */
			case SSP_POLL_READ:
				/* Neu xu kien doc co gia tri > 0 nghia la 1 to tien dang duoc doc va xac thuc */
				if (poll->events[i].data1)
				{
					/* In len man hinh kenh tien vua doc duoc (1 -> 6)*/
					DEBUG_INFO("NV11 >> Note Read %d %s\r\n", poll->events[i].data1, poll->events[i].cc);

					if (poll->events[i].data1 > 0 && poll->events[i].data1 < 10)
					{
						/* Bao su kien, trang thai may vua nhan tien -> NV11_NOTE_READ;*/
						m_last_n11_event = NV11_NOTE_READ;
						/* Tra ve gia tri tien vua doc duoc */
						m_lastest_note = m_denominations[poll->events[i].data1 - 1];
						DEBUG_WARN("Lastest value %u\r\n", m_lastest_note);
					}
				}
				break;

			/* Su kien nay bao tien da duoc chuyen tu vi tri nhan tien sang vi tri ngan luu tru */
			case SSP_POLL_CREDIT:
				/* Dung dau doc tien */
				NV11_Disable(false);
				/* Tien da duoc chap nhan va chuyen vao ngan luu tru */
				DEBUG_INFO("NV11 >> Credit %d %s\r\n", poll->events[i].data1, poll->events[i].cc);
				break;

			/* Su kien nay say ra khi thiet bi mat dien luc dang trong qua trinh tra lai tien,
			   gia tri tra lai -> so tien can phai thanh toan not */
			case SSP_POLL_INCOMPLETE_PAYOUT:
				/* In ra man hinh debug so tien con phai tra lai */
				DEBUG_INFO("NV11 >> Incomplete payout %d of %d %s\r\n", poll->events[i].data1, poll->events[i].data2, poll->events[i].cc);
				break;

			/* Su kien nay say ra khi thiet bi mat dien luc chua thuc hien viec tra lai
			   gia tri tra lai -> So tien va su kien yeu cau*/
			case SSP_POLL_INCOMPLETE_FLOAT:
				/* In ra man hinh debug so tien va su kien yeu cau */
				DEBUG_WARN("NV11 >> Incomplete float %d of %d %s\r\n", poll->events[i].data1, poll->events[i].data2, poll->events[i].cc);
				break;

			/* Su kien dang trong qua trinh tra lai cho nguoi dung (vi tien bi tu choi) */
			case SSP_POLL_REJECTING:
				DEBUG_INFO("NV11 >> SSP_POLL_REJECTING\r\n");
				break;

			/* Su kien da hoan thanh viec tra lai tien cho nguoi tieu dung (vi tien bi tu choi) */
			case SSP_POLL_REJECTED:
				DEBUG_WARN("NV11 >> Note Rejected\r\n");
				m_last_n11_event = NV11_NOTE_REJECTED; /* Nhan su kien tien bi tra lai */
				//*vdm_selling_task_get_hold_balance() = 0;//cmt_by_duclq
				m_is_holding = false; /* Xoa trang thai giu tien */
				break;

			/* Su kien thong bao tien dang duoc van chuyen den ngan xep */
			case SSP_POLL_STACKING:
				DEBUG_VERBOSE("NV11 >> Note SSP_POLL_STACKING\r\n");
				break;

			/* Su kien bao tien da duoc chuyen vao kho luu tru tra lai (NoteFloat) */
			case SSP_POLL_STORED:
				// The note has been stored in the payout unit
				DEBUG_INFO("NV11 >> Stored\r\n");
				/* Nhan su kien tien chuyen vao kho tra lai */
				m_last_n11_event = NV11_NOTE_STORED;
				/* Neu la dau doc tien NV200 */
				if (m_unit_type == NV_TYPE_NV200)
				{
					for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
					{
						m_stored_notes[channel_index] = ssp6_check_note_level(&m_ssp_cmd, m_channel[channel_index]);
					}
				}
				/* Hien thi len man hinh debug tong so tien trong kho tra lai */
				m_num_note_payout = NV11_GetTotalCashInNoteFloat(false) / NV11_GetDenominationForChange(false);
				m_last_num_note_payout = m_num_note_payout;
				DEBUG_INFO("NV11 >> Stored Notes: %d\r\n", m_num_note_payout * NV11_GetDenominationForChange(false));
				break;

			/* Su kien co mot to tien di tu tren float note xuong stack note */
			case SSP_POLL_NOTE_TRANSFERRED_TO_STACK:
				DEBUG_INFO("NV11 >> Note Transferred to Stacker\r\n");
				/* Thuc hien tra lai them 1 to tien */
				NV11_PayoutNote(false);
				break;

			/* Su kien bao tien da duoc chuyen vao ngan xep chong (kho khong tra lai) */
			case SSP_POLL_STACKED:
				/* Hien thi tien dang trong ngan xep */
				DEBUG_INFO("NV11 >> Stacked\r\n");
				/* Nhan su kien tien chuyen vao ngan xep */
				m_last_n11_event = NV11_NOTE_STACKED;
				break;

			/* Khong co su kien nay */
			case SSP_POLL_SAFE_JAM:
				DEBUG_INFO("NV11 >> Safe Jam\r\n");
				break;

			/* Su kien to tien bi ket trong qua trinh van chuyen, gia tri tra ve vi tri bi ket */
			case SSP_POLL_UNSAFE_JAM:
				DEBUG_INFO("NV11 >> Unsafe Jam\r\n");
				break;

			/* Su kien tien bi ket */
			case SSP_POLL_JAMMED:
				DEBUG_INFO("NV11 >> Jammed %d\r\n", poll->events[i].data1);
				break;

			/* Su kien Poll da bi vo hieu hoa */
			case SSP_POLL_DISABLED:
				DEBUG_VERBOSE("NV11 has been DISABLED\r\n");
				//nv11_enable();
				break;
			/* Su kien phat hien mot no luc gian lan */
			case SSP_POLL_FRAUD_ATTEMPT:
				DEBUG_INFO("NV11 >> Fraud Attempt %d %s\r\n", poll->events[i].data1, poll->events[i].cc);
				m_last_n11_event = NV11_NOTE_FRAUD_ATTEMPT;
				break;

			/* Su kien bao ngan xep day */
			case SSP_POLL_STACKER_FULL:
				DEBUG_INFO("NV11 >> Stacker Full\r\n");
				break;

			/* Su kien bao hop tien bi thao */
			case SSP_POLL_CASH_BOX_REMOVED:
				DEBUG_INFO("NV11 >> Cashbox Removed\r\n");
				break;

				/* Su kien bao hop tien duoc thay the */
			case SSP_POLL_CASH_BOX_REPLACED:
				DEBUG_INFO("NV11 >> Cashbox Replaced\r\n");
				break;

				/* Su kien bao mot to tien da duoc dua vao luc thiet bi luc khoi dong, va da duoc dua ra khoi mat truoc cua may */
			case SSP_POLL_CLEARED_FROM_FRONT:
				DEBUG_INFO("NV11 >> Cleared from front %d\r\n", m_denominations[poll->events[i].data1 - 1]);
				/*if (*vdm_selling_task_get_hold_balance() != 0)
					*vdm_selling_task_get_hold_balance() -= m_denominations[poll->events[i].data1 - 1];*/ //cmt_by_duclq
				break;

			/* Su kien bao mot to tien da duoc dua vao luc thiet bi luc khoi dong, va da duoc dua ra khoi hop dung tien */
			case SSP_POLL_CLEARED_INTO_CASHBOX:
				DEBUG_INFO("NV11 >> Cleared Into Cashbox %d\r\n", m_denominations[poll->events[i].data1 - 1]);
				/*if (*vdm_selling_task_get_hold_balance() != 0)
					*vdm_selling_task_get_hold_balance() -= m_denominations[poll->events[i].data1 - 1];
				*vdm_selling_task_get_closing_balance() += m_denominations[poll->events[i].data1 - 1];*/ //cmt_by_duclq
				break;

			/* Su kien tra ve tong gia tri ma thiet bi da phan chia de phan hoi lai lenh phan chia (Dispended command) */
			case SSP_POLL_DISPENSED:
				if (m_unit_type == NV_TYPE_NV200)
				{
					for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
					{
						m_stored_notes[channel_index] = ssp6_check_note_level(&m_ssp_cmd, m_channel[channel_index]);
					}
				}
				/* tinh toan so tien con lai co trong dau tra lai */
				DEBUG_INFO("NV11 >> Note dispensed\r\n");
				nv11_enable(false);
				m_num_note_payout = NV11_GetTotalCashInNoteFloat(false) / NV11_GetDenominationForChange(false);
				DEBUG_INFO("NV11 >> Stored Notes: %d", m_num_note_payout * NV11_GetDenominationForChange(false));

				if (m_num_note_payout == 0) /* neu khong co to tien nao trong dau tra lai thi kiem tra */
				{
					if (m_last_num_note_payout > 1) /* neu dot ngot tien tra lai ve khong */
					{
						DEBUG_INFO("NV11 >> Error money payback to 0\r\n");
						m_error_payout = 1;
						vdm_device_config_lock_resource(portMAX_DELAY);
						//vdm_device_config_sync_error_to_fram();//cmt_by_duclq
						device_config->sensor_error.name.payout = m_error_payout;
						vdm_device_config_unlock_resource();
					}
					else if (m_last_num_note_payout == 1) /* neu truoc do chi co 1 to tien thi cap nhat luong tien truoc do */
					{
						m_last_num_note_payout = m_num_note_payout;
					}
				}
				else if (m_num_note_payout != 0) /* neu con tien trong dau tra lai thi cap nhat tien tra lai de so sanh ve sau */
				{
					m_last_num_note_payout = m_num_note_payout;
				}
				m_last_n11_event = NV11_NOTE_DISPENSED;
				break;

			/* Su kien bao thiet bi da hoan thanh viec lam trong (empty) de phan hoi lenh lam trong (Empty Command) */
			case SSP_POLL_EMPTIED:
				if (m_unit_type == NV_TYPE_NV200)
				{
					for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
					{
						m_stored_notes[channel_index] = ssp6_check_note_level(&m_ssp_cmd, m_channel[channel_index]);
					}
				}
				break;

			/* Su kien tra ve khi thiet bi hieu chuan phan cung that bai (loi phan cung) */
			case SSP_POLL_CALIBRATION_FAIL:
				/* Du lieu tra ve bao gom cac ma loi co san */
				DEBUG_INFO("NV11 >> Calibration fail:\r\n");

				switch (poll->events[i].data1)
				{
					case NO_FAILUE: /* Khong co loi*/
						DEBUG_INFO("NV11 >> No failure\r\n");
					case SENSOR_FLAP: /* Loi cam bien nap thanh toan */
						DEBUG_ERROR("NV11 >> Optical sensor flap\r\n");
					case SENSOR_EXIT: /* Loi cam bien Exit */
						DEBUG_ERROR("NV11 >> Optical sensor exit\r\n");
					case SENSOR_COIL1: /* Loi cam bien cuon 1 */
						DEBUG_ERROR("NV11 >> Coil sensor 1\r\n");
					case SENSOR_COIL2: /* Loi cam bien cuon 2 */
						DEBUG_ERROR("NV11 >> Coil sensor 2\r\n");
					case NOT_INITIALISED: /* Loi thiet bi khong duoc khoi tao */
						DEBUG_INFO("NV11 >> Unit not initialised\r\n");
					case CHECKSUM_ERROR: /* Loi checksum */
						DEBUG_ERROR("NV11 >> Data checksum error\r\n");
					case COMMAND_RECAL: /* Hieu chinh lai bang lenh yeu cau (loi thoi)*/
						DEBUG_ERROR("NV11 >> Recalibration by command required\r\n");
						ssp6_run_calibration(sspC);
						break;
					default:
						break;
				}
				break;

			/* Su kien nay say ra khi MCU yeu cau tam dung thiet bi, chu trinh tra lai se tu dong huy -> bao loi qua trinh tra lai */
			/* Su dung cho dau doc NV200 */
			case SSP_POLL_HALTED: //Halted

				if (m_unit_type == NV_TYPE_NV200)
				{
					DEBUG_ERROR("NV11 >> NV200 failed to pay the note out\r\n");
					for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
					{
						m_stored_notes[channel_index] = ssp6_check_note_level(&m_ssp_cmd, m_channel[channel_index]);
					}
				}
				DEBUG_INFO("NV11 >> Stored Notes: %d\r\n", NV11_GetTotalCashInNoteFloat(lock));
				m_last_n11_event = NV200_PAYOUT_FAILED; /* Thiet lap su kien NV200 thanh toan loi */
				break;

			/* Doi voi cac su kien con lai, in ra ma hinh debug */
			default:
                // DEBUG_VERBOSE("NV11 >> Poll Event: 0x%x\r\n", poll->events[i].event);       
				break;
        }
    }
}

//void nv11_init_mutex(void)
//{
//    if (m_lock_nv11 == NULL)
//    {
//        m_lock_nv11 = xSemaphoreCreateMutex();
//        xSemaphoreGive(m_lock_nv11);
//    }
//}//author:cmt_by_duclq

/**
 * @brief   Ham khoi tao dau doc tien NV11
 * 
 * @param   NONE                     
 * @retval  int NV11_Init -> Tra ve trang thai khoi tao dau doc tien (0 = hoan thanh init, 1 = loi init)
 */
bool NV11_Init(void)
{
//	nv11_init_mutex();//author:cmt_by_duclq
    ssp_serial_flush();
    SSP_RESPONSE_ENUM nv11_response;

    vdm_device_config_t *device_config = vdm_get_device_config();
    memset(&m_ssp_cmd, 0, sizeof(m_ssp_cmd));
    SSP_COMMAND *sspC = &m_ssp_cmd;
    uint32_t i = 0;
    sspC->PortNumber = 1;
    sspC->SSPAddress = 0;
    sspC->Timeout = 300;
    sspC->EncryptionStatus = NO_ENCRYPTION;
    sspC->RetryLevel = 3;
    sspC->BaudRate = 9600;

    for (i = 0; i < 10; i++)
    {
        m_refund_channels[i] = false; /* Xoa toan bo du lieu trong refundChannel */
    }

    /* Kiem tra su co mat cua NV11 (dau NV11 co duoc ket noi voi MCU khong) */
    DEBUG_INFO("NV11 >> Getting information from Note Validator...\r\n");
    /* Ham kiem tra dong bo tra ket qua = 0xF0 -> Tim thay thiet bi */
    nv11_response = ssp6_sync(sspC);
    DEBUG_INFO("NV11 >> response: %u\r\n",nv11_response);		//hiendv
//    nv11_response = 0xF0;			//hiendv
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> NO VALIDATOR FOUND\r\n");
        return false;
    }
    DEBUG_INFO("NV11 >> Validator Found\r\n");
		HAL_Delay(1000); printf("\r\n");
		
    /* Ham cai dat khoa ma hoa (Encryption key) mac dinh -> Ket qua phan hoi ve phai = 0xF0 (cai dat ok) */
    nv11_response = ssp6_setup_encryption(sspC, (unsigned long long)0x123456701234567LL);
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Encryption Failed %d\r\n", nv11_response);
        return false;
    }
    else
    {
        DEBUG_INFO("NV11 >> Encryption Setup\r\n");
    }
		HAL_Delay(1000); printf("\r\n");
		
    /* Ham kiem tra Version cua SSP co phai V6 -> Ham tra ve xac nhan la V6 = 0xF0 */
    nv11_response = ssp6_host_protocol(sspC, 0x06);
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Host Protocol Failed %d\r\n", nv11_response);
        return false;
    }
		HAL_Delay(1000); printf("\r\n");

    /* Ham thu thap toan bo cac gia tri cai dat trong thiet bi (bao gom UnitType =0x07 (NV11), Firmwareversion (03760), Numberofchannel (6) , 
       channeldata = Menh gia tien trong kenh, RealValueMultifier = 100, ProtocolVersion = 6*/
    nv11_response = ssp6_setup_request(sspC, &m_setup_req);
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Setup Request Failed %d\r\n", nv11_response);
        return false;
    }
		HAL_Delay(1000); printf("\r\n");
		
    /* In ra man hinh debug Vision cua phan mem, va kenh tien te */
    DEBUG_INFO("NV11 >> Firmware: %s\r\n", m_setup_req.FirmwareVersion);
    DEBUG_INFO("NV11 >> Channels:\r\n");
    m_channel_num = m_setup_req.NumberOfChannels;
    /* In ra man hinh menh gia tien cua tung kenh tien te: 1 = 1.000, 2 = 2.000, 3 = 5.000, 4 = 10.000, 5 = 20.000, 6 = 50.000*/
    for (i = 0; i < m_setup_req.NumberOfChannels; i++)
    {
        m_channel[i] = m_setup_req.ChannelData[i].value;
        DEBUG_INFO("NV11 >> channel %d: %d %s\r\n", i + 1, m_setup_req.ChannelData[i].value, m_setup_req.ChannelData[i].cc);
    }
		HAL_Delay(1000); printf("\r\n");
    /* Cho phep dau doc tien hoat dong -> Ket qua ok = 0xF0 */
    nv11_response = ssp6_enable(sspC);
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Enable Failed %d\r\n", nv11_response);
        return false;
    }
		HAL_Delay(1000); printf("\r\n");
		
    /* Neu UnitType = 0x03 -> Tuc la thiet bi dang ket noi la SMART Hopper*/
    if (m_setup_req.UnitType == 0x03)
    {
        /* SMART Hopper requires different inhibit commands */
        for (i = 0; i < m_setup_req.NumberOfChannels; i++)
        {
            /* Ham cai dat cac kenh tien te duoc phep tra lai */
            ssp6_set_coinmech_inhibits(sspC, m_setup_req.ChannelData[i].value, m_setup_req.ChannelData[i].cc, ENABLED);

        }
    }
    /* Neu la cac loai dau doc tien khac */
    else
    {
        /* 0x06 -> dau doc SMART Payout, 0x07 -> dau doc NV11*/
        //        if (m_setup_req.UnitType == 0x06 || m_setup_req.UnitType == 0x07)
        //        {
        //            /* Cho phep kich hoat tra lai tien -> ket qua ok = 0xF0 */
        //            if (ssp6_enable_payout(sspC, m_setup_req.UnitType) != SSP_RESPONSE_OK)
        //            {
        //                DEBUG_INFO("NV11 >> Enable payout Failed");
        //                m_count_init_payout_fail++;
        //                if(m_count_init_payout_fail == 5)
        //                {
        //                  m_unit_type = m_setup_req.UnitType;
        //                  m_nv11_initialized = 1;
        //                }
        //                return false;
        //            }
        //        }

        m_unit_type = m_setup_req.UnitType; /* Nhan chung loai dau doc tin vao m_unit_type */

        /* Cai dat cho phep tat ca cac kenh tien te hoat dong (6 kenh) 
           Vd: Neu cho phep kenh 1 - 3, khoa kenh 4 - 16: data = 0x07,0x00 */
        vdm_device_config_lock_resource(portMAX_DELAY);
        uint32_t max_money_read = device_config->accept_cash_max;
				max_money_read = 0xff;//add by duclq
        vdm_device_config_unlock_resource();
        DEBUG_VERBOSE("[%s-%u] ssp6_set_inhibits %08X\r\n", __FUNCTION__, __LINE__, max_money_read);
        nv11_response = ssp6_set_inhibits(sspC, max_money_read, 0xFF);
        if (nv11_response != SSP_RESPONSE_OK)
        {
            DEBUG_ERROR("NV11 >> Inhibits Failed");
            return false;
        }
        /* Neu dau doc tien la loai NV11 */
        if (m_unit_type == NV_TYPE_NV11)
        {
            /* In ra man hinh chung loai dau doc NV11 */
            DEBUG_INFO("NV11 >> Unit Type: NV11\r\n");
            /* Lay so to tien chua trong FloatNote (dau tra lai) hien thi len man hinh */
            int32_t stored_notes = ssp6_get_stored_notes(&m_ssp_cmd);
            DEBUG_INFO("NV11 >> Stored Notes: %d\r\n", stored_notes);

            m_last_num_note_payout = stored_notes;
            m_num_note_payout = stored_notes;

            /* xoa loi payout note */
            m_error_payout = 0;

            vdm_device_config_lock_resource(portMAX_DELAY);
            //vdm_device_config_sync_error_to_fram();//cmt_by_duclq
            device_config->sensor_error.name.payout = m_error_payout;
            vdm_device_config_unlock_resource();
        }
        /* Neu dau doc tien la NV200 */
        else if (m_unit_type == NV_TYPE_NV200)
        {
            /* In ra man hinh chung loai dau doc Smart Payout */
            DEBUG_INFO("NV11 >> Unit Type: Smart Payout\r\n");
            DEBUG_INFO("NV11 >> Stored Notes:\r\n");
            /* Lay so to tien cua tung loai tien te cai dat trong FloatNote (dau tra lai), hien thi ra man hinh */
            for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
            {
                m_stored_notes[channel_index] = ssp6_check_note_level(&m_ssp_cmd, m_channel[channel_index]);
                DEBUG_INFO("%d: %d\r\n", m_channel[channel_index], m_stored_notes[channel_index]);
            }
        }
        /* Neu dau doc la NV9 (khong co dau tra lai tien)*/
        else if (m_unit_type == NV_TYPE_NV9)
        {
            /* In ra man hinh chung loai dau doc NV9 */
            DEBUG_INFO("NV11 >> Unit Type: NV9\r\n");
            vdm_device_config_lock_resource(portMAX_DELAY);
            //vdm_device_config_sync_error_to_fram();//cmt_by_duclq
            device_config->peripheral.feature_enable.name.refund_enable = 0;
            //            Perh_RefundOff();
            vdm_device_config_unlock_resource();
        }
        /* Neu la cac loai dau doc khac */
        else
        {
            /* In ra man hinh khong nhan dang duoc dau doc */
            DEBUG_INFO("NV11 >> Unit Type: UNKNOWN\r\n");
            return false;
        }
        /* Neu dau doc tien la loai NV11 */
        if (m_unit_type == NV_TYPE_NV11)
        {
            for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
            {
                /* Lay kenh tien te cho phep tra lai cai dat trong ROM */
                vdm_device_config_lock_resource(portMAX_DELAY);

                if (VDM_DEVICE_CONFIG_REFUND_CHANNEL_ENABLE(device_config, channel_index))
                {
                    vdm_device_config_unlock_resource();
                    /* Cai dat menh gia tien te luu tru tren FloatNote de tra lai */
                    if (NV11_SetDenominationForChange(m_channel[channel_index], false))
                    {
                        return false;
                    }
                    m_refund_note = m_channel[channel_index]; /* Lay gia tri kenh tien te vao m_refund_note */
                    DEBUG_INFO("Refund note %u\r\n", m_refund_note);
                    break;
                }
                else
                {
                    vdm_device_config_unlock_resource();
                }

                if (channel_index > 7)
                {
                    break;
                }
            }

            /* 0x06 -> dau doc SMART Payout, 0x07 -> dau doc NV11 cho phep de dua tien len tren */
            if (m_setup_req.UnitType == 0x06 || m_setup_req.UnitType == 0x07)
            {
                /* Cho phep kich hoat tra lai tien -> ket qua ok = 0xF0 */
                if (ssp6_enable_payout(sspC, m_setup_req.UnitType) != SSP_RESPONSE_OK)
                {
                    DEBUG_ERROR("NV11 >> Enable payout Failed\r\n");
                    m_count_init_payout_fail++;
                    DEBUG_INFO("NV11 >> Payout init %d times\r\n", m_count_init_payout_fail);

                    // if (m_count_init_payout_fail == 13)
                    // {
                    //     m_count_init_payout_fail = 0;
                    //     m_unit_type = m_setup_req.UnitType;
                    //     m_nv11_initialized = 1;
                    //     NV11_ClearLatestEvent(false);
                    //     NV11_DisplayOn();
                    //     m_init_payout_fault = true;
                    //     DEBUG_ERROR("Init payout fault\r\n");
                    //     return true;
                    // }
                    return false;
                }
                else
                {
                    m_count_init_payout_fail = 0;
                    m_unit_type = m_setup_req.UnitType;
                    m_nv11_initialized = 1;
                    NV11_ClearLatestEvent(false);
                    NV11_DisplayOn();
                    DEBUG_INFO("NV11 enable payout success\r\n");
                    return true;
                }
            }
        }

        /* Neu dau doc tien la loai NV200 */
        else if (m_unit_type == NV_TYPE_NV200)
        {
            for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
            {
                vdm_device_config_lock_resource(portMAX_DELAY);

                /* Lay cac kenh tien te cho phep tra lai cai dat trong ROM */
                if (VDM_DEVICE_CONFIG_REFUND_CHANNEL_ENABLE(device_config, channel_index))
                {
                    vdm_device_config_unlock_resource();
                    /* Cai dat menh gia tien te luu tru tren FloatNote de tra lai */
                    if (NV11_SetDenominationForChange(m_channel[channel_index], false))
                    {
                        return false;
                    }
                    m_refund_channels[channel_index] = true;
                }
                else
                {
                    vdm_device_config_unlock_resource();
                }

                if (channel_index > 7)
                {
                    break;
                }
            }
        }
    }

    m_nv11_initialized = true; /* Xac nhan cai dat da hoan thanh */

    NV11_DisplayOn(); /* Cho phep dau doc hien thi khung vien */

    return true; /* Bao trang thai da cai dat xong */
}
/**
 * @brief   Ham lay chung loai may doc tien
 * 
 * @param   NONE                     
 * @retval  m_unit_type -> Chung loai may doc tien
 */
int32_t NV11_GetUnitType(bool lock)
{
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    int32_t retval = m_unit_type;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }

    return retval;
}
/**
 * @brief   Ham cho phep thiet bi hoat dong, cai dat
 * 
 * @param   NONE                     
 * @retval  int nv11_enable -> Trang thai cai cho phep hoat dong (True or False)
 */
bool nv11_enable(bool lock)
{
    bool retval = true;
    SSP_RESPONSE_ENUM error_code;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }
    if (!m_nv11_initialized)
    {
        retval = false;
        goto exit;
    }

    SSP_COMMAND *sspC = &m_ssp_cmd;

    /* enable the validator */
    error_code = ssp6_enable(sspC);
    if (error_code != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Enable NV11 Failed\r\n");
        {
            retval = false;
            goto exit;
        }
    }
    DEBUG_INFO("NV11 >> Enable NV11 Successfully\r\n");

exit:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}
/**
 * @brief   Ham vo hieu hoa thiet bi
 * 
 * @param   NONE                     
 * @retval  int NV11_Disable -> Trang thai vo hieu hoa (True or False)
 */
int32_t NV11_Disable(bool lock)
{
    int32_t retval = 0;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    if (!m_nv11_initialized)
    {
        retval = 1;
        goto end;
    }

    SSP_COMMAND *sspC = &m_ssp_cmd;

    /* disable the validator */
    if (ssp6_disable(sspC) != SSP_RESPONSE_OK)
    {
        DEBUG_WARN("NV11 >> Disable Failed\r\n");
        retval = 1;
    }
    else
    {
        DEBUG_VERBOSE("NV11 >> Disable Successfully\r\n");
    }

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }

end:
    return retval;
}
/**
 * @brief   Ham cho phep thiet bi tra lai tien
 * 
 * @param   NONE                     
 * @retval  int NV11_EnablePayoutb-> Trang thai cho phep tra lai tien (True or False)
 */
int32_t NV11_EnablePayout(bool lock)
{
    int32_t retval;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    if (!m_nv11_initialized)
    {
        retval = 1;
        goto end;
    }


    /* Ham cho phep tra lai tien, m_setup_req.UnitType = NV11 */
    if (ssp6_enable_payout(&m_ssp_cmd, m_setup_req.UnitType) != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Enable payout Failed\r\n");
        //        m_last_n11_event = NV11_ERROR;
    }
    else
    {
        DEBUG_INFO("NV11 >> Enable Payout Successfully\r\n");
        retval = 0;
    }
end:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}

/**
 * @brief   Ham cai dat cac menh gia tien chap nhan
 * 
 * @param   NONE                     
 * @retval  0 neu tat chuc nang tra lai tien thanh cong
 *          1 neu ta chuc nang tra lai tien that bai
 */
bool NV11_SetAcceptNote(uint8_t channel, bool lock)
{
    bool retval = true;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }
    if (ssp6_set_inhibits(&m_ssp_cmd, channel, 0xFF) != SSP_RESPONSE_OK)
    {
        retval = false;
    }

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}

/**
 * @brief   Ham tat chuc nang tra lai tien
 * 
 * @param   NONE                     
 * @retval  0 neu tat chuc nang tra lai tien thanh cong
 *          1 neu ta chuc nang tra lai tien that bai
 */
int32_t NV11_DisablePayout(bool lock)
{
    int32_t retval = 0;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    if (!m_nv11_initialized)
    {
        retval = 1;
        goto end;
    }

    /* Ham cho phep tra lai tien, m_setup_req.UnitType = NV11 */
    if (ssp6_disable_payout(&m_ssp_cmd) != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Disable payout Failed\r\n");
        //        m_last_n11_event = NV11_ERROR;
        retval = 1;
        goto end;
    }
    DEBUG_INFO("NV11 >> Disable Payout Successfully\r\n");

end:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}
/**
 * @brief   Ham cai dat menh gia tien mat luu tru de tra lai
 * 
 * @param   uint32_t denomination -> Menh gia tien cai dat (Vd: 1000, 2000, 5000, 10000, 20000, 50000)                    
 * @retval  int NV11_SetDenominationForChange -> Trang thai cai dat menh gia (True or False)
 */
int32_t NV11_SetDenominationForChange(uint32_t denomination, bool lock)
{
    int32_t retval = 0;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    /* Ham cai dat menh gia tien tra lai, gia tri = menh gia*100 (Vd: 5000*100) */
    if (ssp6_set_route(&m_ssp_cmd, denomination * 100, "VND", 0) != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> ERROR: Set routing failed: %d\r\n", denomination);
        retval = 1;
    }
    else
    {
        DEBUG_INFO("NV11 >> Set routing Successfully: %d\r\n", denomination);
    }
exit:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}
/**
 * @brief   Ham dinh tuyen menh gia tien tra lai cho NV200
 * 
 * @param   m_refund_channels[channel_index] -> Menh gia cai dat
 * @retval  int NV11_RoutingPayoutForNV200 -> Trang thai cai dat menh gia (True or False)
 */
int32_t NV11_RoutingPayoutForNV200(bool lock)
{
    int32_t res = 0;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
    {
        if (m_refund_channels[channel_index])
        {
            res = NV11_SetDenominationForChange(m_channel[channel_index], false);
            if (res > 0)
            {
                if (lock)
                {
                    //author:cmt_by_duclq//NV11_UNLOCK();
                }
                return res;
            }
        }
    }
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return 0;
}
/**
 * @brief   Ham cai dat tat ca cac menh gia tien mat luu tru de tra lai
 * 
 * @param   m_channel[channel_index] -> Tat ca cac menh gia tien cai dat (Vd: 1000, 2000, 5000, 10000, 20000, 50000)
 * @retval  int NV11_RoutingAllToCashbox -> Trang thai cai dat menh gia (True or False)
 */
int32_t NV11_RoutingAllToCashbox(void)
{
    for (int32_t channel_index = 0; channel_index < m_channel_num; channel_index++)
    {
        if (ssp6_set_route(&m_ssp_cmd, m_channel[channel_index] * 100, "VND", 1) != SSP_RESPONSE_OK)
        {
            DEBUG_ERROR("NV11 >> ERROR: Set routing failed\r\n");
            return 1;
        }
        DEBUG_INFO("NV11 >> Set routing Successfully\r\n");
    }
    return 0;
}
/**
 * @brief  Ham thuc hien lenh giu tien (ngam tien) tai cua NV11   
 * 
 * @param  NONE   
 * @retval int NV11_Hold -> Tra ve trang thai giu tien (OK -> F0, Fail -> 0xF8)  
 */
int32_t NV11_Hold(bool lock)
{
    int32_t retval;

    DEBUG_INFO("NV11 hold\r\n");
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    if (!m_nv11_initialized) /*Neu chua khoi tao, tra ket qua = 1*/
    {
        retval = 1;
        goto done;
    }

    m_is_holding = true; /* Bao dang giu (ngam) tien */
    retval = ssp6_hold(&m_ssp_cmd);
    if (retval != SSP_RESPONSE_OK)
    {
        DEBUG_WARN("NV11 hold error code %02X\r\n", retval);
    }

done:

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}
/**
 * @brief  Ham thuc hien lenh huy giu tien (ngam tien) tai cua NV11   
 * 
 * @param  NONE 
 * @retval int NV11_UnHold -> Tra ve trang thai huy giu tien
 */
void NV11_UnHold(bool lock)
{
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    m_is_holding = false; /* Bao huy giu (ngam) tien */
    
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
}
/**
 * @brief  Ham tu choi nhan tien, tra lai tien cho khach
 * 
 * @param  NONE 
 * @retval int NV11_UnHold -> Tra ve trang thai huy giu tien
 */
int32_t NV11_Reject(bool lock)
{
    int32_t retval = 0;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    if (!m_nv11_initialized) /*Neu chua khoi tao, tra ket qua = 1*/
    {
        retval = 1;
        goto exit;
    }

    m_is_holding = false;                        /* Bao huy giu (ngam) tien */
    ssp6_reject(&m_ssp_cmd);                 /* Thuc hien lenh tu choi nhan tien */
    while (m_last_n11_event != NV11_NOTE_REJECTED) /* Kiem tra neu su kien cuoi khong phai la Reject */
    {
        NV11_Process(false); /* Thuc hien ham xu ly NV11 */
        HAL_Delay(1);//cmt_by_duclq
    }
exit:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval; /* Thuc hien xong lenh tra ket qua = 0*/
}
/**
 * @brief  Ham reset bo dem luu tru ve 0, dieu huong cac khoan tien luu tru vao hop tien mat
 * 
 * @param  NONE 
 * @retval int NV11_UnHold -> Tra ve trang thai huy giu tien
 */
int32_t NV11_EmptyNotes(bool lock)
{
	int32_t retval;
	if (lock)
	{
		//author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
	}

    if (!m_nv11_initialized) /*Neu chua khoi tao, tra ket qua = 1*/
    {
        retval = 1;
        goto end;
    }
    retval = 0;
    ssp6_empty_notes(&m_ssp_cmd);

end:
	if (lock)
	{
		//author:cmt_by_duclq//NV11_UNLOCK();
	}
    return retval;
}
/**
 * @brief  Ham lay gia tri so kenh tien te cua NV 11
 * 
 * @param  NONE 
 * @retval int int NV11_GetChannelNumbe -> Tra ve so kenh tien te
 */
int32_t NV11_GetChannelNumber()
{
    return m_channel_num;
}
/**
 * @brief  Ham lay menh gia tien trong tung kenh tien te cua NV11
 * 
 * @param  NONE 
 * @retval int int NV11_GetChannelNumbe -> Tra ve so kenh tien te
 */
int32_t NV11_GetChannelValue(int32_t channel)
{
    if (channel < m_channel_num) /* Neu so kenh can kiem tra < so kenh khai bao NV11 (6) */
    {
        return m_channel[channel]; /* Tra ket qua menh gia tien trong kenh */
    }
    return -1; /* Neu lon hon, tra ket qua la -1*/
}
/**
 * @brief  NV200 Ham lay so to tien cua tung menh gia luu tru thanh toan (tra lai) 
 * 
 * @param  int channel -> Kenh menh gia can kiem tra
 * @retval int NV11_GetStoredNoteByChannel -> So to tien tuong ung voi kenh kiem tra
 */
int32_t NV11_GetStoredNoteByChannel(int32_t channel)
{
    int32_t retval = -1;
    //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    if (channel < m_channel_num)
    {
        retval = m_stored_notes[channel];
    }
    //author:cmt_by_duclq//NV11_UNLOCK();
    return retval;
}
/**
 * @brief  Ham lay menh gia luu tru thanh toan (tra lai) cua NV11 
 * 
 * @param  NONE 
 * @retval int NV11_GetDenominationForChange -> Menh gia luu tru thanh toan       
 */
int32_t NV11_GetDenominationForChange(bool lock)
{
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    int32_t retval = m_refund_note;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }

    return retval;
}

bool nv11_is_availble(bool lock)
{
	bool retval;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    retval = m_nv11_initialized;

    if (lock)
    {
    	//author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}

int32_t NV11_GetAvailableChange(bool lock)
{
    int32_t retval = 0;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }
    if (!m_nv11_initialized)         /*Neu chua khoi tao, tra ket qua = 1*/
    {
        retval = 0;
        goto end;
    }
    /* Neu la loai dau doc tien NV11 */
    if (m_unit_type == NV_TYPE_NV11)
    {
        /*Tong so tien = so luong trong note Float * Menh gia tien tra lai
          NV11 cai dat duoc 1 menh gia tra lai
        */
        retval = (ssp6_get_stored_notes(&m_ssp_cmd) * NV11_GetDenominationForChange(false));
    }
    /* Neu la loai dau doc tien NV200 */
    else if (m_unit_type == NV_TYPE_NV200)
    {
        int32_t stored_notes = 0;
        for (int32_t i = 0; i < m_channel_num; i++)
        {
            /* Tong so tien = Tong (so luong menh gia x menh gia) 
              NV 200 cai dat duoc nhieu menh gia tra lai
            */
            stored_notes += (m_stored_notes[i] * m_channel[i]);
        }
        retval = stored_notes;
    }
    /* Neu la loai dau doc tien khac, tra ket qua = 0 */
    else
    {
        retval = 0;
    }

end:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}

/**
 * @brief  Ham tong so tien trong NoteFloat (thanh toan, tra lai) 
 * 
 * @param  NONE 
 * @retval int NV11_GetTotalCashInNoteFloat -> Tong so tien co trong NoteFloat     
 */
int32_t NV11_GetTotalCashInNoteFloat(bool lock)
{
    int32_t retval = 0;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    if (!m_nv11_initialized) /*Neu chua khoi tao, tra ket qua = 1*/
    {
        goto end;
    }
    /* Neu la loai dau doc tien NV11 */
    if (m_unit_type == NV_TYPE_NV11)
    {
        /*Tong so tien = so luong trong note Float * Menh gia tien tra lai
          NV11 cai dat duoc 1 menh gia tra lai
        */
        retval = (ssp6_get_stored_notes(&m_ssp_cmd) * m_refund_note);
    }
    /* Neu la loai dau doc tien NV200 */
    else if (m_unit_type == NV_TYPE_NV200)
    {
        int32_t stored_notes = 0;
        for (int32_t i = 0; i < m_channel_num; i++)
        {
            /* Tong so tien = Tong (so luong menh gia x menh gia) 
              NV 200 cai dat duoc nhieu menh gia tra lai
            */
            stored_notes += (m_stored_notes[i] * m_channel[i]);
        }
        retval = stored_notes;
    }
    /* Neu la loai dau doc tien khac, tra ket qua = 0 */
    else
    {
        return 0;
    }
end:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}
/**
 * @brief  Ham kiem tra trang thai dau doc dang giu (ngam) tien hay khong
 * 
 * @param  NONE 
 * @retval m_lastest_note -> Menh gia tien dang giu    
 */
bool NV11_GetHoldingCash(uint32_t *result, bool lock)
{
    bool retval = false;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }
    if (!m_nv11_initialized) /* Neu chua khoi tao, tra ket qua = 1*/
    {
        goto end;
    }

    if (m_is_holding)
    {
        retval = true;
        *result = m_lastest_note;
    }
end:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}

/**
 * @brief  Ham lay su kien gan nhat NV11 thuc hien
 * 
 * @param  NONE 
 * @retval m_last_n11_event -> Su kien gan nhat
 */
nv11_event_t NV11_GetLatestEvent(bool lock)
{
    nv11_event_t evt;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    evt = m_last_n11_event;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }

    return evt;
}
/**
 * @brief  Ham xoa toan bo su kien
 * 
 * @param  NONE 
 * @retval NONE 
 */
void NV11_ClearLatestEvent(bool lock)
{
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    m_last_n11_event = NV11_EVENT_NONE;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
}
/**
 * @brief  Ham lay menh gia to tien gan nhat NV11
 * 
 * @param  NONE 
 * @retval m_lastest_note -> Menh gia to tien gan nhat
 */
int32_t NV11_GetLatestNote(bool lock)
{
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    int32_t retval = m_lastest_note;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}

static nv11_reconnect_event_callback_t m_nv11_callback;
void nv11_register_reconnect_event(nv11_reconnect_event_callback_t callback)
{
    m_nv11_callback = callback;
}
/**
 * @brief  Ham xu ly dau doc tien
 * 
 * @param  NONE 
 * @retval m_last_n11_event -> Cac su kien dau doc tien tra ve gan nhat
 */
// bool rewwrite_sdcard = true;
void NV11_Process(bool lock)
{
    SSP_RESPONSE_ENUM rsp_status;
    SSP_POLL_DATA6 poll;
    static uint8_t count = 0;
    static uint32_t reconnect_count = 0;
    static uint32_t old_tick = 0;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    if (!m_nv11_initialized) /* Neu chua khoi tao -> thoat*/
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
        return;
    }
    uint32_t now = HAL_GetTick();//cmt_by_duclq
    if (now - old_tick > (uint32_t)150)
    {
        old_tick = now;
        if (m_last_n11_event == NV11_ERROR) /* Neu su kien gan nhat bao NV11 loi */
        {
            count++;
            /* Check connection */
            if (count > 20) /* Neu su kien van giu trong 20 chu ki may*/
            {
                count = 0;
                DEBUG_ERROR("NV11 >> NV11 is error. Trying to reconnect...\r\n"); /* Bao loi NV11 len man hinh debug*/
                if (m_nv11_callback)
                {
                    m_nv11_callback(false);
                }

                if (reconnect_count++ > 12)
                {
                    DEBUG_WARN("Reset NV11\r\n");
                    // vdm_sdcard_log(VDM_SDCARD_LOG_LEVEL_ERROR, "Reset nv11\r\n");
                    nv11_reset(false);
                    reconnect_count = 0;
                    HAL_Delay(1000);//cmt_by_duclq
                    NVIC_SystemReset();     // huytv dirty fix
                }
                ssp_serial_deinitialize();
                ssp_serial_initialize();
                if (NV11_Init()) /* Thuc hien khoi tao lai NV11 */
                {
                    DEBUG_WARN("NV11 >> NV11 is working now\r\n"); /* Neu ok -> bao NV11 hoat dong len man hinh debug*/
                    m_last_n11_event = NV11_EVENT_NONE;               /* Xoa su kien NV11 */
                    if (m_nv11_callback)
                    {
                        m_nv11_callback(true);
                        reconnect_count = 0;
                    }
                }
            }
        }
        else if (m_is_holding == true) /* Neu dau doc dang giu tien */
        {
            SSP_RESPONSE_ENUM err = ssp6_hold(&m_ssp_cmd);
            if (err != SSP_RESPONSE_OK) /* Neu dat lenh giu tien Fail */
            {
                m_last_n11_event = NV11_ERROR; /* Bao loi NV11 */
                DEBUG_ERROR("[%s line %u] NV11 error %02X\r\n", __FUNCTION__, __LINE__, err);
//                vdm_sdcard_log(VDM_SDCARD_LOG_LEVEL_ERROR, "NV11 hold, response error %02X\r\n", err);//cmt_by_duclq
                m_is_holding = false;        /* Xoa trang thai dang giu tien */
            }
            else
            {
                DEBUG_VERBOSE("NV11 hold success\r\n");
            }
        }
        /* Cac su kien khac */
        else
        {
            /* Thuc hien ham doc cac su kien trong NV11, neu phan hoi ve khong phai la 0xF0 */
            if ((rsp_status = ssp6_poll(&m_ssp_cmd, &poll)) != SSP_RESPONSE_OK)
            {
                if (rsp_status == SSP_RESPONSE_TIMEOUT) /* Neu phan hoi su kien ve la SSP_RESPONSE_TIMEOUT */
                {
                    // If the poll timed out, then give up
                    DEBUG_ERROR("NV11 >> SSP Poll Timeout\r\n"); /* Bao len man hinh debug */
                    m_last_n11_event = NV11_ERROR;                  /* Bao loi NV11 */
                    DEBUG_ERROR("[%s line %u] NV11 error %02x\r\n", __FUNCTION__, __LINE__, rsp_status);
                }
                else
                {
                    if (rsp_status == SSP_RESPONSE_KEY_NOT_SET) /* Neu phai hoi su kien mot khoa ma hoa khong duoc cai dat*/
                    {
                        // The validator has responded with key not set, so we should try to negotiate one
                        if (ssp6_setup_encryption(&m_ssp_cmd, (unsigned long long)0x123456701234567LL) != SSP_RESPONSE_OK) /* Thuc hien lai viec cai dat mot khoa ma hoa defaul */
                        {
                            DEBUG_ERROR("NV11 >> Encryption Failed\r\n"); /* Cai dat loi, bao len man hinh debug */
                            m_last_n11_event = NV11_ERROR;                   /* Bao loi NV11 */
                        }
                        else /* Cai dat khoa ma hoa thanh cong */
                        {
                            DEBUG_INFO("NV11 >> Encryption Setup success\r\n"); /* Bao hoan thanh cai dat */
                        }
                    }
                    else
                    {
                        DEBUG_INFO("NV11 >> Unexpected error %02X\r\n", rsp_status); /* Bao hoan thanh cai dat */
                        m_last_n11_event = NV11_ERROR; /* Cac phai hoi con lai, ma khong phai 0xF0 -> Bao loi NV11 */
                    }
                }
            }
            /* Thuc hien ham doc cac su kien trong NV11, neu phan hoi ve la 0xF0 -> phan tich cac su kien */
            else
            {
                parse_poll(&m_ssp_cmd, &poll, false); /* Thuc hien phan tich cac su kien NV11 */
            }
        }
        HAL_Delay(1);//cmt_by_duclq
    }
    if (lock)
    {
    	//author:cmt_by_duclq//NV11_UNLOCK();
    }
}

bool nv11_reset(bool lock)
{
    bool retval = false;
    if (!m_nv11_initialized) /* Neu chua khoi tao -> tra ket qua = 1 */
    {
        goto end;
    }

    if (lock) 
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    SSP_RESPONSE_ENUM ssp6_code = ssp6_reset(&m_ssp_cmd);
    if (ssp6_code != SSP_RESPONSE_OK)
    {
        DEBUG_WARN("NV11 reset failed %02X\r\n", ssp6_code);
        goto end;
    }       
    retval = true;

end:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return retval;
}

/* This is the blocking function */
/**
 * @brief  Ham thanh toan, tra lai tien dau doc NV11, moi lan thanh toan chi duoc 1 to = menh gia cai dat tra lai
 * 
 * @param  NONE 
 * @retval int NV11_PayoutNote -> Trang thai thanh toan (0 -> ok,1 -> Fail)
 */
int32_t NV11_PayoutNote(bool lock)
{
    uint8_t count_down_value = 60;

    if (!m_nv11_initialized) /* Neu chua khoi tao -> tra ket qua = 1 */
    {
        return 1;
    }

    if (lock) 
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    DEBUG_INFO("NV11 >> Payout note start\r\n");
    
    SSP_RESPONSE_ENUM ret;
    uint32_t tick = 0;
    NV11_ClearLatestEvent(false); /* Xoa toan bo su kien */
    m_action_payout_fault = false;
    
    /* FloatNote se thanh toan to tien cuoi cung vua duoc luu tru, neu co the tra tien ham se tra ra ket qua 
      la 0xF0 = SSP_RESPONSE_OK*/
    ret = ssp6_payout_note(&m_ssp_cmd); /* Tra ve trang thai co the thanh toan hay khong */
    if (ret == SSP_RESPONSE_OK)            /* Neu co the thanh toan */
    {
        tick = HAL_GetTick();                              /* Tinh time out *///cmt_by_duclq
        while (NV11_GetLatestEvent(false) != NV11_NOTE_DISPENSED) /* Kiem tra co su kien tranh chap tien hay khong*/
        {
            /* hien thi dem nguoc */
            if ((HAL_GetTick() - tick > 1000) && count_down_value != 0)//cmt_by_duclq
            {
                display_payout_countdown(count_down_value);
                count_down_value--;
                tick = HAL_GetTick();//cmt_by_duclq
            }
            else if (count_down_value == 0)
            {
                DEBUG_WARN("NV11 >> Count down reject end\r\n");
                DEBUG_ERROR("NV11 >> Note dispense error by timeout\r\n");
                m_action_payout_fault = true;
                if (lock)
                {
                    //author:cmt_by_duclq//NV11_UNLOCK();
                }
                return 1;
            }

            NV11_Process(false); /* Thuc hien lenh xu ly cua NV11 */
            if (NV11_GetLatestEvent(false) == NV11_NOTE_DISPENSED)
            {
                DEBUG_INFO("NV11 >> Note dispense by flag\r\n");
                if (lock)
                {
                    //author:cmt_by_duclq//NV11_UNLOCK();
                }
                return 0;
            }

            if (NV11_GetLatestEvent(false) == NV11_ERROR) /* Neu co su kien NV11 loi -> thoat, tra gia tri = 1 */
            {
                DEBUG_ERROR("NV11 >> Note dispense error by payout note\r\n");
                if (lock)
                {
                    //author:cmt_by_duclq//NV11_UNLOCK();
                }
                return 1;
            }

            /* Check tick all time */
            //            Perh_ProcessAllWhile();
            HAL_Delay(1);//cmt_by_duclq
        }
        if (lock)
        {
            //author:cmt_by_duclq//NV11_UNLOCK();
        }
        return 0; /* Neu hoan thanh ok -> thoat, tra gia tri = 0*/
    }
    DEBUG_ERROR("NV11 >> Note dispense error by nv11 error\r\n");
    m_last_n11_event = NV11_ERROR; /* Neu khong the thanh toan, nhan su kien NV11 loi */
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }
    return 1;
}

bool NV11_GetStatusPayoutNote(bool lock)
{
    bool retval = false;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    retval = m_action_payout_fault;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }

    return retval;
}

/* This is the blocking function */
/**
 * @brief  NV200 Ham thanh toan, tra lai tien
 * 
 * @param  NONE 
 * @retval int NV11_PayoutValue -> Trang thai thanh toan (0 -> ok,1 -> Fail)
 */
int32_t NV11_PayoutValue(int32_t value, bool lock)
{
    int32_t retval = 1;
    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }
    if (!m_nv11_initialized) /* Neu chua khoi tao -> tra ket qua = 1 */
    {
        retval = 1;
        goto end;
    }

    NV11_ClearLatestEvent(true); /* Xoa toan bo su kien */
    /* Thuc hien lenh thanh toan, gia tri = menh gia*100, lenh 0x58 = Payout_amount, 0x19 = Test_Payout_amount*/
    if (ssp6_payout(&m_ssp_cmd, value * 100, "VND", 0x58) == SSP_RESPONSE_OK) /* Neu thuc hien thanh toan ok */
    {
        retval = 0; /* Thoat, tra gia tri = 0 */
    }
    else
    {
        retval = 1;
    }

end:
    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }

    return retval;
}

/* Ham su dung cho NV200 */
bool NV200_ValueIsOnForPayout(int32_t value, bool lock)
{
    bool retval = false;

    if (lock)
    {
        //author:cmt_by_duclq//NV11_LOCK(portMAX_DELAY);
    }

    for (int32_t i = 0; i < m_channel_num; i++)
    {
        if (m_channel[i] == value && m_refund_channels[i])
        {
            retval = true;
            break;
        }
    }

    if (lock)
    {
        //author:cmt_by_duclq//NV11_UNLOCK();
    }

    return retval;
}

/**
 * @brief  Ham bat led vien dau doc tien NV11
 * 
 * @param  NONE 
 * @retval int NV11_DisplayOn -> Trang thai hoan thanh
 */
int32_t NV11_DisplayOn(void)
{
    if (!m_nv11_initialized)
    {
        return 1;
    }

    return ssp6_display_on(&m_ssp_cmd);
}
/**
 * @brief  Ham tat led vien dau doc tien NV11
 * 
 * @param  NONE 
 * @retval int NV11_DisplayOff -> Trang thai hoan thanh
 */
int32_t NV11_DisplayOff(void)
{
    if (!m_nv11_initialized)
    {
        return 1;
    }

    return ssp6_display_off(&m_ssp_cmd);
}
//-----------------------------------------------------------------------------------------------------
int8_t vdm_NV11_Init(void)
{
    ssp_serial_flush();
    SSP_RESPONSE_ENUM nv11_response;

    vdm_device_config_t *device_config = vdm_get_device_config();
    memset(&m_ssp_cmd, 0, sizeof(m_ssp_cmd));
    SSP_COMMAND *sspC = &m_ssp_cmd;
    uint32_t i = 0;
    sspC->PortNumber = 1;
    sspC->SSPAddress = 0;
    sspC->Timeout = 300;
    sspC->EncryptionStatus = NO_ENCRYPTION;
    sspC->RetryLevel = 3;
    sspC->BaudRate = 9600;
	
    for (i = 0; i < 10; i++)
    {
        m_refund_channels[i] = false; /* Xoa toan bo du lieu trong refundChannel */
    }
		
		/* Kiem tra su co mat cua NV11 (dau NV11 co duoc ket noi voi MCU khong) */
    DEBUG_INFO("NV11 >> Getting information from Note Validator...\r\n");
    /* Ham kiem tra dong bo tra ket qua = 0xF0 -> Tim thay thiet bi */
    nv11_response = ssp6_sync(sspC);
    if (nv11_response != SSP_RESPONSE_OK)
    {
      DEBUG_ERROR("NV11 >> NO VALIDATOR FOUND\r\n");
			return ERROR_SYNC;
    }
    DEBUG_INFO("NV11 >> Validator Found\r\n");
		HAL_Delay(1000);
		
		/* Ham cai dat khoa ma hoa (Encryption key) mac dinh -> Ket qua phan hoi ve phai = 0xF0 (cai dat ok) */
    nv11_response = ssp6_setup_encryption(sspC, (unsigned long long)0x123456701234567LL);
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Encryption Failed %d\r\n", nv11_response);
        return ERROR_KEY_NOT_SET;
    }
    else
    {
        DEBUG_INFO("NV11 >> Encryption Setup\r\n");
    }
		HAL_Delay(1000);
		
    /* Ham kiem tra Version cua SSP co phai V6 -> Ham tra ve xac nhan la V6 = 0xF0 */
    nv11_response = ssp6_host_protocol(sspC, 0x06);
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Host Protocol Failed %d\r\n", nv11_response);
			return ERROR_HOST_PROT;
    }
		HAL_Delay(1000); printf("\r\n");
		
    /* Ham thu thap toan bo cac gia tri cai dat trong thiet bi (bao gom UnitType =0x07 (NV11), Firmwareversion (03760), Numberofchannel (6) , 
       channeldata = Menh gia tien trong kenh, RealValueMultifier = 100, ProtocolVersion = 6*/
    nv11_response = ssp6_setup_request(sspC, &m_setup_req);
    if (nv11_response != SSP_RESPONSE_OK)
    {
        DEBUG_ERROR("NV11 >> Setup Request Failed %d\r\n", nv11_response);
			return ERROR_SETUP_REQUEST;
    }
		HAL_Delay(1000); printf("\r\n");
		
    /* In ra man hinh debug Vision cua phan mem, va kenh tien te */
    DEBUG_INFO("NV11 >> Firmware: %s\r\n", m_setup_req.FirmwareVersion);
    DEBUG_INFO("NV11 >> Channels:\r\n");
    m_channel_num = m_setup_req.NumberOfChannels;
    /* In ra man hinh menh gia tien cua tung kenh tien te: 1 = 1.000, 2 = 2.000, 3 = 5.000, 4 = 10.000, 5 = 20.000, 6 = 50.000*/
    for (i = 0; i < m_setup_req.NumberOfChannels; i++)
    {
        m_channel[i] = m_setup_req.ChannelData[i].value;
        DEBUG_INFO("NV11 >> channel %d: %d %s\r\n", i + 1, m_setup_req.ChannelData[i].value, m_setup_req.ChannelData[i].cc);
    }
		HAL_Delay(1000); printf("\r\n");
		
//    /* disable the validator */
//    if (ssp6_disable(sspC) != SSP_RESPONSE_OK) DEBUG_WARN("NV11 >> Disable Failed\r\n");
//    else DEBUG_VERBOSE("NV11 >> Disable Successfully\r\n");
//		HAL_Delay(1000); printf("\r\n");
		
//			nv11_response = ssp6_set_inhibits(sspC, 0x00, 0x00);
//			if (nv11_response != SSP_RESPONSE_OK) DEBUG_ERROR("NV11 >> Inhibits Failed");
//			else printf("NV11 >> Set Inhibits done!\r\n");
//			HAL_Delay(1000); printf("\r\n");
		
			nv11_response = ssp6_set_inhibits(sspC, 0xFF, 0xFF);
			if (nv11_response != SSP_RESPONSE_OK)
			{
				DEBUG_ERROR("NV11 >> Inhibits Failed");
				return ERROR_SET_INHIBITS;
			}
			else printf("NV11 >> Set Inhibits done!\r\n");
			HAL_Delay(1000); printf("\r\n");
			
		
    /* Cho phep dau doc tien hoat dong -> Ket qua ok = 0xF0 */
    nv11_response = ssp6_enable(sspC);
    if (nv11_response != SSP_RESPONSE_OK)
		{
			DEBUG_ERROR("NV11 >> Enable Failed %d\r\n", nv11_response);
			return ERROR_ENABLE;
		}
		else printf("NV11 >> Enable Successfully\r\n");
		HAL_Delay(1000); printf("\r\n");
		
    NV11_DisplayOn();
		return INIT_OK;
}

static void vdm_parse_poll(SSP_COMMAND *sspC, SSP_POLL_DATA6 *poll, bool lock)
{
	uint32_t i;
	int8_t poll_ret;
	for (i = 0; i < poll->event_count; ++i) /* Vong quet toan bo cac su kien tra ve co trong poll */
	{
		switch (poll->events[i].event) /* Kiem tra su kien thu i*/
		{
			/* Su kien Reset -> Xay ra khi thiet bi duoc cap nguon và da tra qua qua trinh reset */
			case SSP_POLL_RESET:
				DEBUG_INFO("NV11 >> Unit Reset\r\n");
				poll_ret = POLL_RESET;
				break;
			/* Su kien Read -> Xay ra khi dau doc dang doc tien giay */
			case SSP_POLL_READ:
				/* Neu xu kien doc co gia tri > 0 nghia la 1 to tien dang duoc doc va xac thuc */
				if (poll->events[i].data1)
				{
					/* In len man hinh kenh tien vua doc duoc (1 -> 6)*/
					DEBUG_INFO("NV11 >> Note Read %d %s\r\n", poll->events[i].data1, poll->events[i].cc);
					printf("Menh gia tien: %d %s \r\n",m_setup_req.ChannelData[poll->events[i].data1-1].value, m_setup_req.ChannelData[poll->events[i].data1-1].cc);
				}
				poll_ret = poll->events[i].data1;
				break;

			/* Su kien nay bao tien da duoc chuyen tu vi tri nhan tien sang vi tri ngan luu tru */
			case SSP_POLL_CREDIT:
				/* Dung dau doc tien */
				NV11_Disable(false);
				/* Tien da duoc chap nhan va chuyen vao ngan luu tru */
				DEBUG_INFO("NV11 >> Credit %d %s\r\n", poll->events[i].data1, poll->events[i].cc);
				break;

			/* Su kien nay say ra khi thiet bi mat dien luc dang trong qua trinh tra lai tien,
			   gia tri tra lai -> so tien can phai thanh toan not */
			case SSP_POLL_INCOMPLETE_PAYOUT:
				/* In ra man hinh debug so tien con phai tra lai */
				DEBUG_INFO("NV11 >> Incomplete payout %d of %d %s\r\n", poll->events[i].data1, poll->events[i].data2, poll->events[i].cc);
				break;

			/* Su kien nay say ra khi thiet bi mat dien luc chua thuc hien viec tra lai
			   gia tri tra lai -> So tien va su kien yeu cau*/
			case SSP_POLL_INCOMPLETE_FLOAT:
				/* In ra man hinh debug so tien va su kien yeu cau */
				DEBUG_WARN("NV11 >> Incomplete float %d of %d %s\r\n", poll->events[i].data1, poll->events[i].data2, poll->events[i].cc);
				break;

			/* Su kien dang trong qua trinh tra lai cho nguoi dung (vi tien bi tu choi) */
			case SSP_POLL_REJECTING:
				DEBUG_INFO("NV11 >> SSP_POLL_REJECTING\r\n");
				break;

			/* Su kien da hoan thanh viec tra lai tien cho nguoi tieu dung (vi tien bi tu choi) */
			case SSP_POLL_REJECTED:
				DEBUG_WARN("NV11 >> Note Rejected\r\n");
				break;

			/* Su kien thong bao tien dang duoc van chuyen den ngan xep */
			case SSP_POLL_STACKING:
				DEBUG_VERBOSE("NV11 >> Note SSP_POLL_STACKING\r\n");
				break;

			/* Su kien bao tien da duoc chuyen vao kho luu tru tra lai (NoteFloat) */
			case SSP_POLL_STORED:
				// The note has been stored in the payout unit
				DEBUG_INFO("NV11 >> Stored\r\n");

				/* Hien thi len man hinh debug tong so tien trong kho tra lai */
				m_num_note_payout = NV11_GetTotalCashInNoteFloat(false) / NV11_GetDenominationForChange(false);
				m_last_num_note_payout = m_num_note_payout;
				DEBUG_INFO("NV11 >> Stored Notes: %d\r\n", m_num_note_payout * NV11_GetDenominationForChange(false));
				break;

			/* Su kien co mot to tien di tu tren float note xuong stack note */
			case SSP_POLL_NOTE_TRANSFERRED_TO_STACK:
				DEBUG_INFO("NV11 >> Note Transferred to Stacker\r\n");
				/* Thuc hien tra lai them 1 to tien */
				NV11_PayoutNote(false);
				break;

			/* Su kien bao tien da duoc chuyen vao ngan xep chong (kho khong tra lai) */
			case SSP_POLL_STACKED:
				/* Hien thi tien dang trong ngan xep */
				DEBUG_INFO("NV11 >> Stacked\r\n");
				/* Nhan su kien tien chuyen vao ngan xep */
				m_last_n11_event = NV11_NOTE_STACKED;
				break;

			/* Khong co su kien nay */
			case SSP_POLL_SAFE_JAM:
				DEBUG_INFO("NV11 >> Safe Jam\r\n");
				break;

			/* Su kien to tien bi ket trong qua trinh van chuyen, gia tri tra ve vi tri bi ket */
			case SSP_POLL_UNSAFE_JAM:
				DEBUG_INFO("NV11 >> Unsafe Jam\r\n");
				break;

			/* Su kien tien bi ket */
			case SSP_POLL_JAMMED:
				DEBUG_INFO("NV11 >> Jammed %d\r\n", poll->events[i].data1);
				break;

			/* Su kien Poll da bi vo hieu hoa */
			case SSP_POLL_DISABLED:
				DEBUG_VERBOSE("NV11 has been DISABLED\r\n");
				//nv11_enable();
				break;
			/* Su kien phat hien mot no luc gian lan */
			case SSP_POLL_FRAUD_ATTEMPT:
				DEBUG_INFO("NV11 >> Fraud Attempt %d %s\r\n", poll->events[i].data1, poll->events[i].cc);
				m_last_n11_event = NV11_NOTE_FRAUD_ATTEMPT;
				break;

			/* Su kien bao ngan xep day */
			case SSP_POLL_STACKER_FULL:
				DEBUG_INFO("NV11 >> Stacker Full\r\n");
				break;

			/* Su kien bao hop tien bi thao */
			case SSP_POLL_CASH_BOX_REMOVED:
				DEBUG_INFO("NV11 >> Cashbox Removed\r\n");
				break;

				/* Su kien bao hop tien duoc thay the */
			case SSP_POLL_CASH_BOX_REPLACED:
				DEBUG_INFO("NV11 >> Cashbox Replaced\r\n");
				break;

				/* Su kien bao mot to tien da duoc dua vao luc thiet bi luc khoi dong, va da duoc dua ra khoi mat truoc cua may */
			case SSP_POLL_CLEARED_FROM_FRONT:
				DEBUG_INFO("NV11 >> Cleared from front %d\r\n", m_denominations[poll->events[i].data1 - 1]);
				/*if (*vdm_selling_task_get_hold_balance() != 0)
					*vdm_selling_task_get_hold_balance() -= m_denominations[poll->events[i].data1 - 1];*/ //cmt_by_duclq
				break;

			/* Su kien bao mot to tien da duoc dua vao luc thiet bi luc khoi dong, va da duoc dua ra khoi hop dung tien */
			case SSP_POLL_CLEARED_INTO_CASHBOX:
				DEBUG_INFO("NV11 >> Cleared Into Cashbox %d\r\n", m_denominations[poll->events[i].data1 - 1]);
				/*if (*vdm_selling_task_get_hold_balance() != 0)
					*vdm_selling_task_get_hold_balance() -= m_denominations[poll->events[i].data1 - 1];
				*vdm_selling_task_get_closing_balance() += m_denominations[poll->events[i].data1 - 1];*/ //cmt_by_duclq
				break;

			/* Su kien tra ve tong gia tri ma thiet bi da phan chia de phan hoi lai lenh phan chia (Dispended command) */
			case SSP_POLL_DISPENSED:
				/* tinh toan so tien con lai co trong dau tra lai */
				DEBUG_INFO("NV11 >> Note dispensed\r\n");
				nv11_enable(false);
				m_num_note_payout = NV11_GetTotalCashInNoteFloat(false) / NV11_GetDenominationForChange(false);
				DEBUG_INFO("NV11 >> Stored Notes: %d", m_num_note_payout * NV11_GetDenominationForChange(false));
				break;

			/* Su kien bao thiet bi da hoan thanh viec lam trong (empty) de phan hoi lenh lam trong (Empty Command) */
			case SSP_POLL_EMPTIED:
				break;

			/* Su kien tra ve khi thiet bi hieu chuan phan cung that bai (loi phan cung) */
			case SSP_POLL_CALIBRATION_FAIL:
				/* Du lieu tra ve bao gom cac ma loi co san */
				DEBUG_INFO("NV11 >> Calibration fail:\r\n");

				switch (poll->events[i].data1)
				{
					case NO_FAILUE: /* Khong co loi*/
						DEBUG_INFO("NV11 >> No failure\r\n");
					break;
					case SENSOR_FLAP: /* Loi cam bien nap thanh toan */
						DEBUG_ERROR("NV11 >> Optical sensor flap\r\n");
					break;
					case SENSOR_EXIT: /* Loi cam bien Exit */
						DEBUG_ERROR("NV11 >> Optical sensor exit\r\n");
					break;
					case SENSOR_COIL1: /* Loi cam bien cuon 1 */
						DEBUG_ERROR("NV11 >> Coil sensor 1\r\n");
					break;
					case SENSOR_COIL2: /* Loi cam bien cuon 2 */
						DEBUG_ERROR("NV11 >> Coil sensor 2\r\n");
					break;
					case NOT_INITIALISED: /* Loi thiet bi khong duoc khoi tao */
						DEBUG_INFO("NV11 >> Unit not initialised\r\n");
					break;
					case CHECKSUM_ERROR: /* Loi checksum */
						DEBUG_ERROR("NV11 >> Data checksum error\r\n");
					break;
					case COMMAND_RECAL: /* Hieu chinh lai bang lenh yeu cau (loi thoi)*/
						DEBUG_ERROR("NV11 >> Recalibration by command required\r\n");
						ssp6_run_calibration(sspC);
						break;
					default:
						break;
				}
				break;

			/* Su kien nay say ra khi MCU yeu cau tam dung thiet bi, chu trinh tra lai se tu dong huy -> bao loi qua trinh tra lai */
			/* Su dung cho dau doc NV200 */
			case SSP_POLL_HALTED: //Halted
				DEBUG_INFO("NV11 >> Stored Notes: %d\r\n", NV11_GetTotalCashInNoteFloat(lock));
				m_last_n11_event = NV200_PAYOUT_FAILED; /* Thiet lap su kien NV200 thanh toan loi */
				break;

			/* Doi voi cac su kien con lai, in ra ma hinh debug */
			default:
                // DEBUG_VERBOSE("NV11 >> Poll Event: 0x%x\r\n", poll->events[i].event);       
				break;
        }
    }
}

int8_t vdm_NV11_Process(uint32_t poll_time)
{
	SSP_RESPONSE_ENUM m_rsp_status;
	SSP_POLL_DATA6 m_poll;
	static uint32_t m_old_tick = 0;
	
	uint32_t now = HAL_GetTick();//cmt_by_duclq
  if (now - m_old_tick > (uint32_t)poll_time)
  {
		m_old_tick = now;
		/* Thuc hien ham doc cac su kien trong NV11, neu phan hoi ve khong phai la 0xF0 */
			if ((m_rsp_status = ssp6_poll(&m_ssp_cmd, &m_poll)) != SSP_RESPONSE_OK)
			{
				if (m_rsp_status == SSP_RESPONSE_TIMEOUT) /* Neu phan hoi su kien ve la SSP_RESPONSE_TIMEOUT */
				{
						// If the poll timed out, then give up
						DEBUG_ERROR("NV11 >> SSP Poll Timeout\r\n"); /* Bao len man hinh debug */
					return ERR_POLL_TIMEOUT;
				}
				else
				{
					if (m_rsp_status == SSP_RESPONSE_KEY_NOT_SET) /* Neu phai hoi su kien mot khoa ma hoa khong duoc cai dat*/
					{
							// The validator has responded with key not set, so we should try to negotiate one
							if (ssp6_setup_encryption(&m_ssp_cmd, (unsigned long long)0x123456701234567LL) != SSP_RESPONSE_OK) /* Thuc hien lai viec cai dat mot khoa ma hoa defaul */
							{
									DEBUG_ERROR("NV11 >> Encryption Failed\r\n"); /* Cai dat loi, bao len man hinh debug */
								return ERR_SET_KEY;
							}
							else /* Cai dat khoa ma hoa thanh cong */
							{
									DEBUG_INFO("NV11 >> Encryption Setup success\r\n"); /* Bao hoan thanh cai dat */
							}
					}
					else
					{
							DEBUG_INFO("NV11 >> Unexpected error %02X\r\n", m_rsp_status); /* Bao hoan thanh cai dat */
						return ERR_UNKNOWN;
					}
				}
			}
			/* Thuc hien ham doc cac su kien trong NV11, neu phan hoi ve la 0xF0 -> phan tich cac su kien */
			else
			{
				printf("parse_poll:\r\n");
				vdm_parse_poll(&m_ssp_cmd, &m_poll, false); /* Thuc hien phan tich cac su kien NV11 */
			}
	}
	
	return POLL_IDLE;
}

