#ifndef CANInterface_h
#define CANInterface_h

#include <cstdint>

constexpr uint8_t  CAN_FRAME_MAX_DLC = 8;
constexpr uint32_t MAX_N_AI_STR_SIZE = 72;
// 72 = 40 (N_TAtype) + 3 (N_SA) + 3 (N_TA) + 25 (for the format string) + 1 (for the null terminator)
constexpr uint32_t MAX_FRAME_STR_SIZE = 181;
// 181 = 72 (N_AI) + 5 (flags) + 1 (data_length_code) + 17 (data) + 85 (format string) + 1 (null terminator)

using N_TAtype_t = enum N_TAtype {
    CAN_UNKNOWN                             = 0,
    N_TATYPE_5_CAN_CLASSIC_29bit_Physical   = 218,
    N_TATYPE_6_CAN_CLASSIC_29bit_Functional = 219
};

using ACKResult = enum ACKResult { ACK_SUCCESS, ACK_ERROR, ACK_NONE };

constexpr uint8_t N_NFA_Header_Value  = 0b110;
constexpr uint8_t N_NFA_Padding_Value = 0b00;

using N_AI = union N_AI_union
{
    struct __attribute__((packed))
    {
        uint8_t : 3, N_NFA_Header : 3 {N_NFA_Header_Value}, N_NFA_Padding : 2 {N_NFA_Padding_Value};
        N_TAtype_t N_TAtype : 8 {CAN_UNKNOWN};
        uint8_t    N_TA{0};
        uint8_t    N_SA{0};
    };

    uint32_t N_AI;
};

using CANFrame = struct CANFrame
{
    N_AI     id;  /**< message arbitration identification */
    uint16_t dlc; /**< message data length code */
    struct __attribute__((packed))
    {
        uint32_t ide : 1; /**< Extended Frame Format (29bit ID) */
        uint32_t rtr : 1; /**< Message is a Remote Frame */
        uint32_t fdf : 1; /**< Message is FD format, allow max 64 byte of data */
        uint32_t brs : 1; /**< Transmit message with Bit Rate Shift. */
        uint32_t esi : 1; /**< Transmit side error indicator for received frame */
    };

    union
    {
        uint64_t timestamp;    /**< Timestamp for received message */
        uint64_t trigger_time; /**< Trigger time for transmitting message*/
    };

    uint8_t data[CAN_FRAME_MAX_DLC]{}; /**< Data bytes (not relevant in RTR frame) */
};

/**
 * @brief Convert N_TAtype to string.
 * @param nTAtype The N_TAtype to convert.
 * @return String representation of the N_TAtype.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* N_TAtypeToString(N_TAtype_t nTAtype);

/**
 * @brief Convert N_AI to string.
 * @param nAi The N_AI to convert.
 * @return String representation of the N_AI.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* nAiToString(const N_AI& nAi);

/**
 * @brief Convert frame data to string.
 * @param data Pointer to the data bytes.
 * @param data_length_code The length of the data in bytes.
 * @return String representation of the frame data.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* frameDataToString(const uint8_t* data, uint8_t data_length_code);

/**
 * @brief Convert CANFrame to string.
 * @param frame The CANFrame to convert.
 * @return String representation of the CANFrame.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* frameToString(const CANFrame& frame);

/**
 * @brief Convert ACKResult to string.
 * @param ackResult The ACKResult to convert.
 * @return String representation of the ACKResult.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* ackResultToString(const ACKResult& ackResult);

/**
 * @brief Interface for a CAN bus driver.
 */
class CANInterface
{
public:
    /**
     * @brief Check if a frame is available to read.
     * @return Number of frames available to read. or 0 if no frames are available, or the bus is not active.
     */
    virtual uint32_t frameAvailable() = 0;

    /**
     * @brief Read a frame from the CAN bus.
     * @param frame Pointer to a CANFrame struct to store the read frame.
     * @return True if a frame was read, false if no frame was available, or the bus is not active.
     */
    virtual bool readFrame(CANFrame* frame) = 0;

    /**
     * @brief Write a frame to the CAN bus.
     * @param frame Pointer to a CANFrame struct to write to the bus.
     * @return True if the frame was written, false if the bus is not active, or the frame was not written.
     */
    virtual bool writeFrame(CANFrame* frame) = 0;

    /**
     * @brief Check if the bus is active.
     * @return True if the bus is active, false if the bus is not active.
     */
    virtual bool active() = 0;

    /**
     * @brief Get the ACK result of the last message sent.
     * @return The result of the last ACK or ACK_NONE if no message finished transmission since the last call to this
     * function, or the bus is not active.
     */
    virtual ACKResult getWriteFrameACK() = 0;

    virtual ~CANInterface() = default;
};

#endif // CANInterface_h
