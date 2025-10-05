#include "CANInterface.h"

#include <cinttypes>
#include <cstdio>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const char* N_TAtypeToString(const N_TAtype_t nTAtype)
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

const char* nAiToString(const N_AI& nAi)
{
    static char buffer[MAX_N_AI_STR_SIZE]; // 72 = 40 (N_TAtype) + 3 (N_SA) + 3 (N_TA) + 25 (for the format string) + 1
    // (for the null terminator)

    snprintf(buffer, sizeof(buffer), "{N_SA=%" PRIu8 ", N_TA=%" PRIu8 ", N_TAtype=%s}", nAi.N_SA, nAi.N_TA,
             N_TAtypeToString(nAi.N_TAtype));

    return buffer;
}

const char* frameDataToString(const uint8_t* data, const uint8_t data_length_code)
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

const char* frameToString(const CANFrame& frame)
{
    static char buffer[MAX_FRAME_STR_SIZE]; // 181 = 72 (N_AI) + 5 (flags) + 1 (data_length_code) + 17 (data) + 85
    // (format string) + 1 (null terminator)

    snprintf(buffer, sizeof(buffer),
             "{N_AI=%s, flags={ide=%" PRIu8 ", rtr=%" PRIu8 ", fdf=%" PRIu8 ", brs=%" PRIu8 ", esi=%" PRIu8
             "}, dlc=%" PRIu16 ", timestamp=%" PRIu64 ", data=[0x%s]}",
             nAiToString(frame.id), frame.ide, frame.rtr, frame.fdf, frame.brs, frame.esi, frame.dlc, frame.timestamp,
             frameDataToString(frame.data, frame.dlc));

    return buffer;
}

const char* ackResultToString(const ACKResult& ackResult)
{
    switch (ackResult)
    {
        case ACK_SUCCESS:
            return "ACK_SUCCESS";
        case ACK_ERROR:
            return "ACK_ERROR";
        case ACK_NONE:
            return "ACK_NONE";
        default:
            return "UNKNOWN_ACK_RESULT";
    }
}
