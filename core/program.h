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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "user_port.h"
#include "uploader.h"

class Program : public QObject
{
    Q_OBJECT
public:
    enum Code
    {
        // response codes
        DOWN = 0x0180,
        CRC  = 0x0181,
        SCAN = 0x0182,
        TIME = 0x0183,
        ERR  = 0x0184,
        PULL  = 0x0185,
    };
    enum Bl_Mode{
        USB = 0x00,
        WIFI = 0x01,
        MSC = 0x02,
        Serial = 0x03,
    };
    explicit Program(QObject *parent = nullptr);
    static void set_bl_mode(enum Bl_Mode mode)
    {
        _bl_mode = mode;
    }
    static enum Bl_Mode get_bl_mode(void)
    {
        return _bl_mode;
    }
    static void exit_thread(void) { _exit = 1;}

public slots:
    void doWork(const QString parameter, const Firmware *fw) ;
    void doWorkEncrypt(const QString parameter) ;
    void recSerialData();
    void slots_progress_update(const QString &titel, int value)
    {
        emit sig_progress_update(titel, value);
    }

signals:
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
private:
    user_port *_port;
    uint32_t Worker_count;
    uploader *up;
    //static uint8_t _mavlink;
    static uint8_t _exit;
    static enum Bl_Mode _bl_mode;
    //uploader upload;
};
#if 1
class ControllerProgram : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    ControllerProgram() {
        Program *worker = new Program;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        //connect(this, &ControllerProgram::operate, worker, &Program::doWork);
        connect(this, SIGNAL(operate(QString,const Firmware*)), worker, SLOT(doWork(QString,const Firmware*)));
        connect(this, SIGNAL(operateEncrypt(QString)), worker, SLOT(doWorkEncrypt(QString)));
        connect(worker, &Program::resultReady, this, &ControllerProgram::handleResults);
        connect(worker, &Program::sig_progress_update, this, &ControllerProgram::slots_progress_update);
        workerThread.start();
    }
    ~ControllerProgram() {
        workerThread.quit();
        workerThread.wait();
    }
    void start(const QString COM, const Firmware *fw)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        operate(COM, fw);
    }
    void startEncrypt(const QString COM)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        operateEncrypt(COM);
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
        emit sig_progress_update(titel, value);
    }
signals:
    void operate(const QString, const Firmware *fw);
    void operateEncrypt(const QString);
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
};

#else
class ControllerProgram : public QObject
{
    Q_OBJECT
    QThread workerThread;
    Program *worker;
public:
    ControllerProgram() {
        worker = new Program;
        //worker->moveToThread(&workerThread);
        //connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        //connect(this, &ControllerProgram::operate, worker, &Program::doWork);
        connect(worker, &Program::resultReady, this, &ControllerProgram::handleResults);
        connect(worker, &Program::sig_progress_update, this, &ControllerProgram::slots_progress_update);
        workerThread.start();
    }
    ~ControllerProgram() {
        workerThread.quit();
        workerThread.wait();
    }
    void start(const QString &COM, const Firmware *fw)
    {
        qDebug() << "Worker Run Thread : " << QThread::currentThreadId();
        //operate(COM, fw);
        worker->doWork(COM, fw);
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
        emit sig_progress_update(titel, value);
    }
signals:
    void operate(const QString &, const Firmware *fw);
    void resultReady(int value);
    void sig_progress_update(const QString &titel, int value);
};
#endif

#endif // PROGRAM_H
