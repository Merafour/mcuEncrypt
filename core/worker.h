#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "user_port.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);



public slots:
    void doWork(const QString &parameter) ;
//    {
//        //QByteArray tempdata;
//        //user_port *_port;
//        char data[16];
//        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
//        int nValue = 0;
//        //_port = new user_port();
//        //_port->OpenPortDefault(COM);
//        while ( (nValue < 100))
//        {
//            // 休眠50毫秒
//            ++nValue;
//    //        tempdata.clear();
//    //        tempdata.append(uploader::Code::GET_SYNC);
//    //        tempdata.append(uploader::Code::EOC);
//    //        if(_port->isOpen()) _port->write(tempdata);
////            data[0] = uploader::Code::GET_SYNC;
////            data[1] = uploader::Code::EOC;
////            _port->write(data, 2);
//            // 准备更新
//            emit resultReady(nValue);
//            QThread::msleep(50);
//        }
//    }
    void recSerialData();

signals:
    void resultReady(int value);
private:
    user_port *_port;
    uint32_t Worker_count;
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller() {
        Worker *worker = new Worker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &Controller::operate, worker, &Worker::doWork);
        connect(worker, &Worker::resultReady, this, &Controller::handleResults);
        workerThread.start();
    }
    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }
    void start(const QString &COM)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        operate(COM);
    }
public slots:
    void handleResults(int value)
    {
        //qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        //qDebug() << "handleResults : " << value;
        emit resultReady(value);
    }
signals:
    void operate(const QString &);
    void resultReady(int value);
};

#endif // WORKER_H
