#include "ssp_helpers.h"
//#include "platform.h"
//#include "fsl_debug_console.h"

#ifdef WIN32
#include "port_win32.h"
#include "port_win32_ssp.h"
#else
#include "SSPComs.h"
#endif

int32_t send_ssp_command(SSP_COMMAND *sspC)
{
    return SSPSendCommand(sspC->PortNumber, sspC);
}

int32_t negotiate_ssp_encryption(SSP_COMMAND *sspC, SSP_FULL_KEY *hostKey)
{
    return NegotiateSSPEncryption(sspC->PortNumber, sspC->SSPAddress, hostKey);
}

static SSP_RESPONSE_ENUM _ssp_return_values(SSP_COMMAND *sspC)
{
    //CHECK FOR TIMEOUT
    if (send_ssp_command(sspC) == 0)
        return SSP_RESPONSE_TIMEOUT;

    // extract the validators response code
    return (SSP_RESPONSE_ENUM)sspC->ResponseData[0];
}

/*
 * Helper functions to send a simple command
 */

// Send an SSP payout command (0x33)
SSP_RESPONSE_ENUM ssp6_payout(SSP_COMMAND *sspC, const int32_t value, const char *cc, const char option)
{
    SSP_RESPONSE_ENUM resp;
    int32_t i;

    sspC->CommandDataLength = 9;
    sspC->CommandData[0] = SSP_CMD_PAYOUT_VALUE;

    for (i = 0; i < 4; i++)
        sspC->CommandData[i + 1] = value >> (i * 8);

    for (i = 0; i < 3; i++)
        sspC->CommandData[i + 5] = cc[i];

    sspC->CommandData[8] = option;

    resp = _ssp_return_values(sspC);
    return resp;
}

// Send an SSP payout command (0x3B)
SSP_RESPONSE_ENUM ssp6_set_route(SSP_COMMAND *sspC, const int32_t value, const char *cc, const char route)
{
    SSP_RESPONSE_ENUM resp;
    int32_t i;

    sspC->CommandDataLength = 9;
    sspC->CommandData[0] = SSP_CMD_SET_ROUTING;
    sspC->CommandData[1] = route;

    for (i = 0; i < 4; i++)
        sspC->CommandData[i + 2] = value >> (i * 8);

    for (i = 0; i < 3; i++)
        sspC->CommandData[i + 6] = cc[i];

    resp = _ssp_return_values(sspC);
    return resp;
}

// Send an SSP sync (0x11)
SSP_RESPONSE_ENUM ssp6_sync(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_SYNC;
    resp = _ssp_return_values(sspC);
    return resp;
}

// Setup SSP encryption, sends SSP commands set generator (0x4A), set modulus (0x4B) and exchange keys (0x4C)
SSP_RESPONSE_ENUM ssp6_setup_encryption(SSP_COMMAND *sspC, const long long fixedkey)
{
    sspC->Key.FixedKey = fixedkey;
    if (negotiate_ssp_encryption(sspC, &(sspC->Key)) == 0)
        return SSP_RESPONSE_TIMEOUT;
    sspC->EncryptionStatus = 1;
    return SSP_RESPONSE_OK;
}

// Send an SSP host protocol version (0x06)
SSP_RESPONSE_ENUM ssp6_host_protocol(SSP_COMMAND *sspC, const uint8_t host_protocol)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 2;
    sspC->CommandData[0] = SSP_CMD_HOST_PROTOCOL;
    sspC->CommandData[1] = host_protocol;
    resp = _ssp_return_values(sspC);
    return resp;
}

// Send an SSP setup request (0x05), and parse the response into an SSP6_SETUP_REQUEST_DATA
SSP_RESPONSE_ENUM ssp6_setup_request(SSP_COMMAND *sspC, SSP6_SETUP_REQUEST_DATA *setup_request_data)
{
    SSP_RESPONSE_ENUM resp;
    uint32_t i;
    int32_t offset;

    // Send the setup request command
    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_SETUP_REQUEST;
    resp = _ssp_return_values(sspC);

    // If the command succeeded parse the response
    if (resp == SSP_RESPONSE_OK)
    {
        offset = 1;
        // SSP unit type
        setup_request_data->UnitType = sspC->ResponseData[offset++];

        if (setup_request_data->UnitType == 0x03)
        {
            // If the unit is a SMART Hopper

            // Extract the firmware version
            for (i = 0; i < 4; ++i)
                setup_request_data->FirmwareVersion[i] = sspC->ResponseData[offset++];
            setup_request_data->FirmwareVersion[i] = '\n'; //NULL TERMINATOR

            // Country Code field is obsolete in SSPv6
            offset += 3;

            // Extract the SSP protocol version and the number of channels
            setup_request_data->ProtocolVersion = sspC->ResponseData[offset++];
            setup_request_data->NumberOfChannels = sspC->ResponseData[offset++];

            // For each channel, extract the channel value
            for (i = 0; i < setup_request_data->NumberOfChannels; i++)
            {
                int32_t j;
                setup_request_data->ChannelData[i].value = 0;
                for (j = 0; j < 2; j++)
                    setup_request_data->ChannelData[i].value += sspC->ResponseData[offset++] << (j * 8);
            }
            // For each channel, extract the country code
            for (i = 0; i < setup_request_data->NumberOfChannels; i++)
            {
                int32_t j;
                for (j = 0; j < 3; j++)
                    setup_request_data->ChannelData[i].cc[j] = sspC->ResponseData[offset++];

                setup_request_data->ChannelData[i].cc[j] = '\0'; // NULL TERMINATOR
            }
        }
        else
        {

            // Extract the firmware version
            for (i = 0; i < 4; ++i)
                setup_request_data->FirmwareVersion[i] = sspC->ResponseData[offset++];
            setup_request_data->FirmwareVersion[i] = '\0'; //NULL TERMINATOR

            // Country Code field is obsolete in SSPv6
            offset += 3;
            // Value multiplier field is obsolete in SSPv6
            offset += 3;

            // Extract the number of channels
            setup_request_data->NumberOfChannels = sspC->ResponseData[offset++];

            // The channel values can be ignoredhere
            offset += setup_request_data->NumberOfChannels; // skip past channel values for ssp < v6

            for (i = 0; i < setup_request_data->NumberOfChannels; i++)
            {
                setup_request_data->ChannelData[i].security = sspC->ResponseData[offset++];
            }

            // Extract the multiplier
            setup_request_data->RealValueMultiplier = 0;
            for (i = 0; i < 3; ++i)
                setup_request_data->RealValueMultiplier += ((uint32_t)sspC->ResponseData[offset++] << ((2 - i) * 8));

            // The protocol version
            setup_request_data->ProtocolVersion = sspC->ResponseData[offset++];

            // for each channel extract the country code
            for (i = 0; i < setup_request_data->NumberOfChannels; i++)
            {
                int32_t j;
                for (j = 0; j < 3; j++)
                    setup_request_data->ChannelData[i].cc[j] = sspC->ResponseData[offset++];

                setup_request_data->ChannelData[i].cc[j] = '\0'; // NULL termination
            }

            // for each channel extract the value
            for (i = 0; i < setup_request_data->NumberOfChannels; i++)
            {
                int32_t j;
                setup_request_data->ChannelData[i].value = 0;
                for (j = 0; j < 4; j++)
                    setup_request_data->ChannelData[i].value += sspC->ResponseData[offset++] << (j * 8);
            }
        }
    }
    return resp;
}

// send an enable command
SSP_RESPONSE_ENUM ssp6_enable(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_ENABLE;
    resp = _ssp_return_values(sspC);
    return resp;
}
// send an enable payout command
SSP_RESPONSE_ENUM ssp6_enable_payout(SSP_COMMAND *sspC, const char type)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_ENABLE_PAYOUT_DEVICE;

    if (type == 0x07)
    {
        sspC->CommandDataLength = 2;
        sspC->CommandData[1] = 0x00;
    }

    resp = _ssp_return_values(sspC);
    return resp;
}

// send a set inhibits command
SSP_RESPONSE_ENUM ssp6_set_inhibits(SSP_COMMAND *sspC, const uint8_t lowchannels, const uint8_t highchannels)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 3;
    sspC->CommandData[0] = SSP_CMD_SET_INHIBITS;
    sspC->CommandData[1] = lowchannels;
    sspC->CommandData[2] = highchannels;
    resp = _ssp_return_values(sspC);
    return resp;
}

// poll the validator, and extract the responses.
SSP_RESPONSE_ENUM ssp6_poll(SSP_COMMAND *sspC, SSP_POLL_DATA6 *poll_response)
{
    SSP_RESPONSE_ENUM resp;
    uint8_t i, j;

    // send the poll command
    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_POLL;
    resp = _ssp_return_values(sspC);

    if (resp == SSP_RESPONSE_OK)
    {
        // if the poll was successful, iterate over all of the response
        poll_response->event_count = 0;
        for (i = 1; i < sspC->ResponseDataLength; ++i)
        {
            // initialise the event structure
            poll_response->events[poll_response->event_count].event = sspC->ResponseData[i];
            poll_response->events[poll_response->event_count].data1 = 0;
            poll_response->events[poll_response->event_count].data2 = 0;
            poll_response->events[poll_response->event_count].cc[0] = 0;
            poll_response->events[poll_response->event_count].cc[3] = 0;

            switch (sspC->ResponseData[i])
            {
            //all these commands have one data byte
            case SSP_POLL_CREDIT:
            case SSP_POLL_READ:
            case SSP_POLL_FRAUD_ATTEMPT:
            case SSP_POLL_CLEARED_FROM_FRONT:
            case SSP_POLL_CLEARED_INTO_CASHBOX:
            case SSP_POLL_CALIBRATION_FAIL:
                i++; //move onto the data
                poll_response->events[poll_response->event_count].data1 = sspC->ResponseData[i];
                break;

            //all these commands have 7 data bytes per country;
            case SSP_POLL_DISPENSING:
            case SSP_POLL_DISPENSED:
            case SSP_POLL_JAMMED:
            case SSP_POLL_HALTED:
            case SSP_POLL_NOTE_TRANSFERRED_TO_STACK:
            case SSP_POLL_PAID_STACK_POWER_UP:
            case SSP_POLL_PAID_STORE_POWER_UP:
            case SSP_POLL_PAID_DISPENED_POWER_UP:
            case SSP_POLL_BEZEL_HOLD:

            // NV200
            case SSP_POLL_FLOATING:
            case SSP_POLL_FLOATED:
            case SSP_POLL_TIMEOUT:
            case SSP_POLL_CASHBOX_PAID:
            case SSP_POLL_COIN_CREDIT:
            case SSP_POLL_SMART_EMPTYING:
            case SSP_POLL_SMART_EMPTIED:
            {
                uint8_t event = sspC->ResponseData[i];
                uint32_t countries;
                i++; // move onto the country count;
                countries = (unsigned int)sspC->ResponseData[i];
                // for every country in the response, make a new event structure and store into it
                for (j = 0; j < countries; ++j)
                {
                    int32_t k;
                    poll_response->events[poll_response->event_count].event = event;
                    poll_response->events[poll_response->event_count].data1 = 0;
                    poll_response->events[poll_response->event_count].data2 = 0;
                    poll_response->events[poll_response->event_count].cc[3] = '\0';

                    for (k = 0; k < 4; ++k)
                    {
                        i++; //move through the 4 bytes of data
                        poll_response->events[poll_response->event_count].data1 += (((uint32_t)sspC->ResponseData[i]) << (8 * k));
                    }
                    for (k = 0; k < 4; ++k)
                    {
                        i++; //move through the 3 bytes of country code
                        poll_response->events[poll_response->event_count].cc[k] += sspC->ResponseData[i];
                    }

                    if (j != countries - 1) // the last time through event_count will be updated elsewhere.
                        poll_response->event_count++;
                }
            }
            break;

            //all these commands have 11 data bytes per country;
            case SSP_POLL_INCOMPLETE_PAYOUT:
            case SSP_POLL_INCOMPLETE_FLOAT:
            {
                uint32_t countries;
                uint8_t event = sspC->ResponseData[i];
                i++; // move onto the country count;
                countries = (uint32_t)sspC->ResponseData[i];
                // for every country in the response, make a new event structure and store into it
                for (j = 0; j < countries; ++j)
                {
                    int32_t k;
                    poll_response->events[poll_response->event_count].event = event;
                    poll_response->events[poll_response->event_count].data1 = 0;
                    poll_response->events[poll_response->event_count].data2 = 0;
                    poll_response->events[poll_response->event_count].cc[3] = '\0';

                    for (k = 0; k < 4; ++k)
                    {
                        i++; //move through the 4 bytes of data
                        poll_response->events[poll_response->event_count].data1 += (((uint32_t)sspC->ResponseData[i]) << (8 * k));
                    }
                    for (k = 0; k < 4; ++k)
                    {
                        i++; //move through the 4 bytes of data
                        poll_response->events[poll_response->event_count].data2 += (((uint32_t)sspC->ResponseData[i]) << (8 * k));
                    }
                    for (k = 0; k < 3; ++k)
                    {
                        i++; //move through the 3 bytes of country code
                        poll_response->events[poll_response->event_count].cc[k] += sspC->ResponseData[i];
                    }

                    if (j != countries - 1) // the last time through event_count will be updated elsewhere.
                        poll_response->event_count++;
                }
            }
            break;
            default: //every other command has no data bytes
                poll_response->events[poll_response->event_count].data1 = 0;
                poll_response->events[poll_response->event_count].data2 = 0;
                poll_response->events[poll_response->event_count].cc[0] = '\0';
                break;
            }
            poll_response->event_count++;
        }
    }
    return resp;
}

// reset the validator
SSP_RESPONSE_ENUM ssp6_reset(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_RESET;
    resp = _ssp_return_values(sspC);
    return resp;
}

// disable the payout unit
SSP_RESPONSE_ENUM ssp6_disable_payout(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_DISABLE_PAYOUT_DEVICE;

    resp = _ssp_return_values(sspC);
    return resp;
}

// disable the validator
SSP_RESPONSE_ENUM ssp6_disable(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_DISABLE;
    resp = _ssp_return_values(sspC);
    return resp;
}

// payout the next note from an NV11
SSP_RESPONSE_ENUM ssp6_payout_note(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_PAYOUT_NOTE;

    resp = _ssp_return_values(sspC);
    return resp;
}

// stack the next note in the NV11
SSP_RESPONSE_ENUM ssp6_stack_note(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_STACK_NOTE;

    resp = _ssp_return_values(sspC);
    return resp;
}

// run a calibration sequence on the hopper
SSP_RESPONSE_ENUM ssp6_run_calibration(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = SSP_CMD_RUN_CALIBRATION;
    resp = _ssp_return_values(sspC);
    return resp;
}

// set the inhibits for the atached coinmech
SSP_RESPONSE_ENUM ssp6_set_coinmech_inhibits(SSP_COMMAND *sspC, uint32_t value, const char *cc, enum channel_state state)
{
    SSP_RESPONSE_ENUM resp;
    int32_t i;

    sspC->CommandDataLength = 7;
    sspC->CommandData[0] = SSP_CMD_SET_COINMECH_INHIBITS;
    sspC->CommandData[1] = state;
    sspC->CommandData[2] = value & 0xFF;
    sspC->CommandData[3] = (value >> 8) & 0xFF;

    for (i = 0; i < 3; i++)
        sspC->CommandData[4 + i] = cc[i];

    resp = _ssp_return_values(sspC);
    return resp;
}

SSP_RESPONSE_ENUM ssp6_set_reporting_type(SSP_COMMAND *sspC, uint8_t byChannel)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 2;
    sspC->CommandData[0] = 0x45;

    if (byChannel == 0)
    {
        sspC->CommandData[1] = 0x00; // report by 4 bit value
    }
    else
    {
        sspC->CommandData[1] = 0x01; // report by channel number
    }

    resp = _ssp_return_values(sspC);
    return resp;
}

int32_t ssp6_get_stored_notes(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;
    int32_t counter = 0, i = 0;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = 0x41;

    resp = _ssp_return_values(sspC);

    if (resp == SSP_RESPONSE_OK)
    {
        for (i = (sspC->ResponseData[1] * 4) + 1; i >= 2; i -= 4)
        {
            counter++;
        }
    }

    return counter;
}

SSP_RESPONSE_ENUM ssp6_empty_notes(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = 0x3F;

    resp = _ssp_return_values(sspC);
    return resp;
}

SSP_RESPONSE_ENUM ssp6_reject(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = 0x08;

    resp = _ssp_return_values(sspC);
    return resp;
}

SSP_RESPONSE_ENUM ssp6_hold(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = 0x18;

    resp = _ssp_return_values(sspC);
    return resp;
}

// This function uses the command GET NOTE AMOUNT to find out the number of
// a specified type of note stored in the payout. Returns the number of notes stored
// of that denomination.
int32_t ssp6_check_note_level(SSP_COMMAND *sspC, int note)
{
    SSP_RESPONSE_ENUM resp;

    note *= 100;
retry:
    sspC->CommandData[0] = SSP_CMD_GET_NOTE_AMOUNT;
    sspC->CommandData[4] = (uint8_t)((note >> 24));
    sspC->CommandData[3] = (uint8_t)((note >> 16));
    sspC->CommandData[2] = (uint8_t)((note >> 8));
    sspC->CommandData[1] = (uint8_t)(note);

    sspC->CommandData[5] = 'V';
    sspC->CommandData[6] = 'N';
    sspC->CommandData[7] = 'D';
    sspC->CommandDataLength = 8;

    resp = _ssp_return_values(sspC);
    if (resp == SSP_RESPONSE_OK)
    {
        int32_t i = (int32_t)sspC->ResponseData[1];
        return i;
    }
    goto retry;
}

SSP_RESPONSE_ENUM ssp6_display_on(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = 0x03;

    resp = _ssp_return_values(sspC);
    return resp;
}

SSP_RESPONSE_ENUM ssp6_display_off(SSP_COMMAND *sspC)
{
    SSP_RESPONSE_ENUM resp;

    sspC->CommandDataLength = 1;
    sspC->CommandData[0] = 0x04;

    resp = _ssp_return_values(sspC);
    return resp;
}
