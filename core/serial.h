#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class Serial : public QObject
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = nullptr);
    ~Serial(void);
    QSerialPort *port;
signals:

public slots:
    void readData(void);
    void threadStarted(void);
    void threadFinished(void);
};

#endif // SERIAL_H
