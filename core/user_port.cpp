/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : serial.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "user_port.h"
#include <QDebug>

user_port::user_port()
{
    //connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(errorOccurred_slots(QSerialPort::SerialPortError)));
    _stop = 0;
}

void user_port::flush_serial()
{
    //串口部分初始化
    //QListView *view = new QListView(this);
    QStandardItemModel *model = new QStandardItemModel(this);
    QStandardItem *item;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name         : " << info.portName();
        qDebug() << "Description  : " << info.description();
        qDebug() << "Manufacturer : " << info.manufacturer();
        qDebug() << "Serial number: " << info.serialNumber();

        item = new QStandardItem(info.portName());
        QString comtips="Name:"+info.portName()+"\r\n"+
                "Description:"+info.description()+"\r\n"+
                "Manufacturer:"+info.manufacturer()+"\r\n"+
                "Serial number:"+info.serialNumber();
        item->setToolTip(comtips);                                  //设置提示信息
        model->appendRow(item);
        //qDebug() << comtips;
        //cbx_com->addItem(info.portName());
    }
}

QStringList user_port::GetPortNames()
{
    QStringList list;

    list.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
#if 0
        qDebug() << "Name         : " << info.portName();
        qDebug() << "Description  : " << info.description();
        qDebug() << "Manufacturer : " << info.manufacturer();
        qDebug() << "Serial number: " << info.serialNumber();
#endif
        list << info.portName();
    }
    qDebug() << "list         : " << list;
}

QString user_port::search_Description(const QString desc)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name         : " << info.portName();
        qDebug() << "Description  : " << info.description();
        qDebug() << "Manufacturer : " << info.manufacturer();
        qDebug() << "Serial number: " << info.serialNumber();

        QString comtips="Name:"+info.portName()+"\r\n"+
                "Description:"+info.description()+"\r\n"+
                "Manufacturer:"+info.manufacturer()+"\r\n"+
                "Serial number:"+info.serialNumber();                                 //设置提示信息
        qDebug() << comtips;
        //cbx_com->addItem(info.portName());
        if(0==desc.compare(info.description())) return info.portName();
    }
    return "";
}

int user_port::OpenPortDefault(const QString &name)
{
    if(isOpen())
    {
        close();
    }
    //QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(recSerialData())); // 连接串口收到数据事件与读取数据函数
    //connect(this, SIGNAL(readyRead()), this, SLOT(_Read())); // 连接串口收到数据事件与读取数据函数
    //serial->setPortName("COM13");
    setPortName(name);
    //setBaudRate(QSerialPort::Baud115200);
    setBaudRate(QSerialPort::Baud57600);
    setParity(QSerialPort::NoParity);
    setDataBits(QSerialPort::Data8);
    setStopBits(QSerialPort::OneStop);
    setFlowControl(QSerialPort::NoFlowControl);
    if (this->open(QIODevice::ReadWrite)) {
        //qDebug()<<"open success" << this->readBufferSize();
        _stop = 0;
        return 0;
    } else {
        //qDebug()<<"open failed";
        _stop = 1;
        return -1;
    }
    //this->setReadBufferSize(2);
    //write("test", 5);
    //my_port = serial;
}

qint64 user_port::_write(const char *data, qint64 len)
{
    if(1==_stop) return 0;
    write(data, len);
}

QByteArray user_port::_readAll()
{
    if(1==_stop) return stop_arr;
    return readAll();
}

void user_port::errorOccurred_slots(QSerialPort::SerialPortError error)
{
    if(QSerialPort::SerialPortError::TimeoutError == error) return;
    if(QSerialPort::SerialPortError::NoError == error) return;
    //qDebug()<<"error:" << error;
    if(QSerialPort::SerialPortError::DeviceNotFoundError == error) _stop=1;
    if(QSerialPort::SerialPortError::ResourceError == error) _stop=1;
}
