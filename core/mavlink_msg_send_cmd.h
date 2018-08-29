// MESSAGE SEND_CMD PACKING

#define MAVLINK_MSG_ID_SEND_CMD 96

MAVPACKED(
typedef struct __mavlink_send_cmd_t {
 char text[10]; /*< Status text message, without null termination character*/
}) mavlink_send_cmd_t;

#define MAVLINK_MSG_ID_SEND_CMD_LEN 10
#define MAVLINK_MSG_ID_SEND_CMD_MIN_LEN 10
#define MAVLINK_MSG_ID_96_LEN 10
#define MAVLINK_MSG_ID_96_MIN_LEN 10

#define MAVLINK_MSG_ID_SEND_CMD_CRC 79
#define MAVLINK_MSG_ID_96_CRC 79

#define MAVLINK_MSG_SEND_CMD_FIELD_TEXT_LEN 10

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_SEND_CMD { \
	96, \
	"SEND_CMD", \
	1, \
	{  { "text", NULL, MAVLINK_TYPE_CHAR, 10, 0, offsetof(mavlink_send_cmd_t, text) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_SEND_CMD { \
	"SEND_CMD", \
	1, \
	{  { "text", NULL, MAVLINK_TYPE_CHAR, 10, 0, offsetof(mavlink_send_cmd_t, text) }, \
         } \
}
#endif

/**
 * @brief Pack a send_cmd message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param text Status text message, without null termination character
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_send_cmd_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SEND_CMD_LEN];

	_mav_put_char_array(buf, 0, text, 10);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SEND_CMD_LEN);
#else
	mavlink_send_cmd_t packet;

	mav_array_memcpy(packet.text, text, sizeof(char)*10);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SEND_CMD_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SEND_CMD;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SEND_CMD_MIN_LEN, MAVLINK_MSG_ID_SEND_CMD_LEN, MAVLINK_MSG_ID_SEND_CMD_CRC);
}

/**
 * @brief Pack a send_cmd message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param text Status text message, without null termination character
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_send_cmd_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SEND_CMD_LEN];

	_mav_put_char_array(buf, 0, text, 10);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SEND_CMD_LEN);
#else
	mavlink_send_cmd_t packet;

	mav_array_memcpy(packet.text, text, sizeof(char)*10);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SEND_CMD_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SEND_CMD;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SEND_CMD_MIN_LEN, MAVLINK_MSG_ID_SEND_CMD_LEN, MAVLINK_MSG_ID_SEND_CMD_CRC);
}

/**
 * @brief Encode a send_cmd struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param send_cmd C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_send_cmd_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_send_cmd_t* send_cmd)
{
	return mavlink_msg_send_cmd_pack(system_id, component_id, msg, send_cmd->text);
}

/**
 * @brief Encode a send_cmd struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param send_cmd C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_send_cmd_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_send_cmd_t* send_cmd)
{
	return mavlink_msg_send_cmd_pack_chan(system_id, component_id, chan, msg, send_cmd->text);
}

/**
 * @brief Send a send_cmd message
 * @param chan MAVLink channel to send the message
 *
 * @param text Status text message, without null termination character
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_send_cmd_send(mavlink_channel_t chan, const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SEND_CMD_LEN];

	_mav_put_char_array(buf, 0, text, 10);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_CMD, buf, MAVLINK_MSG_ID_SEND_CMD_MIN_LEN, MAVLINK_MSG_ID_SEND_CMD_LEN, MAVLINK_MSG_ID_SEND_CMD_CRC);
#else
	mavlink_send_cmd_t packet;

	mav_array_memcpy(packet.text, text, sizeof(char)*10);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_CMD, (const char *)&packet, MAVLINK_MSG_ID_SEND_CMD_MIN_LEN, MAVLINK_MSG_ID_SEND_CMD_LEN, MAVLINK_MSG_ID_SEND_CMD_CRC);
#endif
}

/**
 * @brief Send a send_cmd message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_send_cmd_send_struct(mavlink_channel_t chan, const mavlink_send_cmd_t* send_cmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_send_cmd_send(chan, send_cmd->text);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_CMD, (const char *)send_cmd, MAVLINK_MSG_ID_SEND_CMD_MIN_LEN, MAVLINK_MSG_ID_SEND_CMD_LEN, MAVLINK_MSG_ID_SEND_CMD_CRC);
#endif
}

#if MAVLINK_MSG_ID_SEND_CMD_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_send_cmd_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  const char *text)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;

	_mav_put_char_array(buf, 0, text, 10);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_CMD, buf, MAVLINK_MSG_ID_SEND_CMD_MIN_LEN, MAVLINK_MSG_ID_SEND_CMD_LEN, MAVLINK_MSG_ID_SEND_CMD_CRC);
#else
	mavlink_send_cmd_t *packet = (mavlink_send_cmd_t *)msgbuf;

	mav_array_memcpy(packet->text, text, sizeof(char)*10);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SEND_CMD, (const char *)packet, MAVLINK_MSG_ID_SEND_CMD_MIN_LEN, MAVLINK_MSG_ID_SEND_CMD_LEN, MAVLINK_MSG_ID_SEND_CMD_CRC);
#endif
}
#endif

#endif

// MESSAGE SEND_CMD UNPACKING


/**
 * @brief Get field text from send_cmd message
 *
 * @return Status text message, without null termination character
 */
static inline uint16_t mavlink_msg_send_cmd_get_text(const mavlink_message_t* msg, char *text)
{
	return _MAV_RETURN_char_array(msg, text, 10,  0);
}

/**
 * @brief Decode a send_cmd message into a struct
 *
 * @param msg The message to decode
 * @param send_cmd C-struct to decode the message contents into
 */
static inline void mavlink_msg_send_cmd_decode(const mavlink_message_t* msg, mavlink_send_cmd_t* send_cmd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	mavlink_msg_send_cmd_get_text(msg, send_cmd->text);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_SEND_CMD_LEN? msg->len : MAVLINK_MSG_ID_SEND_CMD_LEN;
        memset(send_cmd, 0, MAVLINK_MSG_ID_SEND_CMD_LEN);
	memcpy(send_cmd, _MAV_PAYLOAD(msg), len);
#endif
}
