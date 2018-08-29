#include "worker.h"
#include "uploader.h"

Worker::Worker(QObject *parent) : QObject(parent)
{

}

void Worker::recSerialData()
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

void Worker::doWork(const QString &parameter)
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
            data[0] = uploader::Code::GET_SYNC;
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
