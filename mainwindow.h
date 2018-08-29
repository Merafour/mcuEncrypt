/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : form main.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "core/user_port.h"
#include "core/program.h"
#include "core/scanning.h"
#include "form_encrypt.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Button_Encrypt_clicked();


    void slots_progress_update(const QString &titel, int value);
    void slots_Serial_Idle(const QString COM);
    void actionLanguage_triggered();
    void actionMode_triggered();
    void actionHelp_triggered();
private:
    Ui::MainWindow *ui;


    void encrypt(QString COM);
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
    QString qss_default;
    ControllerProgram *_program;
    ControllerScanning *_scanning;
    uint8_t _encrypt;
    uint8_t _busy;
    QString _COM;
    uint8_t _Multiple;
    Form_Encrypt *_Encrypt_form[DOWNLOAD_FORM_SIZE];

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

#endif // MAINWINDOW_H
