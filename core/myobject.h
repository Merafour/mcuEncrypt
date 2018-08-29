#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <QObject>
#include<QDebug>
#include<QThread>
#include<QLabel>

class MyObject : public QObject
{
    Q_OBJECT
public:
    explicit MyObject(QObject *parent = nullptr);
    ~MyObject();
signals:

public slots:
    void Thread1();
    void Thread2(QLabel *lbl);
    void Thread3();
};

#endif // MYOBJECT_H
