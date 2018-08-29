#include "myobject.h"

MyObject::MyObject(QObject *parent) : QObject(parent)
{

}
MyObject::~MyObject()
{

}

void MyObject::Thread1()
{

    qDebug()<<QThread::currentThreadId();
}
void MyObject::Thread2(QLabel *lbl)
{

     for(int i=0;i<1000000000;i++)
     {}
     lbl->setText("aaaaa");
    qDebug()<<QThread::currentThreadId();
}
void MyObject::Thread3()
{
    qDebug()<<QThread::currentThreadId();
}
