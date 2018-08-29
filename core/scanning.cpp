/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : scanning thread.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "scanning.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "uploader.h"
#include "user_port.h"
#include "program.h"
#include <QDateTime>

QStringList Scanning::port_list;
QStringList Scanning::pull_list;
QMutex Scanning::mutex;
uint8_t Scanning::_is_run = 0;
uint8_t Scanning::_overload = 0;
uint8_t Scanning::_stop = 0;
uint8_t Scanning::_scanning = 0;
uint8_t Scanning::_exit = 0;

Scanning::Scanning(QObject *parent) : QObject(parent)
{
}

void Scanning::Serial_Free(const QString COM)
{
    qDebug()<<"Free2 tmp ="<< COM;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        //qDebug()<<"Free tmp ="<< tmp;
        if(0==COM.compare(tmp))
        {
            port_list.removeOne(COM);
            break;
        }
    }
    pull_list << COM;
    mutex.unlock();
}
#if 0
void Scanning::doWork()
{
    char data[16];
    char sync[2];
    int lenght=0;
    int count;
    QString port;
    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    //int nValue = 0;
    my_port = new user_port();
    sync[0] = uploader::Code::GET_SYNC;
    sync[1] = uploader::Code::EOC;
    port_list.clear();
    port_list << "NULL";
    _is_run = 1;
    qDebug() << "_overload : " << _overload;
    if(_overload>0) return ;
    _overload = 1;
    count = 0;
    // 获取同步,扫描飞控
    while (1)
    {
        count++;
        //qDebug() << "doWork : " << count;
        // 准备更新
        //emit resultReady(nValue);
        _is_run = 0;
        QThread::msleep(500); // 2Hz
        _is_run = 1;
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            port = info.portName();
            //if(0!=info.description().compare("USB-SERIAL CH340")) continue;
            if(serach_port(port)) continue;
            if(0!=my_port->OpenPortDefault(port)) continue;
            //qDebug() << port << "Scanning...";
            emit sig_progress_update("Scanning Device... "+port, Program::Code::SCAN);
            my_port->write(sync, 2);  // SYNC
            have_data(10);
            memset(data, 0, sizeof(data));
            lenght = my_port->read(data, sizeof(data));
            if(0==lenght)
            {
                my_port->close();
                continue;
            }
            //qDebug("SYNC[%d]: %02X %02X", lenght, data[0], data[1]);
            if((uploader::Code::INSYNC == data[0]) && (uploader::Code::OK == data[1]))
            {
                //emit sig_Serial_Idle(port);
                send_idle(port);
                //qDebug("port: %s, idle", port.toLocal8Bit().constData());
            }
            my_port->close();
        }

    }
}
#else
/*
 * 扫描 bootloader以升级固件
*/
void Scanning::doWork()
{
    int count;
    int i=0;
    QString port;
    QString pull;
    uint deadline =QDateTime::currentDateTime().toTime_t();
    uint time =QDateTime::currentDateTime().toTime_t();
    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    //int nValue = 0;
    my_port = new user_port();
    pull_port = new user_port();
    port_list.clear();
    port_list << "NULL";
    pull_list.clear();
    //pull_list << "NULL";
    _is_run = 1;
    qDebug() << "_overload : " << _overload;
    if(_overload>0) return ;
    _overload = 1;
    count = 0;
    //up = new uploader(this);
//    if(Program::get_mavlink())
//    {
//        qDebug() << "new uploader_safe ";
//        up = new uploader_safe(this);
//    }
//    else
//    {
//        qDebug() << "new uploader ";
//        up = new uploader(this);
//    }
    if(Program::Bl_Mode::USB == Program::get_bl_mode())
    {
        qDebug() << "new uploader ";
        up = new uploader(this);
    }
    else if(Program::Bl_Mode::WIFI == Program::get_bl_mode())
    {
        qDebug() << "new uploader_safe ";
        up = new uploader_safe(this);
    }
    else if(Program::Bl_Mode::Serial == Program::get_bl_mode())
    {
        qDebug() << "new uploader_safe ";
        up = new uploader_safe(this);
    }
    else
    {
        qDebug() << "new uploader ";
        up = new uploader(this);
    }
    //up = new uploader_safe(this);
    up->setting(my_port, "");
    deadline =QDateTime::currentDateTime().toTime_t();
    connect(pull_port, &user_port::errorOccurred, pull_port, &user_port::errorOccurred_slots);
    // 获取同步,扫描飞控
    //while (1)
    while (0==_stop)
    {
        count++;
        //qDebug() << "Scanning doWork : " << count;
        //qDebug() << "Scanning doWork : " << QString::number(QDateTime::currentDateTime().toTime_t()-time);
        // 准备更新
        //emit resultReady(nValue);
        if(false == pull.isEmpty())
        {
            if(0!=pull_port->stop())
            {
                //emit
                emit sig_progress_update(pull, Program::Code::PULL);
                pull="";
            }
        }
        mutex.lock();
        if(false == pull.isEmpty()) pull_list << pull;
        if(pull_list.size()>0)  // 检测设备拔出
        {
//            for(int i = 0; i< pull_list.size();++i)
//            {
//                QString tmp = pull_list.at(i);
//                qDebug()<<"pull_list_" << QString::number(i) << ": " << tmp;
//            }
            pull = pull_list.at(0);
            pull_list.removeOne(pull);
            pull_port->OpenPortDefault(pull);
        }
        mutex.unlock();
        _is_run = 0;
        QThread::msleep(100); // 2Hz
        _is_run = 1;
        if(1!=_scanning)
        {
            emit sig_progress_update(tr("watting... ")+QString::number(QDateTime::currentDateTime().toTime_t()-deadline), Program::Code::SCAN);
            time =QDateTime::currentDateTime().toTime_t();
            continue;
        }
        if(1==_exit) break;
        deadline =QDateTime::currentDateTime().toTime_t();
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            port = info.portName();
            //if(0!=info.description().compare("USB-SERIAL CH340")) continue;
            if(0==port.compare("COM1")) continue;
            if(serach_port(port)) continue;
            if(0!=my_port->OpenPortDefault(port)) continue;
            connect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
            qDebug() << port << "Scanning...";
            emit sig_progress_update(tr("Scanning Device... ")+port, Program::Code::SCAN);
            for(i=0; i<5; i++)
            {
                if(0!=up->__scan()) break;
                //QThread::msleep(2); // 2Hz
            }
            if(0!=up->__scan())
            //if(0!=up->__b2_scan())
            {
                disconnect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
                my_port->close();
                continue;
            }
            disconnect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
            send_idle(port);
            my_port->close();
        }
    }
    disconnect(pull_port, &user_port::errorOccurred, pull_port, &user_port::errorOccurred_slots);
    if(1==_exit) exit(0);
}
/*
 * 扫描 B2 以加密设备
*/
void Scanning::doWorkEncrypt()
{
    int count;
    int i=0;
    QString port;
    QString pull;
    uint deadline =QDateTime::currentDateTime().toTime_t();
    uint time =QDateTime::currentDateTime().toTime_t();
    qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
    //int nValue = 0;
    my_port = new user_port();
    pull_port = new user_port();
    port_list.clear();
    port_list << "NULL";
    pull_list.clear();
    //pull_list << "NULL";
    _is_run = 1;
    qDebug() << "_overload : " << _overload;
    if(_overload>0) return ;
    _overload = 1;
    count = 0;
    if(Program::Bl_Mode::USB == Program::get_bl_mode())
    {
        qDebug() << "new uploader ";
        up = new uploader(this);
    }
    else if(Program::Bl_Mode::WIFI == Program::get_bl_mode())
    {
        qDebug() << "new uploader_safe ";
        up = new uploader_safe(this);
    }
    else if(Program::Bl_Mode::Serial == Program::get_bl_mode())
    {
        qDebug() << "new uploader_safe ";
        up = new uploader_safe(this);
    }
    else
    {
        qDebug() << "new uploader ";
        up = new uploader(this);
    }
    //up = new uploader_safe(this);
    up->setting(my_port, "");
    deadline =QDateTime::currentDateTime().toTime_t();
    connect(pull_port, &user_port::errorOccurred, pull_port, &user_port::errorOccurred_slots);
    // 获取同步,扫描飞控
    //while (1)
    while (0==_stop)
    {
        count++;
        //qDebug() << "Scanning doWork : " << count;
        //qDebug() << "Scanning doWork : " << QString::number(QDateTime::currentDateTime().toTime_t()-time);
        // 准备更新
        //emit resultReady(nValue);
        if(false == pull.isEmpty())
        {
            if(0!=pull_port->stop())
            {
                //emit
                emit sig_progress_update(pull, Program::Code::PULL);
                pull="";
            }
        }
        mutex.lock();
        if(false == pull.isEmpty()) pull_list << pull;
        if(pull_list.size()>0)  // 检测设备拔出
        {
//            for(int i = 0; i< pull_list.size();++i)
//            {
//                QString tmp = pull_list.at(i);
//                qDebug()<<"pull_list_" << QString::number(i) << ": " << tmp;
//            }
            pull = pull_list.at(0);
            pull_list.removeOne(pull);
            pull_port->OpenPortDefault(pull);
        }
        mutex.unlock();
        _is_run = 0;
        QThread::msleep(100); // 2Hz
        _is_run = 1;
        if(1!=_scanning)
        {
            emit sig_progress_update(tr("watting... ")+QString::number(QDateTime::currentDateTime().toTime_t()-deadline), Program::Code::SCAN);
            time =QDateTime::currentDateTime().toTime_t();
            continue;
        }
        if(1==_exit) break;
        deadline =QDateTime::currentDateTime().toTime_t();
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            port = info.portName();
            //if(0!=info.description().compare("USB-SERIAL CH340")) continue;
            if(0==port.compare("COM1")) continue;
            if(serach_port(port)) continue;
            if(0!=my_port->OpenPortDefault(port)) continue;
            connect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
            qDebug() << port << "Scanning...";
            emit sig_progress_update(tr("Scanning Device... ")+port, Program::Code::SCAN);
            for(i=0; i<5; i++)
            {
                if(0!=up->__scan()) break;
                //QThread::msleep(2); // 2Hz
            }
            if(0!=up->__b2_scan())
            {
                disconnect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
                my_port->close();
                continue;
            }
            disconnect(my_port, &user_port::errorOccurred, my_port, &user_port::errorOccurred_slots);
            send_idle(port);
            my_port->close();
        }
    }
    disconnect(pull_port, &user_port::errorOccurred, pull_port, &user_port::errorOccurred_slots);
    if(1==_exit) exit(0);
}
#endif
void Scanning::recSerialData()
{
    char datas[128];
    qint64 len=0;
    qint64 count=0;
    //Worker_count++;
    len = my_port->read(datas, sizeof(datas));
    qDebug("recv[%3d]: ", len);
   for(count=0; count<len; count++)
   {
       qDebug("%02X ", datas[count]);
   }
   qDebug("\n");
}

void Scanning::slots_Serial_Free(const QString COM)
{
    //qDebug()<<"Free tmp ="<< COM;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        //qDebug()<<"Free tmp ="<< tmp;
        if(0==COM.compare(tmp))
        {
            port_list.removeOne(COM);
            break;
        }
    }
    pull_list << COM;
    mutex.unlock();
}

void Scanning::send_idle(QString port)
{
    int serach=0;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        //qDebug()<<"tmp ="<< tmp;
        if(0==port.compare(tmp))
        {
            serach = 1;
            break;
        }
    }
    if(0==serach)
    {
        port_list << port;
        emit sig_Serial_Idle(port);
    }
    mutex.unlock();
}

int Scanning::serach_port(QString &port)
{
    int serach=0;
    mutex.lock();
    for(int i = 0; i< port_list.size();++i)
    {
        QString tmp = port_list.at(i);
        if(0==port.compare(tmp))
        {
            serach = 1;
            break;
        }
    }
    for(int i = 0; i< pull_list.size();++i)
    {
        QString tmp = pull_list.at(i);
        if(0==port.compare(tmp))
        {
            serach = 1;
            break;
        }
    }
    mutex.unlock();
    return serach;
}
