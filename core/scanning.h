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

#ifndef SCANNING_H
#define SCANNING_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QStringList>
#include <QMutex>

#include "user_port.h"
#include "uploader.h"

class Scanning : public QObject
{
    Q_OBJECT
public:
    explicit Scanning(QObject *parent = nullptr);
    static uint8_t is_run(void)
    {
        return _is_run;
    }
    static uint8_t stop(void) { return _stop; }
    static void stop(uint8_t _s) { _stop = _s; }
    static void Serial_Free(const QString COM);
    static void scanning(uint8_t _s) { _scanning = _s;}
    static void exit_thread(void) { _exit = 1;}

public slots:
    void doWork(void) ;
    void doWorkEncrypt(void) ;
    void recSerialData();
    void slots_progress_update(const QString &titel, int value)
    {
        emit sig_progress_update(titel, value);
    }
    void slots_Serial_Free(const QString COM);

signals:
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
    void sig_Serial_Idle(const QString COM);
private:
    user_port *my_port;
    user_port *pull_port;
    static QStringList port_list;
    static QStringList pull_list;
    uploader *up;
    static QMutex mutex;
    static uint8_t _is_run;
    static uint8_t _stop;
    static uint8_t _overload;  // 禁止重载
    static uint8_t _scanning;
    static uint8_t _exit;

    int have_data(const uint32_t timeout)
    {
        uint32_t _time;
        for(_time=0; _time<timeout; _time+=10)
        {
            if(my_port->waitForReadyRead(10)) break;
        }
        return my_port->size();
    }
    void send_idle(QString port);
    int serach_port(QString &port);
};

class ControllerScanning : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    ControllerScanning() {
        Scanning *worker = new Scanning;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        //connect(this, &ControllerScanning::operate, worker, &Scanning::doWork);
        connect(this, SIGNAL(operate()), worker, SLOT(doWork()));
        connect(this, SIGNAL(operateEncrypt()), worker, SLOT(doWorkEncrypt()));
        connect(this, &ControllerScanning::sig_Serial_Free, worker, &Scanning::slots_Serial_Free);
        connect(worker, &Scanning::resultReady, this, &ControllerScanning::handleResults);
        connect(worker, &Scanning::sig_progress_update, this, &ControllerScanning::slots_progress_update);
        connect(worker, &Scanning::sig_Serial_Idle, this, &ControllerScanning::slots_Serial_Idle);
        //connect(worker, &Scanning::sig_Serial_Free, this, &ControllerScanning::slots_Serial_Free);
        workerThread.start();
    }
    ~ControllerScanning() {
        workerThread.quit();
        workerThread.wait();
    }
    void start(void)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        emit operate();
        workerThread.setPriority(QThread::TimeCriticalPriority);
    }
    void startEncrypt(void)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        emit operateEncrypt();
        workerThread.setPriority(QThread::TimeCriticalPriority);
    }
    void Serial_Free(const QString COM)
    {
        //qDebug() << "Serial_Free Run Thread : " << QThread::currentThreadId();
        //emit sig_Serial_Free(COM);
        Scanning::Serial_Free(COM);
    }
public slots:
    void handleResults(int value)
    {
        //qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        //qDebug() << "handleResults : " << value;
        emit resultReady(value);
    }
    void slots_progress_update(const QString &titel, int value)
    {
        //qDebug() << "ControllerScanning : " << titel;
        emit sig_progress_update(titel, value);
    }
    void slots_Serial_Idle(const QString COM)
    {
        emit sig_Serial_Idle(COM);
    }
signals:
    void operate(void);
    void operateEncrypt(void);
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
    void sig_Serial_Idle(const QString COM);
    void sig_Serial_Free(const QString COM);
};

#endif // SCANNING_H
