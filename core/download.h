/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : download UI.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTreeWidget>
#include <QTextEdit>
#include <QCloseEvent>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QListView>
#include <QStandardItemModel>
#include <QThread>
#include <QSettings>
#include <QTranslator>
#include "core/form_download.h"

#include "user_port.h"
#include "program.h"
#include "scanning.h"

namespace Ui {
class DownLoad;
}

class DownLoad : public QMainWindow
{
    Q_OBJECT

public:
    explicit DownLoad(QWidget *parent = 0);
    ~DownLoad();

private slots:
    void on_Button_Loadfile_clicked();

    void on_Button_Download_clicked();

    void slots_progress_update(const QString &titel, int value);
    void slots_Serial_Idle(const QString COM);

    void on_checkBox_mul_stateChanged(int arg1);

    void actionLanguage_triggered();
    void actionMode_triggered();
    void actionHelp_triggered();

private:
    Ui::DownLoad *ui;
    void open_firmware(void);
    void download(QString COM);
    int idle(QString COM);
    uint8_t busy(void)
    {
        return _busy;
    }
    void create_menu(void);
    void multiple(const uint8_t checked);
    void mode_setting(void);
    void Language_cutover(void);
#define DOWNLOAD_FORM_SIZE  10
    user_port *my_port;
    QString fileName;
    Firmware *fw;
    QString qss_default;
    ControllerProgram *_program;
    ControllerScanning *_scanning;
    uint8_t _download;
    uint8_t _busy;
    QString _COM;
    uint8_t _Multiple;
    Form_Download *_download_form[DOWNLOAD_FORM_SIZE];

    //QTranslator translator;
    user_port *_port;
    uploader *up;

    // menu
    QMenu *Menu_Mode;
    QAction *actionMultiple;
    QAction *actionSingle;
    QAction *actionWIFI;
    QAction *actionUSB;
    QAction *actionMSC;
    QAction *actionSerial;
    QMenu *Menu_Help;
    QAction *actionAbout;

    // setting
#define Setting_Language     "Language"
#define Setting_Mul          "Multiple"
#define Setting_USB          "USB"

    QString _Title;
};

#endif // DOWNLOAD_H
