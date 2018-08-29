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

#ifndef USER_PORT_H
#define USER_PORT_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QListView>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>

class user_port : public QSerialPort
{
public:
    user_port();

    void flush_serial(void);
    static QStringList GetPortNames(void);
    static QString search_Description(const QString desc);
    int OpenPortDefault(const QString &name);
    uint8_t stop(void) { return _stop; }

    qint64 _write(const char *data, qint64 len);
    QByteArray _readAll();

public slots:
    void errorOccurred_slots(QSerialPort::SerialPortError error);

private:
    uint8_t _stop;
    QByteArray stop_arr;

};

#endif // USER_PORT_H
