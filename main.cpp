/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : main.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "mainwindow.h"
#include <QApplication>

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
//}
#include <QApplication>
#include <QProcess>
#include "core/download.h"
#if 0
#define DEBUG_INFO_FILE  1
#else
#define DEBUG_INFO_FILE  0
#endif
#if DEBUG_INFO_FILE
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString txt=str;
#else
void customMessageHandler(QtMsgType type, const char *msg)
{
    QString txt(msg);
#endif
    QFile outFile("debug.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if DEBUG_INFO_FILE
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qInstallMessageHandler(customMessageHandler);
#else
    qInstallMsgHandler(customMessageHandler);
#endif
#endif

#if 1
    MainWindow w;
    w.show();
#else
    DownLoad w;
    w.show();
#endif
#if 0
    return a.exec();
#else
    int ret = a.exec();
    if(7714==ret)
    {
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
        return 0;
    }
    return ret;
#endif
}
