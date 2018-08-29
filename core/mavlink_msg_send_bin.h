// MESSAGE SEND_BIN PACKING

#define MAVLINK_MSG_ID_SEND_BIN 95

MAVPACKED(
typedef struct __mavlink_send_bin_t {
 char text[64]; /*< Status text message, without null termination character*/
}) mavlink_send_bin_t;

#define MAVLINK_MSG_ID_SEND_BIN_LEN 64
#define MAVLINK_MSG_ID_SEND_BIN_MIN_LEN 64
#define MAVLINK_MSG_ID_95_LEN 64
#define MAVLINK_MSG_ID_95_MIN_LEN 64

#define MAVLINK_MSG_ID_SEND_BIN_CRC 192
#define MAVLINK_MSG_ID_95_CRC 192

#define MAVLINK_MSG_SEND_BIN_FIELD_TEXT_LEN 64

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_SEND_BIN { \
	95, \
	"SEND_BIN", \
	1, \
	{  { "text", NULL, MAVLINK_TYPE_CHAR, 64, 0, offsetof(mavlink_send_bin_t, text) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_SEND_BIN { \
	"SEND_BIN", \
	1, \
	{  { "text", NULL, MAVLINK_TYPE_CHAR, 64, 0, offsetof(mavlink_send_bin_t, text) }, \
         } \
}
#endif

/**
 * @brief Pack a send_bin message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param text Status text message, without null termination character
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_send_bin_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SEND_BIN_LEN];

	_mav_put_char_array(buf, 0, text, 64);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SEND_BIN_LEN);
#else
	mavlink_send_bin_t packet;

	mav_array_memcpy(packet.text, text, sizeof(char)*64);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SEND_BIN_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SEND_BIN;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SEND_BIN_MIN_LEN, MAVLINK_MSG_ID_SEND_BIN_LEN, MAVLINK_MSG_ID_SEND_BIN_CRC);
}

/**
 * @brief Pack a send_bin message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param text Status text message, without null termination character
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_send_bin_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SEND_BIN_LEN];

	_mav_put_char_array(buf, 0, text, 64);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SEND_BIN_LEN);
#else
	mavlink_send_bin_t packet;

	mav_array_memcpy(packet.text, text, sizeof(char)*64);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SEND_BIN_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SEND_BIN;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SEND_BIN_MIN_LEN, MAVLINK_MSG_ID_SEND_BIN_LEN, MAVLINK_MSG_ID_SEND_BIN_CRC);
}

/**
 * @brief Encode a send_bin struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param send_bin C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_send_bin_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_send_bin_t* send_bin)
{
	return mavlink_msg_send_bin_pack(system_id, component_id, msg, send_bin->text);
}

/**
 * @brief Encode a send_bin struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param send_bin C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_send_bin_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_send_bin_t* send_bin)
{
	return mavlink_msg_send_bin_pack_chan(system_id, component_id, chan, msg, send_bin->text);
}

/**
 * @brief Send a send_bin message
 * @param chan MAVLink channel to send the message
 *
 * @param text Status text message, without null termination character
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_send_bin_send(mavlink_channel_t chan, const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SEND_BIN_LEN];

	_mav_put_char_array(buf, 0, text, 64);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_BIN, buf, MAVLINK_MSG_ID_SEND_BIN_MIN_LEN, MAVLINK_MSG_ID_SEND_BIN_LEN, MAVLINK_MSG_ID_SEND_BIN_CRC);
#else
	mavlink_send_bin_t packet;

	mav_array_memcpy(packet.text, text, sizeof(char)*64);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_BIN, (const char *)&packet, MAVLINK_MSG_ID_SEND_BIN_MIN_LEN, MAVLINK_MSG_ID_SEND_BIN_LEN, MAVLINK_MSG_ID_SEND_BIN_CRC);
#endif
}

/**
 * @brief Send a send_bin message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_send_bin_send_struct(mavlink_channel_t chan, const mavlink_send_bin_t* send_bin)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_send_bin_send(chan, send_bin->text);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_BIN, (const char *)send_bin, MAVLINK_MSG_ID_SEND_BIN_MIN_LEN, MAVLINK_MSG_ID_SEND_BIN_LEN, MAVLINK_MSG_ID_SEND_BIN_CRC);
#endif
}

#if MAVLINK_MSG_ID_SEND_BIN_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_send_bin_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;

	_mav_put_char_array(buf, 0, text, 64);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_BIN, buf, MAVLINK_MSG_ID_SEND_BIN_MIN_LEN, MAVLINK_MSG_ID_SEND_BIN_LEN, MAVLINK_MSG_ID_SEND_BIN_CRC);
#else
	mavlink_send_bin_t *packet = (mavlink_send_bin_t *)msgbuf;

	mav_array_memcpy(packet->text, text, sizeof(char)*64);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_BIN, (const char *)packet, MAVLINK_MSG_ID_SEND_BIN_MIN_LEN, MAVLINK_MSG_ID_SEND_BIN_LEN, MAVLINK_MSG_ID_SEND_BIN_CRC);
#endif
}
#endif

#endif

// MESSAGE SEND_BIN UNPACKING


/**
 * @brief Get field text from send_bin message
 *
 * @return Status text message, without null termination character
 */
static inline uint16_t mavlink_msg_send_bin_get_text(const mavlink_message_t* msg, char *text)
{
	return _MAV_RETURN_char_array(msg, text, 64,  0);
}

/**
 * @brief Decode a send_bin message into a struct
 *
 * @param msg The message to decode
 * @param send_bin C-struct to decode the message contents into
 */
static inline void mavlink_msg_send_bin_decode(const mavlink_message_t* msg, mavlink_send_bin_t* send_bin)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	mavlink_msg_send_bin_get_text(msg, send_bin->text);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_SEND_BIN_LEN? msg->len : MAVLINK_MSG_ID_SEND_BIN_LEN;
        memset(send_bin, 0, MAVLINK_MSG_ID_SEND_BIN_LEN);
	memcpy(send_bin, _MAV_PAYLOAD(msg), len);
#endif
}
