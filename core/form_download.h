/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : download thread.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#ifndef FORM_DOWNLOAD_H
#define FORM_DOWNLOAD_H

#include <QWidget>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QListView>
#include <QStandardItemModel>
#include <QThread>
#include <QSettings>
#include <QString>
#include <QTranslator>

#include "user_port.h"
#include "uploader.h"
#include "program.h"
namespace Ui {
class Form_Download;
}

class Form_Download : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Download(QWidget *parent = 0);
    ~Form_Download();

    void set_title(QString title);
    QString get_title(void)
    {
        return _title;
    }
    void set_path(QString _path);
    void download(QString COM);
    int idle(QString COM);
    uint8_t busy(void)
    {
        return _busy;
    }
    void Language(QString actionText);

private slots:
    void on_Button_Loadfile_clicked();

    void on_Button_Download_clicked();

    void handleResults(int value);
    void slots_progress_update(const QString &titel, int value);

private:
    Ui::Form_Download *ui;

    void open_firmware(void);
    QString fileName;
    Firmware *fw;
    ControllerProgram *_program;
    uint8_t _busy;
    QString _title;
    QString qss_default;
    QTranslator translator;
};

#endif // FORM_DOWNLOAD_H
