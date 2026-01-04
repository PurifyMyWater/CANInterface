#include "CANInterface.h"

#include <cinttypes>
#include <cstdio>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const char* toString(const N_TAtype_t nTAtype)
{
    switch (nTAtype)
    {
        case N_TATYPE_5_CAN_CLASSIC_29bit_Physical:
            return "N_TATYPE_5_CAN_CLASSIC_29bit_Physical";
        case N_TATYPE_6_CAN_CLASSIC_29bit_Functional:
            return "N_TATYPE_6_CAN_CLASSIC_29bit_Functional";
        default:
            return "UNKNOWN_N_TATYPE";
    }
}

const char* toString(const N_AI& nAi)
{
    static char buffer[MAX_N_AI_STR_SIZE]; // 72 = 40 (N_TAtype) + 3 (N_SA) + 3 (N_TA) + 25 (for the format string) + 1
    // (for the null terminator)

    snprintf(buffer, sizeof(buffer), "{N_SA=%" PRIu8 ", N_TA=%" PRIu8 ", N_TAtype=%s}", nAi.N_SA, nAi.N_TA,
             toString(nAi.N_TAtype));

    return buffer;
}

const char* toString(const uint8_t* data, const uint8_t data_length_code)
{
    /* Allocate twice the number of bytes in the "buf" array because each byte would
     * be converted to two hex characters, also add an extra space for the terminating
     * null byte.
     */
    static char output[(CAN_FRAME_MAX_DLC * 2) + 1];
    const char  size = MIN(data_length_code, CAN_FRAME_MAX_DLC);

    char* ptr = &output[0];

    for (int i = 0; i < size; i++)
    {
        ptr += sprintf(ptr, "%02" PRIX8, data[i]);
    }

    return output;
}

const char* toString(const CANFrame& frame)
{
    static char buffer[MAX_FRAME_STR_SIZE]; // 181 = 72 (N_AI) + 5 (flags) + 1 (data_length_code) + 17 (data) + 74
    // (format string) + 1 (null terminator)

    snprintf(buffer, sizeof(buffer),
             "{N_AI=%s, flags={ide=%" PRIu8 ", rtr=%" PRIu8 ", fdf=%" PRIu8 ", brs=%" PRIu8 ", esi=%" PRIu8
             "}, dlc=%" PRIu16 ", timestamp=%" PRIu64 ", data=[0x%s]}",
             toString(frame.id), frame.ide, frame.rtr, frame.fdf, frame.brs, frame.esi, frame.dlc, frame.timestamp,
             toString(frame.data, frame.dlc));

    return buffer;
}

const char* toString(const CANTXErrorFlags& flags)
{
    static char buffer[MAX_TX_ERROR_FLAGS_STR_SIZE]; // 54 (format string) + 5 (flags) + 1 (null terminator)

    snprintf(buffer, sizeof(buffer),
             "{arb_lost=%" PRIu8 ", bit_err=%" PRIu8 ", form_err=%" PRIu8 ", stuff_err=%" PRIu8 ", ack_err=%" PRIu8 "}",
             flags.arb_lost, flags.bit_err, flags.form_err, flags.stuff_err, flags.ack_err);

    return buffer;
}

const char* toString(CANErrorState state)
{
    switch (state)
    {
        case CAN_ERROR_ACTIVE:
            return "CAN_ERROR_ACTIVE";
        case CAN_ERROR_WARNING:
            return "CAN_ERROR_WARNING";
        case CAN_ERROR_PASSIVE:
            return "CAN_ERROR_PASSIVE";
        case CAN_ERROR_BUS_OFF:
            return "CAN_ERROR_BUS_OFF";
        default:
            return "UNKNOWN_CAN_ERROR_STATE";
    }
}

const char* toString(const CANTXDoneEvent& event)
{
    static char buffer[MAX_FRAME_STR_SIZE + MAX_TX_ERROR_FLAGS_STR_SIZE +
                       37]; // 36 = length of format string + 1 null terminator

    snprintf(buffer, sizeof(buffer), "CANTXDoneEvent={errorFlags=%s, frame=%s}", toString(event.errorFlags),
             toString(event.frame));
    return buffer;
}

const char* toString(const CANRXDoneEvent& event)
{
    static char buffer[MAX_FRAME_STR_SIZE + 24]; // 23 = length of format string + 1 null terminator

    snprintf(buffer, sizeof(buffer), "CANRXDoneEvent={frame=%s}", toString(event.frame));
    return buffer;
}

const char* toString(const CANStateChangedEvent& event)
{
    static char
        buffer[(2 * sizeof("UNKNOWN_CAN_ERROR_STATE")) + 44]; // 423= length of format string + 1 null terminator
    snprintf(buffer, sizeof(buffer), "CANStateChangedEvent={oldState=%s, newState=%s}", toString(event.oldState),
             toString(event.newState));
    return buffer;
}

const char* toString(const std::monostate&)
{
    return "{No Event}";
}

const char* toString(const CANEvent& event)
{
    return std::visit([](const auto& e) { return toString(e); }, event);
}
