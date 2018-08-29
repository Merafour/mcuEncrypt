#include "serial.h"
#include <QMessageBox>
#include <QDebug>
#include <QThread>

Serial::Serial(QObject *parent) : QObject(parent)
{
    port = new QSerialPort();
    port->setPortName("COM1");
    if(!port->open(QSerialPort::ReadWrite))
    {
        QMessageBox WrrMsg;
        WrrMsg.setInformativeText("无法打开该串口");
        WrrMsg.show();
        WrrMsg.exec();
    }
    port->setBaudRate(QSerialPort::Baud19200,QSerialPort::AllDirections);   // 19200,N,8,1
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);
    connect(port, SIGNAL(readyRead()), this, SLOT(readData()), Qt::DirectConnection);   // 注意，真正执行时 port 与 Serial 在同一个线程中，因此使用 Qt::DirectConnection。
}

Serial::~Serial(void)
{
}
void Serial::readData(void)
{
    qDebug()<< "Reading Data...ID is:" << QThread::currentThreadId();
    port->clear(QSerialPort::AllDirections);
}
void Serial::threadStarted(void)
{
    qDebug()<< "Thread has started...ID is:" << QThread::currentThreadId();
}
void Serial::threadFinished(void)
{
    qDebug()<< "Closing COM port...ID is:" << QThread::currentThreadId();
    if(port->isOpen())
    {
        port->close();      // 关闭串口。
    }
}





