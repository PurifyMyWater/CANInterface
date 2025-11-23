#ifndef CANInterface_h
#define CANInterface_h

#include <cstdint>
#include <variant>

constexpr uint8_t  CAN_FRAME_MAX_DLC = 8;
constexpr uint32_t MAX_N_AI_STR_SIZE = 72;
// 72 = 40 (N_TAtype) + 3 (N_SA) + 3 (N_TA) + 25 (for the format string) + 1 (for the null terminator)
constexpr uint32_t MAX_FRAME_STR_SIZE = 181;
// 181 = 72 (N_AI) + 5 (flags) + 1 (data_length_code) + 17 (data) + 85 (format string) + 1 (null terminator)
constexpr uint32_t MAX_TX_ERROR_FLAGS_STR_SIZE = 60; // 54 (format string) + 5 (flags) + 1 (null terminator)

using N_TAtype_t = enum N_TAtype_t {
    CAN_UNKNOWN                             = 0,
    N_TATYPE_5_CAN_CLASSIC_29bit_Physical   = 218,
    N_TATYPE_6_CAN_CLASSIC_29bit_Functional = 219
};

constexpr uint8_t N_NFA_Header_Value  = 0b110;
constexpr uint8_t N_NFA_Padding_Value = 0b00;

using N_AI = union N_AI
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
        uint32_t ide : 1 {0}; /**< Extended Frame Format (29bit ID) */
        uint32_t rtr : 1 {0}; /**< Message is a Remote Frame */
        uint32_t fdf : 1 {0}; /**< Message is FD format, allow max 64 byte of data */
        uint32_t brs : 1 {0}; /**< Transmit message with Bit Rate Shift. */
        uint32_t esi : 1 {0}; /**< Transmit side error indicator for received frame */
    };

    union
    {
        uint64_t timestamp{0}; /**< Timestamp for received message */
        uint64_t trigger_time; /**< Trigger time for transmitting message*/
    };

    uint8_t data[CAN_FRAME_MAX_DLC]{}; /**< Data bytes (not relevant in RTR frame) */
};

using CANErrorState = enum CANErrorState {
    CAN_ERROR_ACTIVE,  /**< Error active state: TEC/REC < 96 */
    CAN_ERROR_WARNING, /**< Error warning state: TEC/REC >= 96 and < 128 */
    CAN_ERROR_PASSIVE, /**< Error passive state: TEC/REC >= 128 and < 256 */
    CAN_ERROR_BUS_OFF, /**< Bus-off state: TEC >= 256 (node offline) */
};

using CANTXErrorFlags = union CANTXErrorFlags
{
    struct
    {
        uint32_t arb_lost : 1;  /**< Arbitration lost error (lost arbitration during transmission) */
        uint32_t bit_err : 1;   /**< Bit error detected (dominant/recessive mismatch during transmission) */
        uint32_t form_err : 1;  /**< Form error detected (frame fixed-form bit violation) */
        uint32_t stuff_err : 1; /**< Stuff error detected (e.g. dominant error frame received) */
        uint32_t ack_err : 1;   /**< ACK error (no ack), transmission without acknowledge received */
    };
    uint32_t val; /**< Integrated error flags */
};

using CANTXDoneEvent = struct CANTXDoneEvent
{
    CANTXErrorFlags errorFlags;
    CANFrame        frame;
};

using CANRXDoneEvent = struct CANRXDoneEvent
{
    CANFrame frame;
};

using CANStateChangedEvent = struct CANStateChangedEvent
{
    CANErrorState oldState;
    CANErrorState newState;
};

using CANNoEvent = struct CANNoEvent
{
    // Empty struct to represent no event
};

using CANEvent = std::variant<CANRXDoneEvent, CANTXDoneEvent, CANStateChangedEvent, CANNoEvent>;

/**
 * @brief Convert N_TAtype to string.
 * @param nTAtype The N_TAtype to convert.
 * @return String representation of the N_TAtype.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(N_TAtype_t nTAtype);

/**
 * @brief Convert N_AI to string.
 * @param nAi The N_AI to convert.
 * @return String representation of the N_AI.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const N_AI& nAi);

/**
 * @brief Convert frame data to string.
 * @param data Pointer to the data bytes.
 * @param data_length_code The length of the data in bytes.
 * @return String representation of the frame data.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const uint8_t* data, uint8_t data_length_code);

/**
 * @brief Convert CANFrame to string.
 * @param frame The CANFrame to convert.
 * @return String representation of the CANFrame.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const CANFrame& frame);

/**
 * @brief Convert CANTXErrorFlags to string.
 * @param flags The CANTXErrorFlags to convert.
 * @return String representation of the CANTXErrorFlags.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const CANTXErrorFlags& flags);

/**
 * @brief Convert CANErrorState to string.
 * @param state The CANErrorState to convert.
 * @return String representation of the CANErrorState.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(CANErrorState state);

/**
 * @brief Convert CANTXDoneEvent to string.
 * @param event The CANTXDoneEvent to convert.
 * @return String representation of the CANTXDoneEvent.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const CANTXDoneEvent& event);

/**
 * @brief Convert CANRXDoneEvent to string.
 * @param event The CANRXDoneEvent to convert.
 * @return String representation of the CANRXDoneEvent.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const CANRXDoneEvent& event);

/**
 * @brief Convert CANStateChangedEvent to string.
 * @param event The CANStateChangedEvent to convert.
 * @return String representation of the CANStateChangedEvent.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const CANStateChangedEvent& event);

/**
 * @brief Convert CANNoEvent to string.
 * @param event The CANNoEvent to convert.
 * @return String representation of the CANNoEvent.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const CANNoEvent& event);

/**
 * @brief Convert CANEvent to string.
 * @param event The CANEvent to convert.
 * @return String representation of the CANEvent.
 *
 * @note The output string is static and will be overwritten on the next call to this function.
 */
const char* toString(const CANEvent& event);

/**
 * @brief Interface for a CAN bus driver.
 */
class CANInterface
{
public:
    /**
     * @brief Check and read the next event received from the CAN bus.
     * @param maxTimeToWait_ms Maximum time to wait for an event in milliseconds.
     * @return CANEvent The next CAN event. CANNoEvent if no event was received within the timeout.
     * @note This function will block until an event is received or the timeout is reached.
     */
    virtual CANEvent getEvent(uint32_t maxTimeToWait_ms) = 0;

    /**
     * @brief Send a CAN frame on the bus.
     * @param frame The CAN frame to send.
     * @param maxTimeToWait_ms Maximum time to wait for the frame to put in the transmit queue if it is full. -1 to wait
     * forever.
     * @return true if the frame was put in queue successfully.
     */
    virtual bool sendFrame(const CANFrame& frame, int32_t maxTimeToWait_ms) = 0;

    /**
     * @brief Start the CAN interface.
     * @return true if the interface was started successfully.
     */
    virtual bool enable() = 0;

    /**
     * @brief Stop the CAN interface.
     * @return true if the interface was stopped successfully.
     */
    virtual bool disable() = 0;

    /**
     * @brief Start the bus off recovery procedure.
     * @return true if the recovery procedure was started successfully.
     * @note Follow the CANStateChangedEvent to monitor the recovery process.
     */
    virtual bool recoverFromBusOff() = 0;

    virtual ~CANInterface() = default;
};

#endif // CANInterface_h
