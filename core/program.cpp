/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : program thread.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "program.h"
#include "uploader.h"

#include <QDateTime>

//uint8_t Program::_mavlink = 0;
uint8_t Program::_exit = 0;
enum Program::Bl_Mode Program::_bl_mode = Program::Bl_Mode::USB;

Program::Program(QObject *parent) : QObject(parent)
{

}
#if 0
void Program::doWork(const QString parameter, const Firmware *fw)
{
    //QByteArray tempdata;
//    user_port *_port;
    char data[16];
    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    int nValue = 0;
    _port = new user_port();
    _port->OpenPortDefault(parameter);
    //connect(_port, SIGNAL(readyRead()), this, SLOT(recSerialData())); // 连接串口收到数据事件与读取数据函数
    connect(_port, &user_port::readyRead, this, recSerialData);
    //_port->OpenPortDefault(parameter);
    Worker_count = 0;
    while ( (nValue < 100))
    {
        // 休眠50毫秒
        ++nValue;
//        tempdata.clear();
//        tempdata.append(uploader::Code::GET_SYNC);
//        tempdata.append(uploader::Code::EOC);
//        if(_port->isOpen()) _port->write(tempdata);
            //data[0] = uploader::Code::GET_SYNC;
            data[0] = uploader::Code::GET_CRC;
            data[1] = uploader::Code::EOC;
            _port->write(data, 2);
        // 准备更新
        emit resultReady(nValue);
        QThread::msleep(1);
        if(_port->waitForReadyRead(10)) recSerialData();
        QThread::msleep(50);
    }
    _port->close();
}
#else
#if 0
void Program::doWork(const QString parameter, const Firmware *fw)
{
    //QByteArray tempdata;
//    user_port *_port;
    char data[16];
    const uint32_t timeout = 20*30; // 30s
    uint32_t do_count;
    QString port;
    QString str;
    _port = new user_port();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");

    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    qDebug("Loaded firmware for %d,%d waiting for the bootloader...", fw->board_id, fw->board_revision);
    qDebug() << current_date ;
    up = new uploader(this);
    up->setting(_port, "");
    //upload.setting(_port, "");
    //connect(_port, &user_port::readyRead, up, &uploader::recSerialData);
    //connect(_port, &user_port::readyRead, &upload, &uploader::recSerialData);
    connect(up, &uploader::sig_progress_update, this, &Program::slots_progress_update);
    do_count = 0;
    while(do_count<timeout)
    {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            //port = "COM6";//info.portName();
            port = info.portName();
            //port = parameter;
#if 0
            if (!port.compare("COM") && !port.compare("APM") && !port.compare("ACM") && !port.compare("usb"))
                continue;
#endif
            current_date_time =QDateTime::currentDateTime();
            current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
            qDebug() << current_date << " Watting connect " << port << "sec:" << do_count/20;
            str = " Watting connect " + port + "sec:" + QString::number(do_count/20);
            //emit sig_progress_update("Trying Port "+port, 0);
            emit sig_progress_update(str, 0);
            if(0!=_port->OpenPortDefault(port)) continue;
//            up = new uploader(this);
//            up->setting(_port, "");
//            connect(_port, &user_port::readyRead, up, &uploader::recSerialData);
            //up->setting(_port, "");
            //connect(_port, SIGNAL(readyRead()), uploader, SLOT(recSerialData()));
            //connect(_port, &user_port::readyRead, this, recSerialData);
//            while(1)
//            {
//                up->identify();
//                QThread::msleep(50);
//            }
            if(0!=up->identify())
            //if(0!=upload.identify())
            {
                current_date_time =QDateTime::currentDateTime();
                current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
                qDebug() << current_date << "Not There..";
                _port->close();
                continue;
            }
            qDebug("Found board type %d boardrev %d bl rev %d fwmax %d on %s", up->board_type, up->board_rev,up->bl_rev,up->fw_maxsize, port.toLocal8Bit().constData());
            if(0!=up->currentChecksum((Firmware *)fw))
            {
                qDebug("No need to upload. already on the board");
                _port->close();
                //continue;
                //goto ret;
                do_count = timeout+100;
                break;
            }
            up->upload(fw);
            _port->close();
            //goto ret;
            do_count = timeout+100;
            break;
        }
        QThread::msleep(50);
        do_count++;
    }
    if((timeout<=do_count) && (do_count<(timeout+100))) emit sig_progress_update("Device connect timeout", 0);
//ret:
    delete up;
    delete _port;
//    int nValue = 0;
//    _port = new user_port();
//    _port->OpenPortDefault(parameter);
//    //connect(_port, SIGNAL(readyRead()), this, SLOT(recSerialData())); // 连接串口收到数据事件与读取数据函数
//    connect(_port, &user_port::readyRead, this, recSerialData);
//    //_port->OpenPortDefault(parameter);
//    Worker_count = 0;
//    while ( (nValue < 100))
//    {
//        // 休眠50毫秒
//        ++nValue;
////        tempdata.clear();
////        tempdata.append(uploader::Code::GET_SYNC);
////        tempdata.append(uploader::Code::EOC);
////        if(_port->isOpen()) _port->write(tempdata);
//            data[0] = uploader::Code::GET_SYNC;
//            data[1] = uploader::Code::EOC;
//            _port->write(data, 2);
//        // 准备更新
//        emit resultReady(nValue);
//        QThread::msleep(1);
//        if(_port->waitForReadyRead(10)) recSerialData();
//        QThread::msleep(50);
//    }
//    _port->close();
}
#else
void Program::doWork(const QString parameter, const Firmware *fw)
{
    //QByteArray tempdata;
//    user_port *_port;
    //char data[16];
    //const uint32_t timeout = 20*30; // 30s
    //uint32_t do_count;
    uint8_t _down=0;
    QString port;
    QString str;
    _port = new user_port();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");

    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    qDebug("Loaded firmware for %d,%d waiting for the bootloader...", fw->board_id, fw->board_revision);
    qDebug() << current_date ;
//#if 0
//    up = new uploader(this);
//#else
//    if(0==_mavlink) up = new uploader(this);
//    //else up = new uploader_mavlink(this);
//    else up = new uploader_safe(this);
//#endif
    if(Bl_Mode::WIFI == _bl_mode) up = new uploader_safe(this);
    else if(Bl_Mode::Serial == _bl_mode) up = new uploader_safe(this);
    else if(Bl_Mode::MSC == _bl_mode) up = new uploader(this);
    else up = new uploader(this);  // MSC
    up->setting(_port, "");
    //upload.setting(_port, "");
    //connect(_port, &user_port::readyRead, up, &uploader::recSerialData);
    //connect(_port, &user_port::readyRead, &upload, &uploader::recSerialData);
    connect(up, &uploader::sig_progress_update, this, &Program::slots_progress_update);
    //do_count = 0;
    uint time_start =QDateTime::currentDateTime().toTime_t();
    uint timeout =QDateTime::currentDateTime().toTime_t()+30;
    _down=0;
    while(QDateTime::currentDateTime().toTime_t()<timeout)
    {
        QThread::msleep(50);
        //do_count++;
        if(1==_exit) break;
        {
            port = parameter;
#if 0
            if (!port.compare("COM") && !port.compare("APM") && !port.compare("ACM") && !port.compare("usb"))
                continue;
#endif
            current_date_time =QDateTime::currentDateTime();
            current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
            qDebug() << current_date << " Watting connect " << port << "sec:" << (QDateTime::currentDateTime().toTime_t()-time_start);
            str = " Watting connect " + port + "sec:" + QString::number(QDateTime::currentDateTime().toTime_t()-time_start);
            //emit sig_progress_update("Trying Port "+port, 0);
            emit sig_progress_update(str, 0);
            if(0!=_port->OpenPortDefault(port)) continue;
            connect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
#if 1
            if(0!=up->identify())
            {
                current_date_time =QDateTime::currentDateTime();
                current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
                qDebug() << current_date << "Not There..";
                if(0!=_port->stop()) break;
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                continue;
            }
            if(0!=_port->stop()) break;
            qDebug("Found board type %d boardrev %d bl rev %d fwmax %d on %s", up->board_type, up->board_rev,up->bl_rev,up->fw_maxsize, port.toLocal8Bit().constData());
            if(0!=up->currentChecksum((Firmware *)fw))
            {
                qDebug("No need to upload. already on the board");
                emit sig_progress_update(tr("No need to upload. already on the board"), 0);
//                up->__standby();
                up->__reboot();
                QThread::msleep(2);
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                //continue;
                //goto ret;
                //do_count = timeout+100;
                _down = 1;
                break;
            }
            up->upload(fw);
#else
            //if(0!=up->__sync())
            if(0!=up->__b2_sync())
            {
                current_date_time =QDateTime::currentDateTime();
                current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
                qDebug() << current_date << "Not There..";
                if(0!=_port->stop()) break;
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                continue;
            }
            if(0!=_port->stop()) break;
            qDebug("Found board type %d boardrev %d bl rev %d fwmax %d on %s", up->board_type, up->board_rev,up->bl_rev,up->fw_maxsize, port.toLocal8Bit().constData());
            if(0!=up->get_id())
            {
                qDebug("No need to upload. already on the board");
                emit sig_progress_update(tr("Device ID not get"), 0);
                QThread::msleep(2);
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                //continue;
                //goto ret;
                //do_count = timeout+100;
                _down = 1;
                break;
            }
            if(0!=up->encrypt())
            {
                qDebug("encrypt fail");
                emit sig_progress_update(tr("encrypt fail"), 0);
            }
            emit sig_progress_update(tr("encrypt success"), 0);
            qDebug("encrypt success");
#endif
            disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            _port->close();
            //goto ret;
            //do_count = timeout+100;
            _down = 1;
            break;
        }
    }
    sig_progress_update(port, Code::DOWN);
    //if((timeout<=do_count) && (do_count<(timeout+100))) emit sig_progress_update("Device connect timeout", 0);
    if(0==_down) emit sig_progress_update("Device connect timeout", 0);
    delete up;
    delete _port;
    uint time_end =QDateTime::currentDateTime().toTime_t();
    uint _time = time_end-time_start;
    sig_progress_update(" Time:"+QString::number(_time)+" s", Code::TIME);
    if(1==_exit) exit(0);
}

void Program::doWorkEncrypt(const QString parameter)
{
    uint8_t _down=0;
    QString port;
    QString str;
    _port = new user_port();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    qDebug() << current_date ;
    if(Bl_Mode::WIFI == _bl_mode) up = new uploader_safe(this);
    else if(Bl_Mode::Serial == _bl_mode) up = new uploader_safe(this);
    else if(Bl_Mode::MSC == _bl_mode) up = new uploader(this);
    else up = new uploader(this);  // MSC
    up->setting(_port, "");
    //upload.setting(_port, "");
    //connect(_port, &user_port::readyRead, up, &uploader::recSerialData);
    //connect(_port, &user_port::readyRead, &upload, &uploader::recSerialData);
    connect(up, &uploader::sig_progress_update, this, &Program::slots_progress_update);
    //do_count = 0;
    uint time_start =QDateTime::currentDateTime().toTime_t();
    uint timeout =QDateTime::currentDateTime().toTime_t()+30;
    _down=0;
    while(QDateTime::currentDateTime().toTime_t()<timeout)
    {
        QThread::msleep(50);
        //do_count++;
        if(1==_exit) break;
        {
            port = parameter;
#if 0
            if (!port.compare("COM") && !port.compare("APM") && !port.compare("ACM") && !port.compare("usb"))
                continue;
#endif
            current_date_time =QDateTime::currentDateTime();
            current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
            qDebug() << current_date << " Watting connect " << port << "sec:" << (QDateTime::currentDateTime().toTime_t()-time_start);
            str = " Watting connect " + port + "sec:" + QString::number(QDateTime::currentDateTime().toTime_t()-time_start);
            //emit sig_progress_update("Trying Port "+port, 0);
            emit sig_progress_update(str, 0);
            if(0!=_port->OpenPortDefault(port)) continue;
            connect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            //if(0!=up->__sync())
            if(0!=up->__b2_sync())
            {
                current_date_time =QDateTime::currentDateTime();
                current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
                qDebug() << current_date << "Not There..";
                if(0!=_port->stop()) break;
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                continue;
            }
            if(0!=_port->stop()) break;
            qDebug("Found board type %d boardrev %d bl rev %d fwmax %d on %s", up->board_type, up->board_rev,up->bl_rev,up->fw_maxsize, port.toLocal8Bit().constData());
            //if(0!=up->get_id())
            if(0!=up->get_id_tea())
            {
                qDebug("No need to upload. already on the board");
                emit sig_progress_update(tr("Device ID not get"), 0);
                QThread::msleep(2);
                disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
                _port->close();
                //continue;
                //goto ret;
                //do_count = timeout+100;
                _down = 1;
                break;
            }
            if(0!=up->encrypt())
            {
                qDebug("encrypt fail");
                emit sig_progress_update(tr("encrypt fail"), 0);
            }
            emit sig_progress_update(tr("encrypt success"), 0);
            qDebug("encrypt success");
            disconnect(_port, &user_port::errorOccurred, _port, &user_port::errorOccurred_slots);
            _port->close();
            //goto ret;
            //do_count = timeout+100;
            _down = 1;
            break;
        }
    }
    sig_progress_update(port, Code::DOWN);
    //if((timeout<=do_count) && (do_count<(timeout+100))) emit sig_progress_update("Device connect timeout", 0);
    if(0==_down) emit sig_progress_update("Device connect timeout", 0);
    delete up;
    delete _port;
    uint time_end =QDateTime::currentDateTime().toTime_t();
    uint _time = time_end-time_start;
    sig_progress_update(" Time:"+QString::number(_time)+" s", Code::TIME);
    if(1==_exit) exit(0);
}
#endif
#endif
void Program::recSerialData()
{
#if 0
    QByteArray ba;
    ba = my_port->readAll();
    qDebug()<<"recSerialData";
    qDebug()<<"recSerialData:" << ba.data();
    qDebug("recSerialData: %d", ba.size());
    //qDebug("recSerialData: %s", ba.constData());
#else
    char datas[128];
    qint64 len=0;
    qint64 count=0;
    Worker_count++;
    len = _port->read(datas, sizeof(datas));
    qDebug("recv[%3d]: ", Worker_count);
   for(count=0; count<len; count++)
   {
       qDebug("%02X ", datas[count]);
   }
   qDebug("\n");
#endif
}
