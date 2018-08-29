/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : upload lib.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/


#ifndef UPLOADER_H
#define UPLOADER_H

#include "user_port.h"
#include <QString>
#include <QObject>
#include <QThread>
#include <QTime>

#include"Firmware.h"

class uploader : public QObject
{
    Q_OBJECT

public:
    explicit uploader(QObject *parent = nullptr);
//    ~uploader();

//private slots:
public slots:

    virtual void recSerialData();
signals:
    void sig_progress_update(const QString &titel, int value);
public:
    enum Code
    {
        // response codes
        NOP = 0x00,
        OK = 0x10,
        FAILED = 0x11,
        INSYNC = 0x12,
        INVALID = 0x13,//	# rev3+

        // protocol commands
        EOC = 0x20,
        GET_SYNC = 0x21,
        GET_DEVICE = 0x22,	// returns DEVICE_ID and FREQ bytes
        CHIP_ERASE = 0x23,
        CHIP_VERIFY = 0x24,//# rev2 only
        PROG_MULTI = 0x27,
        READ_MULTI = 0x28,//# rev2 only
        GET_CRC = 0x29,//	# rev3+
        GET_OTP = 0x2a, // read a byte from OTP at the given address
        GET_SN = 0x2b,    // read a word from UDID area ( Serial)  at the given address
        GET_CHIP = 0x2c, // read chip version (MCU IDCODE)
        PROTO_SET_DELAY	= 0x2d, // set minimum boot delay
        REBOOT = 0x30,
        STANDBY = 0x32,
        GET_ID = 0x33,
        ENCRYPT = 0x34,
        B2_SYNC = 0x35,
        GET_ID_TEA = 0x36,
        ENCRYPT_TEA = 0x37,
        GET_KEY = 0x38, // 交换私钥
        GET_ID_TEAD = 0x39, // 动态密钥
    };

    enum Info {
        BL_REV = 1,//	# bootloader protocol revision
        BOARD_ID = 2,//	# board type
        BOARD_REV = 3,//	# board revision
        FLASH_SIZE = 4,//	# max firmware size in bytes
        VEC_AREA = 5
    };

    const uint8_t BL_REV_MIN = 2;//	# minimum supported bootloader protocol
    const uint8_t BL_REV_MAX = 5;//	# maximum supported bootloader protocol
//    const uint8_t PROG_MULTI_MAX = 60;//		# protocol max is 255, must be multiple of 4
//    const uint8_t READ_MULTI_MAX = 60;//		# protocol max is 255, something overflows with >= 64
//    const uint8_t PROG_MULTI_MAX = 200;//		# protocol max is 255, must be multiple of 4
//    const uint8_t READ_MULTI_MAX = 200;//		# protocol max is 255, something overflows with >= 64
    const uint8_t PROG_MULTI_MAX = 200;//		# protocol max is 255, must be multiple of 4
    const uint8_t READ_MULTI_MAX = 200;//		# protocol max is 255, something overflows with >= 64


public:
    int setting(user_port *_port, QString _filename);
    int identify(void);
    int currentChecksum(Firmware *_fw);
    /* 通信不加密 */
    int get_id(void);
    int encrypt(void);
    /* 通信加密 */
    void generate_key(void); // 生成私钥
    int get_key(void);
    int get_id_tea_dhcp(void); // 动态密钥
    int get_id_tea(void);
    int encrypt_tea(void);
    int upload(const Firmware *_fw);

    virtual int __recv(uint8_t buf[], const int count = 2)
    {
        int len=0;
        if(my_port->waitForReadyRead(1)) recSerialData();
        if(0==recv_rindex)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        if(recv_rindex>=recv_len)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        for(len=0; len<count; len++)
        {
            buf[len] = (uint8_t)recv_data[recv_rindex++];
        }
        return 0;
    }
    virtual uint8_t __recv(void)
    {
#if 0
        uint8_t c=0;
        __recv(&c);
        return c;
#else
        if(my_port->waitForReadyRead(1)) recSerialData();
        if(0==recv_rindex)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        if(recv_rindex>=recv_len)
        {
            //QThread::msleep(1);
            if(my_port->waitForReadyRead(10)) recSerialData();
        }
        return (uint8_t)recv_data[recv_rindex++];
#endif
    }
    int have_data(const uint32_t timeout)
    {
        //return recv_len;
        //while(!my_port->waitForReadyRead(10)) ;
        uint32_t _time;
#if 0
        for(_time=0; _time<timeout; _time+=10)
        {
            if(my_port->waitForReadyRead(10)) break;
        }
#else
        for(_time=0; _time<timeout; _time++)
        {
            if(my_port->waitForReadyRead(1)) break;
            if(0!=my_port->stop()) break;
        }
#endif
        recSerialData();
        return my_port->size();
    }
    virtual void clear(void)
    {
        my_port->clear();
        memset(recv_data, 0, sizeof(recv_data));
        recv_len = 0;
        recv_rindex = 0;
        //buffer.clear();
    }
    static int random_init(void)
    {
        int number = 0;
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        number = qrand();
        return number;
    }
    int __recv_int(void);
    int __getSync(void);
    int __sync(void);
    int __b2_sync(void);
    int __scan(void);
    int __b2_scan(void);
    int __getInfo(Info param);
    int __erase(void);
    virtual void __program_multi(const uint8_t data[], const uint32_t len);
    void __program(const Firmware *fw);
    int __verify_v3(Firmware *_fw);
    void __reboot(void);
    void __standby(void);
    //void __send(const uint8_t c);
    virtual void __send(const uint8_t buf[], const uint32_t len);


    int board_type;
    int board_rev;
    int fw_maxsize;
    int bl_rev;
protected:
    user_port *my_port;
    char recv_data[128];
    uint16_t recv_len;
    uint16_t recv_rindex;
    int __program_sync;
    uint32_t mcu_uid[4];
    uint32_t _key[4];
    static const uint32_t public_key[4];// = { 0x5f058808, 0x1adad47d, 0x73dc1683, 0x196c3671};
    static uint32_t private_key[4];
    uint32_t connect_key[4];
};

class uploader_mavlink : public uploader
{
    Q_OBJECT

public:
    explicit uploader_mavlink(QObject *parent = nullptr);

public slots:

    void recSerialData();
signals:
    //void sig_progress_update(const QString &titel, int value);

public:

    int __recv(uint8_t buf[], const int count = 2) ;
    uint8_t __recv(void) ;
    //void __send(const uint8_t c) ;
    void __send(const uint8_t buf[], const uint32_t len) ;
protected:
};

/**
  safe:
  count: [1-255]
  crc16: D:2B, (D<<8)|0xFF
  len;
  _len=len&0xFE;
  uint16_t crc16;
  sum=0;
  for(i=0; i<_len; i++)
  {
        crc16 = D[i];
        crc16 = crc16<<8 + D[i+1];
        sum += crc16;
   }
   if(len&0x1)
   {
        crc16 = D[i];
        crc16 = crc16<<8 + 0xFF;
        sum += crc16;
    }

  send:
   | 1B | 1B|  1B | xB | 2B |
   |0x7E|len|count|DATA|crc|
eg:|0x7E|
  ack:
   | 1B | 1B|  1B |  1B  |  1B  | xB |crc|
   |0x7E|len|count| last |status|DATA|crc|
 */
class uploader_safe : public uploader
{
    Q_OBJECT

public:
    explicit uploader_safe(QObject *parent = nullptr);

//private slots:
public slots:

    void recSerialData() override;
signals:
    //void sig_progress_update(const QString &titel, int value);

public:

    int __recv(uint8_t buf[], const int count = 2) override;
    uint8_t __recv(void) override;
    //void __send(const uint8_t c) ;
    void __program_multi(const uint8_t data[], const uint32_t len);
    void __send(const uint8_t buf[], const uint32_t len) override;
    void clear(void) override
    {
        uploader::clear();
        //memset(recv_buf, 0, sizeof(recv_buf));
        memset(&_pack, 0, sizeof(pack));
        //recv_buf_len = 0;
    }
//protected:
private:
#define PACK_LEN    210
#define PACK_HEAD   0x7E
    struct  pack
    {
         uint8_t head;
         uint8_t len;
         uint8_t count;
         uint8_t last;
         uint8_t status;
         uint8_t data[PACK_LEN-5];
         uint16_t crc16;
         uint8_t recv;
    }_pack;
    uint8_t pack_en[PACK_LEN];
    uint8_t send_count;
    uint8_t send_last;
    int decode(struct pack *_p, const char _buf[], const uint16_t lenght);  // 解码
    int wait_pack(const uint32_t timeout);
    int encode(const uint8_t _data[], const uint8_t lenght);  // 编码
    uint16_t crc16(const uint8_t pack[], const uint16_t lenght);
    unsigned int mcu_head, mcu_tail;
    uint8_t mcu_buf[256];
    void mcu_buf_put(uint8_t b)
    {
        //qDebug("mcu_buf_put: %02X ", b);
        unsigned next = (mcu_head + 1) % sizeof(mcu_buf);
        if (next != mcu_tail) {
            mcu_buf[mcu_head] = b;
            mcu_head = next;
        }
    }
    int mcu_buf_get(void)
    {
        //uint8_t b=0;
        int	ret = -1;
        //qDebug("mcu_buf_get: %d %d ", mcu_head, mcu_tail);
        if (mcu_tail != mcu_head) {
            ret = mcu_buf[mcu_tail];
            mcu_tail = (mcu_tail + 1) % sizeof(mcu_buf);
            //qDebug("mcu_buf_get: %d %d %02X", mcu_head, mcu_tail, (uint8_t)ret);
        }
        return ret;
    }
    unsigned int safe_head, safe_tail;
    uint8_t safe_buf[256];
    void safe_buf_put(uint8_t b)
    {
        //qDebug("safe_buf_put: %02X ", b);
        unsigned next = (safe_head + 1) % sizeof(safe_buf);
        if (next != safe_tail) {
            safe_buf[safe_head] = b;
            safe_head = next;
        }
    }
    int safe_buf_get(void)
    {
        //uint8_t b=0;
        int	ret = -1;
        //qDebug("safe_buf_get: %d %d ", safe_head, safe_tail);
        if (safe_tail != safe_head) {
            ret = safe_buf[safe_tail];
            safe_tail = (safe_tail + 1) % sizeof(safe_buf);
        }
        return ret;
    }
};

#endif // UPLOADER_H
