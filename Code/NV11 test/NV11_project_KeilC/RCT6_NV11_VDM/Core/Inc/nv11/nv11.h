
#ifndef NV11_DRIVER_H
#define NV11_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define NV_TYPE_NV9         0x00
#define NV_TYPE_NV11        0x07
#define NV_TYPE_NV200       0x06
#define NOTE_NUM_PAYOUT     25

#define NV11_EVENT_ID_DISABLE_NV11		                            0x01
#define NV11_EVENT_ID_RESET_CASH                                    0x02
#define NV11_EVENT_ID_GET_TOTAL_CASH_IN_NOTE_FLOAT                  0x03
#define NV11_EVENT_ID_PROCESS                                       0x04
#define NV11_EVENT_ID_GET_NUMBER_OF_FLOATNOTE_RETURN_CUSTOMER       0x05
#define NV11_EVENT_ID_SET_ACCEPT_CASH                               0x06

//NV11 return
#define ERROR_SYNC 1
#define ERROR_HOST_PROT 2
#define ERROR_SETUP_REQUEST 3
#define ERROR_SET_INHIBITS 4
#define ERROR_ENABLE 5
#define ERROR_KEY_NOT_SET 6
#define INIT_OK -1

#define POLL_IDLE -1
#define ERR_POLL_TIMEOUT -2
#define ERR_SET_KEY -3
#define ERR_UNKNOWN -4
#define POLL_RESET -5

typedef enum
{
    NV11_EVENT_NONE,
    NV11_ERROR,
    NV11_NOTE_READ,
    NV11_NOTE_STACKED,
    NV11_NOTE_STORED,
    NV11_NOTE_REJECTED,
    NV11_NOTE_DISPENSED,
    NV11_NOTE_FRAUD_ATTEMPT,
    NV200_PAYOUT_FAILED,
} nv11_event_t;

typedef void (*nv11_callback_t)(void *data, uint32_t len, bool need_free);


typedef struct
{
	uint8_t id;
	uint8_t need_free;
	uint32_t length;
	uint8_t *data;
    nv11_callback_t callback;
    void *callback_arg;
} nv11_msq_t;

/**
 * @brief		Initialize NV11
 * @retval		TRUE NV11 init success
 * 				Otherwise NV11 init failed
 */
bool NV11_Init(void);
//void nv11_init_mutex(void);//duclq_cmt
bool nv11_enable(bool lock);
int NV11_GetUnitType(bool lock);
int NV11_Disable(bool lock);
bool NV11_SetAcceptNote(uint8_t channel, bool lock);
int32_t NV11_EnablePayout(bool lock);
int NV11_DisablePayout(bool lock);
int NV11_SetDenominationForChange(uint32_t denomination, bool lock);
//int NV11_RoutingPayoutForNV200(bool lock);//duclq_cmt
int NV11_RoutingAllToCashbox(void);
int32_t NV11_Hold(bool lock);
void NV11_UnHold(bool lock);
int NV11_Reject(bool lock);
int32_t NV11_EmptyNotes(bool lock);
int NV11_DisplayOn(void);
int NV11_DisplayOff(void);

int NV11_GetChannelNumber(void);
int NV11_GetChannelValue(int channel);
int NV11_GetDenominationForChange(bool lock);
int NV11_GetTotalCashInNoteFloat(bool lock);
int NV11_GetStoredNoteByChannel(int channel);
bool NV11_GetHoldingCash(uint32_t *result, bool lock);
int32_t NV11_GetAvailableChange(bool lock);
nv11_event_t NV11_GetLatestEvent(bool lock);
void NV11_ClearLatestEvent(bool lock);
int NV11_GetLatestNote(bool lock);
void NV11_Process(bool lock);

int8_t vdm_NV11_Process(uint32_t poll_time);
void test_nv11(void);
int8_t vdm_NV11_Init(void);

/* This is the blocking function */
int32_t NV11_PayoutNote(bool lock);
int32_t NV11_PayoutValue(int32_t value, bool lock);
bool NV200_ValueIsOnForPayout(int value, bool lock);
bool NV11_GetStatusPayoutNote(bool lock);
typedef void (*nv11_reconnect_event_callback_t)(bool connected);
void nv11_register_reconnect_event(nv11_reconnect_event_callback_t callback);
bool nv11_is_availble(bool lock);
bool nv11_reset(bool lock);

#endif /* NV11_DRIVER_H */
