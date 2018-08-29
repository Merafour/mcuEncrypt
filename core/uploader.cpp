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

#include "uploader.h"
#include <QDebug>
#include <QThread>
#include <stdio.h>
#include "program.h"
#include "core/tea.h"
const uint32_t uploader::public_key[4] = { 0x5f058808, 0x1adad47d, 0x73dc1683, 0x196c3671};
uint32_t uploader::private_key[4] = {0, 0, 0, 0};

uploader::uploader(QObject *parent) : QObject(parent)
{
    board_type = 0;
    board_rev = 0;
    bl_rev = 0;
    fw_maxsize = 0;
}

void uploader::recSerialData()
{
#if 0
    QByteArray ba;
    ba = my_port->_readAll();
//    qDebug()<<"uploader recSerialData";
//    qDebug()<<"recSerialData:" << ba.data();
//    qDebug("recSerialData: %d", ba.size());
    //qDebug("recSerialData: %s", ba.constData());
    uint32_t count=0;
    const char* datas = ba.constData();
    qDebug("recv[%3d]: ", ba.size());
    for(count=0; count<ba.size(); count++)
    {
        if(buffer.haveEmpty()) buffer.insert(datas[count]);
        qDebug("%02X ", datas[count]);
    }
    qDebug("\n");
//#else
    char datas[128];
    qint64 len=0;
    qint64 count=0;
    len = my_port->read(datas, sizeof(datas));
    qDebug("recv[%3d]: ", len);
   for(count=0; count<len; count++)
   {
       qDebug("%02X ", datas[count]);
   }
   qDebug("\n");
#endif
   QByteArray ba;
   ba = my_port->_readAll();
//    qDebug()<<"uploader recSerialData";
//    qDebug()<<"recSerialData:" << ba.data();
//    qDebug("recSerialData: %d", ba.size());
   //qDebug("recSerialData: %s", ba.constData());
   uint32_t count=0;
   const char* datas = ba.constData();
   int len = ba.size();
   //memset(recv_data, 0, sizeof(recv_data));
   for(count=0; count<len; count++)
   {
       recv_data[recv_len+count] = datas[count];
       //qDebug("%02X ", datas[count]);
   }
   //qDebug("recv[%3d]: %02X %02X %02X %02X %02X %02X", recv_len, recv_data[recv_len+0], recv_data[recv_len+1], recv_data[recv_len+2], recv_data[recv_len+3]\
           , recv_data[recv_len+4], recv_data[recv_len+5]);
   recv_len += count;
   //recv_rindex = 0;
#if 0
   qDebug("recv[%3d]: ", ba.size());
   for(count=0; count<len; count++)
   {
       qDebug("%02X ", (uint8_t)datas[count]);
   }
   qDebug("\n");
#endif

}

int uploader::setting(user_port *_port, QString _filename)
{
    my_port = _port;
    //connect(my_port, SIGNAL(readyRead()), this, SLOT(recSerialData())); // 连接串口收到数据事件与读取数据函数
    return 0;
}

int uploader::identify()
{
    //if(0==self->__sync()) return 0;
    if(0!=__sync())
    {
        return -1;
    }
    bl_rev=__getInfo(Info::BL_REV);
    // Console.WriteLine("2 " + DateTime.Now.Millisecond);
//     if ((bl_rev < (int)BL_REV_MIN) || (bl_rev > (int)BL_REV_MAX))
//     {
//         throw new Exception("Bootloader protocol mismatch");
//     }

     board_type = __getInfo(Info::BOARD_ID);
     board_rev = __getInfo(Info::BOARD_REV);
     fw_maxsize = __getInfo(Info::FLASH_SIZE);
    return 0;
}

int uploader::currentChecksum(Firmware *_fw)
{
    int count=0;
    char str[128];
    uint8_t buf[2];
    buf[0] = Code::GET_CRC;
    buf[1] = Code::EOC;
//    if (bl_rev < 3)
//        return 0;
    //int expect_crc = _fw->crc(fw_maxsize);
    int expect_crc = _fw->crc32(fw_maxsize);
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(3000); // 3s
    int report_crc = __recv_int();
    __getSync();

    qDebug("FW File 0x%X, %d" , expect_crc , expect_crc);
    qDebug("Current 0x%X, %d" , report_crc , report_crc);
    memset(str, 0, sizeof(str));
    snprintf(str, sizeof(str)-1, "FW File 0x%X, %d Current 0x%X, %d" , expect_crc , expect_crc , report_crc , report_crc);
    emit sig_progress_update(str, 0);
    qDebug(str);
    if (expect_crc == report_crc)
    {
         qDebug("Same Firmware. Not uploading");
         return 1;
    }
    return 0;
}

int uploader::get_id()
{
    uint8_t buf[2];
    buf[0] = Code::GET_ID;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(3000); // 3s
    mcu_uid[0] = __recv_int();
    mcu_uid[1] = __recv_int();
    mcu_uid[2] = __recv_int();
    mcu_uid[3] = __recv_int();
    if(0!=__getSync())
    {
        return -1;
    }
    qDebug("UID  : 0x%08X, 0x%08X, 0x%08X, 0x%08X" , mcu_uid[0] , mcu_uid[1], mcu_uid[2], mcu_uid[3]);
    return 0;
}

int uploader::encrypt()
{
    uint8_t encrypt[128];
    uint8_t buf[256+10];
    uint32_t _uid[4];
    uint16_t len=0;
    memset(encrypt, '1', sizeof(encrypt));
    memset(buf, 0, sizeof(buf));
    memcpy(_uid, mcu_uid, sizeof(_uid));
    /*
     * TEA 加密,迭代次数: 32*4 (i<4)
    */
    _key[0] = 0x2f2fb387U; // 密钥, 128 bit
    _key[1] = 0x2f3fb487U;
    _key[2] = 0x2f4fb687U;
    _key[3] = 0x2f5fb987U;
    //set_Iteration(64);
    for(int i=0; i<4; i++)
    {
        tea_encrypt(&_uid[0], _key);
        tea_encrypt(&_uid[2], _key);
    }
    len=sizeof(encrypt);
    buf[0] = Code::ENCRYPT;
    buf[1] = len;
    memcpy(&buf[2], encrypt,  len);
    memcpy(&buf[2], _uid,  sizeof(_uid));
    buf[2+len] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2+len+1);
    have_data(1000); // 1000ms
    if(0!=__getSync())
    {
        return -1;
    }
    return 0;
}

void uploader::generate_key()
{
    uint32_t number = 0;
    if((private_key[0]>0) && (private_key[1]>0) && (private_key[2]>0) && (private_key[3]>0)) return;
    random_init();
    number = qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    private_key[0] = number;
    number = qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    private_key[1] = number;
    number = qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    private_key[2] = number;
    number = qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    number = (number<<8) + qrand();
    private_key[3] = number;
    qDebug("private_key: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , private_key[0] , private_key[1], private_key[2], private_key[3]);
}

int uploader::get_key()
{
    uint32_t text[4];
    uint8_t buf[256+10];
    uint16_t len=0;
    generate_key(); // 生成私钥
    memcpy(text, private_key, sizeof(private_key));
    /*
     * TEA 加密,迭代次数: 32*4 (i<4)
    */
    for(int i=0; i<4; i++) // 使用公钥加密私钥
    {
        tea_encrypt(&text[0], public_key);
        tea_encrypt(&text[2], public_key);
    }
    // 发送私钥
    len = sizeof(text);
    buf[0] = Code::GET_KEY; // 交换私钥命令
    buf[1] = len;
    memcpy(&buf[2], text,  len);
    buf[2+len] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2+len+1);
    have_data(3000); // 3s
    // 接收设备端私钥
    connect_key[0] = __recv_int();
    connect_key[1] = __recv_int();
    connect_key[2] = __recv_int();
    connect_key[3] = __recv_int();
    if(0!=__getSync())
    {
        return -1;
    }
    qDebug("connect_key: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , connect_key[0] , connect_key[1], connect_key[2], connect_key[3]);
    /*
     * TEA 加密,迭代次数: 32
    */
    _key[0] = 0xd4bb30e2U; // 密钥, 128 bit
    _key[1] = 0xf9b9df6fU;
    _key[2] = 0x15da2d49U;
    _key[3] = 0xb10be924U;
    tea_decrypt(&mcu_uid[0], _key);
    tea_decrypt(&mcu_uid[2], _key);
    qDebug("UIDT2: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , mcu_uid[0] , mcu_uid[1], mcu_uid[2], mcu_uid[3]);
    return 0;
}

int uploader::get_id_tea_dhcp()
{
    uint8_t buf[2];
    get_key(); // 获取密钥
    buf[0] = Code::GET_ID_TEAD; // 动态密钥
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(3000); // 3s
    mcu_uid[0] = __recv_int();
    mcu_uid[1] = __recv_int();
    mcu_uid[2] = __recv_int();
    mcu_uid[3] = __recv_int();
    if(0!=__getSync())
    {
        return -1;
    }
    qDebug("UIDT1: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , mcu_uid[0] , mcu_uid[1], mcu_uid[2], mcu_uid[3]);
    /*
     * TEA 加密,迭代次数: 32
    */
    _key[0] = 0xd4bb30e2U; // 密钥, 128 bit
    _key[1] = 0xf9b9df6fU;
    _key[2] = 0x15da2d49U;
    _key[3] = 0xb10be924U;
    tea_decrypt(&mcu_uid[0], _key);
    tea_decrypt(&mcu_uid[2], _key);
    qDebug("UIDT2: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , mcu_uid[0] , mcu_uid[1], mcu_uid[2], mcu_uid[3]);
    return 0;
}

int uploader::get_id_tea()
{
    uint32_t text[4];
    uint8_t buf[2];
    buf[0] = Code::GET_ID_TEA;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    have_data(3000); // 3s
    mcu_uid[0] = __recv_int();
    mcu_uid[1] = __recv_int();
    mcu_uid[2] = __recv_int();
    mcu_uid[3] = __recv_int();
    text[0] = __recv_int();
    text[1] = __recv_int();
    text[2] = __recv_int();
    text[3] = __recv_int();
    if(0!=__getSync())
    {
        return -1;
    }
    qDebug("UIDT1: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , mcu_uid[0] , mcu_uid[1], mcu_uid[2], mcu_uid[3]);
    /*
     * TEA 加密,迭代次数: 32
    */
    _key[0] = 0xd4bb30e2U; // 密钥, 128 bit
    _key[1] = 0xf9b9df6fU;
    _key[2] = 0x15da2d49U;
    _key[3] = 0xb10be924U;
    tea_decrypt(&mcu_uid[0], _key);
    tea_decrypt(&mcu_uid[2], _key);
    tea_decrypt(&text[0], _key);
    tea_decrypt(&text[2], _key);
    qDebug("UIDT2: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , mcu_uid[0] , mcu_uid[1], mcu_uid[2], mcu_uid[3]);
    qDebug("UIDT2: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , text[0] , text[1], text[2], text[3]);
    qDebug("UIDT2: 0x%08X, 0x%08X, 0x%08X, 0x%08X" , mcu_uid[0]+text[0] , mcu_uid[1]+text[1], mcu_uid[2]+text[2], mcu_uid[3]+text[3]);
    if((0xFFFFFFFF != mcu_uid[0]+text[0]) || (0xFFFFFFFF != mcu_uid[1]+text[1]) || (0xFFFFFFFF != mcu_uid[2]+text[2]) || (0xFFFFFFFF != mcu_uid[3]+text[3]))
    {
        return -1;
    }
    return 0;
}

int uploader::encrypt_tea()
{
    ;
}

int uploader::upload(const Firmware *_fw)
{
    //this.port.ReadTimeout = 1000; // 1 sec

    //Make sure we are doing the right thing
//    if (self.board_type != fw.board_id)
//    {
//        if (!(self.board_type == 33 && fw.board_id == 9))
//            throw new Exception("Firmware not suitable for this board");
//    }

    if (fw_maxsize < _fw->image_size())
    {
        //throw new Exception("Firmware image is too large for this board");
        emit sig_progress_update(tr("Firmware image is too large for this board"), 0);
        return -1;
    }

    qDebug("erase...");
#if 0
    __erase();
#else
    if(0!=__erase())
    {
        emit sig_progress_update(tr("ERASE FAIL!"), 0);
        emit sig_progress_update("erase", Program::Code::ERR);
        return -1;
    }
#endif
    qDebug("program...");
    __program(_fw);
    if(0!=__program_sync)
    {
        emit sig_progress_update("program", Program::Code::ERR);
        return -1;
    }

    qDebug("verify...");
//    if (bl_rev == 2)
//        __verify_v2(fw);
//    else
    //currentChecksum((Firmware *)_fw);
    if(0!=__verify_v3((Firmware *)_fw))
    {
        emit sig_progress_update("verify", Program::Code::ERR);
        return -1;
    }

#if 0
    qDebug("done, rebooting.");
    __reboot();
#else
    qDebug("done, standby.");
    //__standby();
    qDebug("done, rebooting.");
    __reboot();
#endif
//    try
//    {
//        self.port.Close();
//    }
//    catch { }
    return 0;
}

//uint8_t uploader::__recv(uint8_t buf[], const int count)
//{
//    int pos = 0;
////    while (pos < count)
////        pos += port.Read(c, pos, count - pos);
//    QThread::msleep(1);
//    //if(my_port->waitForReadyRead(10)) recSerialData();
////    while(!buffer.haveData())
////    {
////        QThread::msleep(2);
////    }
//    //QThread::msleep(2);
//    for(pos=0; pos<count; pos++)
//    {
//        if(buffer.haveData()) buf[pos]=buffer.read();
//    }
//}

int uploader::__recv_int()
{
#if 0
    uint8_t raw[4];
    memset(raw, 0, sizeof(raw));
    __recv(raw, 4);
    int val = (raw[0]&0xFF) | (raw[1]&0xFF<<8) | (raw[2]&0xFF<<16) | (raw[3]&0xFF<<24);
    return val;
#else
    uint8_t raw[4];
    memset(raw, 0, sizeof(raw));
    __recv(raw, 4);
    //int val = (raw[0]&0xFF) | (raw[1]&0xFF<<8) | (raw[2]&0xFF<<16) | (raw[3]&0xFF<<24);
    uint32_t val=0;
    val = raw[3]&0xFF;
    val <<= 8; val |= raw[2]&0xFF;
    val <<= 8; val |= raw[1]&0xFF;
    val <<= 8; val |= raw[0]&0xFF;
    //val = (val<<8) | raw[2]&0xFF;
//    val = (val<<8) | raw[1]&0xFF;
//    val = (val<<8) | raw[0]&0xFF;
    return (int)val;
#endif
}

int uploader::__getSync()
{
    //qDebug("Sync[%3d]: %02X %02X %02X %02X %02X %02X", recv_len, recv_data[0], recv_data[1], recv_data[2], recv_data[3], recv_data[4], recv_data[5]);
    uint8_t c = __recv();
    if (c != (uint8_t)Code::INSYNC)
    {
        qDebug("unexpected {%0X} instead of INSYNC", c);
        return -1;
    }
    c = __recv();
    if (c == (uint8_t)Code::INVALID)
    {
        qDebug("bootloader reports INVALID OPERATION:%0X", c);
        return -1;
    }
    if (c == (uint8_t)Code::FAILED)
    {
        qDebug("bootloader reports OPERATION FAILED:%0X", c);
        return -1;
    }
    if (c != (uint8_t)Code::OK)
    {
        qDebug("unexpected {%0X} instead of OK", c);
        return -1;
    }
    return 0;
}

int uploader::__sync()
{
    uint8_t buf[2];
    buf[0] = Code::GET_SYNC;
    //buf[0] = Code::GET_CRC;
    buf[1] = Code::EOC;
    //my_port->clear();
//    buffer.clear();
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //QThread::msleep(1);
    have_data(100);
    //have_data(1);
    return __getSync();
}

int uploader::__b2_sync()
{
    uint8_t buf[2];
    buf[0] = Code::B2_SYNC;
    //buf[0] = Code::GET_CRC;
    buf[1] = Code::EOC;
    //my_port->clear();
//    buffer.clear();
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //QThread::msleep(1);
    have_data(100);
    //have_data(1);
    return __getSync();
}

int uploader::__scan()
{
    uint8_t buf[2];
    buf[0] = Code::GET_SYNC;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //QThread::msleep(1);
    //have_data(100);
    have_data(10);
    uint8_t insync = __recv();
    uint8_t ok = __recv();
    //return __getSync();
    if(((uint8_t)Code::INSYNC == insync) && ((uint8_t)Code::OK == ok)) return 0;
    return -1;
}

int uploader::__b2_scan()
{
    uint8_t buf[2];
    buf[0] = Code::B2_SYNC;
    buf[1] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //QThread::msleep(1);
    //have_data(100);
    have_data(10);
    uint8_t insync = __recv();
    uint8_t ok = __recv();
    //return __getSync();
    if(((uint8_t)Code::INSYNC == insync) && ((uint8_t)Code::OK == ok)) return 0;
    return -1;
}

int uploader::__getInfo(uploader::Info param)
{
    uint8_t buf[3];
    buf[0] = Code::GET_DEVICE;
    buf[1] = param;
    buf[2] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 3);
    //QThread::msleep(1500);
    int info = __recv_int();
    __getSync();
    qDebug("info:%0X %d", info, info);
    return info;
}
#include <QDateTime>

int uploader::__erase()
{
    char str[64];
    uint32_t count=0;
    //QDateTime current_date_time =QDateTime::currentDateTime();
    uint deadline =QDateTime::currentDateTime().toTime_t()+20;
    uint8_t buf[2];
    buf[0] = Code::CHIP_ERASE;
    buf[1] = Code::EOC;
    __sync();

    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    //DateTime deadline = DateTime.Now.AddSeconds(20);
    //while (DateTime.Now < deadline)
    count=0;
    deadline =QDateTime::currentDateTime().toTime_t()+20;
    while (QDateTime::currentDateTime().toTime_t() < deadline)
    {
        count++;
        //emit sig_progress_update("erase..."+count, 0);
#if 0
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str)-1, "erase... %d", count);
        emit sig_progress_update(str, 0);
#else
        emit sig_progress_update(tr("erase...")+QString::number(count), 0);
#endif
        QThread::msleep(100);
        if (my_port->waitForReadyRead(10))
        {
            qDebug("__erase btr %d", my_port->size());
            break;
        }
        if(0!=my_port->stop()) break;
        //qDebug("Time: %d", QDateTime::currentDateTime().toTime_t());
    }
    recSerialData();
    return __getSync();
}

void uploader::__program_multi(const uint8_t data[], const uint32_t len)
{
#if 0
    uint8_t buf[2];
    buf[0] = Code::PROG_MULTI;
    buf[1] = len;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2);
    __send(data, len);
    __send(Code::EOC);
#else
    uint8_t buf[PROG_MULTI_MAX+10];
    memset(buf, 0, sizeof(buf));
    buf[0] = Code::PROG_MULTI;
    buf[1] = len;
    memcpy(&buf[2], data,  len);
    buf[2+len] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2+len+1);
#endif
    have_data(100); // 100ms
    __program_sync = __getSync();
}

void uploader::__program(const Firmware *fw)
{
#if 1
    const uint8_t* code = fw->image();
    const uint32_t groups=fw->image_size()/PROG_MULTI_MAX + 1;
    uint32_t _size=0;
    uint32_t count=0;
    _size=fw->image_size();
#else
    const uint8_t* code = fw->image();
    const uint32_t groups=fw_maxsize/PROG_MULTI_MAX + 1;
    uint32_t _size=0;
    uint32_t count=0;
    _size=fw_maxsize;
#endif
    count=0;
    __program_sync = 0;
    qDebug("Program {%d}/{%d} %d", count, groups, __program_sync);
    for(count=0; count<groups; count++)
    //for(count=0; count<10; count++)
    {
        //emit sig_progress_update("program..."+QString::number(__program_sync), ((count)*100)/groups);
        emit sig_progress_update(tr("program... ")+QString::number(((count)*100)/groups)+"%", ((count)*100)/groups);
        _size -= PROG_MULTI_MAX;
        if(_size>PROG_MULTI_MAX) __program_multi(&code[count*PROG_MULTI_MAX], PROG_MULTI_MAX);
        else __program_multi(&code[count*PROG_MULTI_MAX], _size);

        //qDebug("Program {%d}/{%d} %d", count, groups, __program_sync);
        if(0!=__program_sync) return;
        QThread::msleep(1);
    }
    //emit sig_progress_update("Download done", ((count)*100)/groups);
    emit sig_progress_update(tr("Download done ")+QString::number(((count)*100)/groups)+"%", ((count)*100)/groups);
//    List<byte[]> groups = self.__split_len(code, PROG_MULTI_MAX);
//    Console.WriteLine("Programing packet total: "+groups.Count);
//    int a = 1;
//    foreach (Byte[] bytes in groups)
//    {
//        self.__program_multi(bytes);

//        Console.WriteLine("Program {0}/{1}",a, groups.Count);

//        a++;
//        if (ProgressEvent != null)
//            ProgressEvent((a / (float)groups.Count) * 100.0);
    //    }
}

int uploader::__verify_v3(Firmware *_fw)
{
    uint8_t buf[2];
    buf[0] = Code::GET_CRC;
    buf[1] = Code::EOC;
    int expect_crc = _fw->crc(fw_maxsize);
    QThread::msleep(1);
    __send(buf, 2);
    have_data(3000); // 3s
    int report_crc = __recv_int();
    __getSync();

    qDebug() << "Expected 0x" << QString::number(expect_crc, 16) << expect_crc;
    qDebug() << "Got      0x" << QString::number(report_crc, 16) << report_crc;
    if (report_crc != expect_crc)
    {
        //throw new Exception("Program CRC failed");
        emit sig_progress_update("Program CRC failed", 100);
        return -1;
    }
    return 0;
}

void uploader::__reboot()
{
    uint8_t buf[2];
    buf[0] = Code::REBOOT;
    buf[1] = Code::EOC;
    __send(buf, 2);
    my_port->flush();
    //QThread::msleep(10);
    have_data(300); // 300ms
    __getSync();
}

void uploader::__standby()
{
    uint8_t buf[2];
    buf[0] = Code::STANDBY;
    buf[1] = Code::EOC;
    __send(buf, 2);
    my_port->flush();
    //QThread::msleep(10);
    have_data(300); // 300ms
    __getSync();
}

//void uploader::__send(const uint8_t c)
//{
//    my_port->_write((const char *)&c, 1);
//}

void uploader::__send(const uint8_t buf[], const uint32_t len)
{
    my_port->_write((const char *)buf, len);
}

uploader_mavlink::uploader_mavlink(QObject *parent) : uploader(parent)
{
    ;
}

void uploader_mavlink::recSerialData()
{
    QByteArray ba;
    ba = my_port->_readAll();
 //    qDebug()<<"uploader recSerialData";
 //    qDebug()<<"recSerialData:" << ba.data();
 //    qDebug("recSerialData: %d", ba.size());
    //qDebug("recSerialData: %s", ba.constData());
    uint32_t count=0;
    const char* datas = ba.constData();
    int len = ba.size();
    //memset(recv_data, 0, sizeof(recv_data));
    for(count=0; count<len; count++)
    {
        recv_data[recv_len+count] = datas[count];
        //qDebug("%02X ", datas[count]);
    }
    //qDebug("recv[%3d]: %02X %02X %02X %02X %02X %02X", recv_len, recv_data[recv_len+0], recv_data[recv_len+1], recv_data[recv_len+2], recv_data[recv_len+3]\
            , recv_data[recv_len+4], recv_data[recv_len+5]);
    recv_len += count;
}

int uploader_mavlink::__recv(uint8_t buf[], const int count)
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

uint8_t uploader_mavlink::__recv()
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

//void uploader_mavlink::__send(const uint8_t c)
//{
//    my_port->_write((const char *)&c, 1);
//}

void uploader_mavlink::__send(const uint8_t buf[], const uint32_t len)
{
    qDebug() << "uploader_mavlink";
    my_port->_write((const char *)buf, len);
}

uploader_safe::uploader_safe(QObject *parent) : uploader(parent)
{
    send_count = 1;
    send_last = 0;
    safe_head = 0;
    safe_tail = 0;
    mcu_head = 0;
    mcu_tail = 0;
}

void uploader_safe::recSerialData()
{
    QByteArray ba;
//    char decode_buf[200];
//    uint16_t decode_len=0;
    ba = my_port->_readAll();
     //qDebug()<<"uploader recSerialData";
     //qDebug()<<"recSerialData:" << ba.data();
     //qDebug("uploader_safe: %d", ba.size());
    //qDebug("recSerialData: %s", ba.constData());
    uint32_t count=0;
    //int uart_in=0;
    const char* datas = ba.constData();
    int len = ba.size();
    //qDebug("uploader_safe recv[%d]: %d", recv_buf_len, len);
    //qDebug("uploader_safe recv[%d]", len);
    //memset(recv_data, 0, sizeof(recv_data));
    for(count=0; count<len; count++)
    {
        //recv_buf[recv_buf_len+count] = datas[count];
        mcu_buf_put(datas[count]);
        //qDebug("%02X ", (uint8_t)datas[count]);
    }
    //qDebug("recv[%3d]: %02X %02X %02X %02X %02X %02X", recv_len, recv_data[recv_len+0], recv_data[recv_len+1], recv_data[recv_len+2], recv_data[recv_len+3]\
            , recv_data[recv_len+4], recv_data[recv_len+5]);
    //recv_buf_len += count;
    //qDebug("uploader_safe recv[%d]: %d", recv_buf_len, len);
//    memset(decode_buf, 0, sizeof(decode_buf));
//    decode_len=0;
//    uart_in = mcu_buf_get();
//    while(uart_in>=0)
//    {
//        decode_buf[decode_len++] = (uint8_t)uart_in;
//        if(decode_len>=(sizeof(decode_buf)-1)) decode_len=0;
//        if(0 == decode(&_pack, decode_buf, decode_len) )
//        {
//            memset(decode_buf, 0, sizeof(decode_buf));
//            decode_len=0;
//            uint16_t i=0;
//            if(0==_pack.len) break; // ACK
//            for(i=0; i<_pack.len; i++)
//            {
//                safe_buf_put(_pack.data[i]);
//            }
//        }
//        uart_in = mcu_buf_get();
//    }
}

int uploader_safe::__recv(uint8_t buf[], const int count)
{
    int len=0;
    int	uart_in=0;
    //if(my_port->waitForReadyRead(1)) recSerialData();
    while(0==wait_pack(1))
    {
        uint16_t i=0;
        for(i=0; i<_pack.len; i++)
        {
            safe_buf_put(_pack.data[i]);
        }
    }
    for(len=0; len<count; len++)
    {
        uart_in = safe_buf_get();
        if(uart_in>=0)
        {
            buf[len] = (uint8_t)uart_in;
            continue;
        }
        break;
    }
    return 0;
}

uint8_t uploader_safe::__recv()
{
    int uart_in=0;
    //if(my_port->waitForReadyRead(1)) recSerialData();
    while(0==wait_pack(1))
    {
        uint16_t i=0;
        for(i=0; i<_pack.len; i++)
        {
            safe_buf_put(_pack.data[i]);
        }
    }
    uart_in = safe_buf_get();
    if(uart_in>=0)
    {
        return (uint8_t)uart_in;
    }
    return 0x01;
}

void uploader_safe::__program_multi(const uint8_t data[], const uint32_t len)
{
    uint8_t buf[PROG_MULTI_MAX+10];
    memset(buf, 0, sizeof(buf));
    buf[0] = Code::PROG_MULTI;
    buf[1] = len;
    memcpy(&buf[2], data,  len);
    buf[2+len] = Code::EOC;
    this->clear();
    QThread::msleep(1);
    __send(buf, 2+len+1);
    //have_data(100); // 100ms
    have_data(100); // 100ms
//    while(0==wait_pack(10))
//    {
//        uint16_t i=0;
//        for(i=0; i<_pack.len; i++)
//        {
//            safe_buf_put(_pack.data[i]);
//        }
//    }
    __program_sync = __getSync();
}

//void uploader_safe::__send(const uint8_t c)
//{
//#if 0
//    my_port->_write((const char *)&c, 1);
//#else
//    int _size = encode(&c, 1);
//    my_port->_write((const char *)pack_en, _size);
//#endif
//}

void uploader_safe::__send(const uint8_t buf[], const uint32_t len)
{
#if 0
    my_port->_write((const char *)buf, len);
#else
//    send_count = 0;
//    send_last = 0;
    int _size = encode(buf, len);
    int i=0;
    int num=0;
    uint16_t count=0;
    my_port->clear();
    my_port->_write((const char *)pack_en, _size);
    //my_port->flush();
    uint time_end =QDateTime::currentDateTime().toTime_t()+1;
//    qDebug() << "uploader_safe";
//    qDebug("pack[%d] ", _size);
//    for(i=0; i<_size; i++)
//    {
//        qDebug(" %02X ", pack_en[i]);
//    }
//    qDebug("\n");
    num=0;
    while((QDateTime::currentDateTime().toTime_t())<time_end)
    {
#if 0
        if(0!=wait_pack(100)) continue;
#else
        if(0!=wait_pack(100))
        {
            num++;
            if(num>6) continue;
            this->clear();
            my_port->_write((const char *)pack_en, _size);
            continue;
        }
#endif
        if(0 == _pack.status) continue; // bl
        //qDebug("_pack L[%d] C[%d] [%02X] send[%d] %d ", _pack.last, _pack.count, _pack.status, send_last, send_count);
        //if((0x01) == _pack.status) // OK
        if(((0x01) == _pack.status) || ((0x02) == _pack.status)) // 接收成功或收到重复数据包
        {
            send_last = send_count;
            count = send_count;
            count++;
            if(count>255) send_count=1;
            else send_count=count;
            //send_count = 0;
            break;
        }
        //qDebug("_pack send agane ");
        qDebug("_pack L[%d] C[%d] [%02X] send[%d] %d ", _pack.last, _pack.count, _pack.status, send_last, send_count);
        if(_pack.count == send_last) // 重发
        {
            this->clear();
            my_port->_write((const char *)pack_en, _size);
            //memset(&_pack, 0, sizeof(pack));
            continue;
        }
        qDebug("_pack reset send ");
        send_count = 0;
        send_last = 0;
        _size = encode(buf, len);
        this->clear();
        my_port->_write((const char *)pack_en, _size);
    }
#endif
}

int uploader_safe::wait_pack(const uint32_t timeout)
{
    //have_data(10);
    char decode_buf[200];
    uint16_t decode_len=0;
    int uart_in=0;
    memset(decode_buf, 0, sizeof(decode_buf));
    decode_len=0;
    //while(1)
    {
        have_data(timeout);
        memset(&_pack, 0, sizeof(pack));
        uart_in = mcu_buf_get();
        while(uart_in>=0)
        {
            decode_buf[decode_len++] = (uint8_t)uart_in;
            if(decode_len>=(sizeof(decode_buf)-1)) decode_len=0;
            if(0 == decode(&_pack, decode_buf, decode_len) )
            {
                return 0;
            }
            uart_in = mcu_buf_get();
        }
    }
    return -1;
}
int uploader_safe::decode(struct pack *_p, const char _buf[], const uint16_t lenght)  // 解码
{
#define DECODE_HEAD_LEN    5
    /**
  ack:
   | 1B | 1B|  1B |  1B  |  1B  | xB |crc|
   |0x7E|len|count| last |status|DATA|crc|
     */
    uint16_t i=0;
    uint16_t crc=0;
    const char *_data=NULL;
    for(i=0; i<lenght; i++)
    {
        if(PACK_HEAD == _buf[i])
        {
            //qDebug("decode [%d]: %02X", i, _buf[i]);
            _data = &_buf[i];
            _p->head = _data[0];
            _p->len  = _data[1];
            _p->count = _data[2];
            _p->last = _data[3];
            _p->status = _data[4];
            if((_p->len+i+DECODE_HEAD_LEN+2) > lenght) return -1;
            memcpy(_p->data, &_data[DECODE_HEAD_LEN], _p->len);
            crc = _data[DECODE_HEAD_LEN+_p->len]&0xFF;
            //qDebug("decode crc: %04X", crc);
            crc = (crc << 8) | _data[DECODE_HEAD_LEN+_p->len+1]&0xFF;
            //qDebug("decode crc: %04X", crc);
            _p->crc16 = crc;
            crc = crc16((const uint8_t*)&_data[0], DECODE_HEAD_LEN+_p->len);
            //qDebug("decode crc[%04X]: %04X", crc, _p->crc16);
            if(crc != _p->crc16) return -2;
            _p->recv = 1;
            return 0;
        }
    }
    return -3;
}

int uploader_safe::encode(const uint8_t _data[], const uint8_t lenght)  // 编码
{
#define ENCODE_HEAD_LEN    3
    /**
   | 1B | 1B|  1B | xB | 2B |
   |0x7E|len|count|DATA|crc|
     */
    uint16_t crc = 0;
    memset(pack_en, 0, sizeof(pack_en));
    pack_en[0] = PACK_HEAD;
    pack_en[1] = lenght;
    pack_en[2] = send_count;
    memcpy(&pack_en[ENCODE_HEAD_LEN], _data, lenght);
    crc = crc16(pack_en, lenght+ENCODE_HEAD_LEN);
    pack_en[ENCODE_HEAD_LEN+lenght] = (crc>>8)&0xFF;
    pack_en[ENCODE_HEAD_LEN+lenght+1] = (crc)&0xFF;
    return (lenght+ENCODE_HEAD_LEN+2);
}

/*
 * lenght:从包头到包尾不包括crc字节
 */
uint16_t uploader_safe::crc16(const uint8_t pack[], const uint16_t lenght)
{
    uint16_t crc=0;
    uint16_t sum=0;
    uint16_t _len=0;
    uint16_t i=0;
    //qDebug() << "uploader_safe crc16";
    //qDebug("pack[%d] ", lenght);
    _len=lenght - (lenght&0x1); // 2字节对对齐
    sum = 0;
    for(i=0; i<_len; i+=2)
    {
        crc = pack[i];
        crc = (crc<<8) + pack[i+1];
        sum += crc;
        //qDebug(" %04X ", crc);
    }
    if(lenght&0x1)
    {
        crc = pack[i];
        crc = (crc<<8) + 0xFF;
        sum += crc;
        //qDebug(" %04X ", crc);
    }
    //qDebug("\n");
    return sum;
}








