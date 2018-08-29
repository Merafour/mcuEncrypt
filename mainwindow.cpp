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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QDebug>
#include <QTranslator>
#include "core/mydialog.h"
#include "core/tea.h"

static char buffer[128];
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    bool ok;
    uint32_t count;
    uint32_t i=0;
    ui->setupUi(this);

    count = setting.value("count").toInt(&ok);
    qDebug() << "count:" << count;
    setting.setValue("count", QString::number(++count));
    setWindowFlags(Qt::WindowCloseButtonHint);
    setToolTip("Encrypt");

    _encrypt=0;
    _busy = 0;
    _Encrypt_form[0] = ui->widget_D1;
    _Encrypt_form[1] = ui->widget_D2;
    _Encrypt_form[2] = ui->widget_D3;
    _Encrypt_form[3] = ui->widget_D4;
    _Encrypt_form[4] = ui->widget_D5;
    _Encrypt_form[5] = ui->widget_D6;
    _Encrypt_form[6] = ui->widget_D7;
    _Encrypt_form[7] = ui->widget_D8;
    _Encrypt_form[8] = ui->widget_D9;
    _Encrypt_form[9] = ui->widget_D10;
    Language_cutover();
    for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
    {
        _Encrypt_form[i]->set_title(tr("Encrypt Thread") + " " +QString::number(i+1));
    }
    create_menu();
    mode_setting();
    ui->m_progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 对齐方式
    ui->m_progressBar->setFormat(QString(tr("watting...  ")));
    qss_default = ui->m_progressBar->styleSheet();
    ui->m_progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 对齐方式
    ui->m_progressBar->setFormat(QString(tr("watting...  ")));
    qss_default = ui->m_progressBar->styleSheet();
    ui->Button_Encrypt->setText(tr("Encrypt"));
    //layout->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();
    setFixedSize(this->width(), this->height());
    //ui->groupBox_download->setFixedSize(ui->groupBox_download->width(), ui->groupBox_download->height());
    //setFixedSize(this->width(), this->height());
    setWindowFlags(windowFlags() &~ Qt::WindowMaximizeButtonHint);
//    _Multiple = 0;
//    multiple(_Multiple);
    _program = new ControllerProgram();
    //connect(_program, &ControllerProgram::resultReady, this, handleResults);
    //connect(_program, SIGNAL(resultReady(int)), this, SLOT(handleResults(int)));
    connect(_program, SIGNAL(sig_progress_update(const QString &, int)), this, SLOT(slots_progress_update(const QString &, int)));
    _scanning = new ControllerScanning();
    connect(_scanning, SIGNAL(sig_Serial_Idle(const QString)), this, SLOT(slots_Serial_Idle(const QString)));
    connect(_scanning, SIGNAL(sig_progress_update(const QString &, int)), this, SLOT(slots_progress_update(const QString &, int)));
    _scanning->Serial_Free("COM6");
    //_scanning->start();
    _scanning->startEncrypt();
    connect(ui->actionEnglish, SIGNAL(triggered()), this, SLOT(actionLanguage_triggered()));
    connect(ui->actionSimplified_Chinese, SIGNAL(triggered()), this, SLOT(actionLanguage_triggered()));
    memset(buffer, 0, sizeof(buffer));
    memset(buffer, 'a', sizeof(buffer)-1);
    //memset(buffer_d, 0, sizeof(buffer_d));
    //set_Iteration(32);
    qDebug() << "buffer1:" << QString::fromLocal8Bit(buffer);
    for(i=0; i<sizeof(buffer); i+=8)
    {
        tea_encrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
        tea_encrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
        tea_encrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
        tea_encrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
    }
    //qDebug() << "buffer2:" << QString::fromLocal8Bit(buffer);
    //tea_decrypt((uint32_t*)buffer, (const uint32_t*)tea_key);
    for(i=0; i<sizeof(buffer); i+=8)
    {
        tea_decrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
        tea_decrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
        tea_decrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
        tea_decrypt((uint32_t*)(&buffer[i]), (const uint32_t*)tea_key);
    }
    qDebug() << "buffer3:" << QString::fromLocal8Bit(buffer);
//    uploader::random_init();
//    qDebug() << "random:" << QString::number(qrand());
    //qDebug() << "buffer_d:" << buffer_d;
}

MainWindow::~MainWindow()
{
    Scanning::exit_thread();
    Program::exit_thread();
    QThread::msleep(300);
    delete ui;
}

void MainWindow::on_Button_Encrypt_clicked()
{
    if(_encrypt)
    {
        ui->Button_Encrypt->setText(tr("Encrypt"));
        ui->menuLanguage->setEnabled(true);
        _encrypt = 0;
        Scanning::scanning(0);
        return;
    }
    ui->Button_Encrypt->setText(tr("STOP"));
    ui->menuLanguage->setEnabled(false);
    _encrypt = 1;
    Scanning::scanning(1);
    qDebug()<<QThread::currentThreadId();
}

void MainWindow::slots_progress_update(const QString &titel, int value)
{
    switch (value) {
    case Program::Code::PULL:
        qDebug()<< "pull out:" << titel;
//        ui->m_progressBar->setStyleSheet(qss_default);
//        ui->m_progressBar->setFormat(titel);
        idle(titel);
        for(int i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            _Encrypt_form[i]->idle(titel);
        }
        break;
    case Program::Code::ERR:
        ui->m_progressBar->setValue(0);
        ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n" "border-color: rgb(255, 255, 255);"));
        break;
    case Program::Code::SCAN:
        //ui->label_watting->setText(titel);
        ui->m_progressBar->setFormat(titel);
        break;
    case Program::Code::DOWN:
        Scanning::Serial_Free(titel); // 释放串口资源
        _busy = 0;
        break;
    case Program::Code::TIME:
        ui->m_progressBar->setFormat(this->ui->m_progressBar->text()+titel);
        break;
    default:
        //this->ui->label_watting->setText(titel);
        this->ui->m_progressBar->setValue(value);
        //QString str = titel+QString::number(value)+"%";
        //ui->m_progressBar->setFormat(str);
        ui->m_progressBar->setFormat(titel);
        break;
    }
}

void MainWindow::slots_Serial_Idle(const QString COM)
{
    qDebug() << "Serial Idle: " << COM;
    if(_encrypt)
    {
        uint8_t i=0;
#if 0
        if(0==busy())
        {
            encrypt(COM);
            return;
        }
#endif
        if(1!=_Multiple)
        {
            if(0==busy())
            {
                encrypt(COM);
                return;
            }
            else
                _scanning->Serial_Free(COM);
            return;
        }
        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            if(0==_Encrypt_form[i]->busy())
            {
                _Encrypt_form[i]->encrypt(COM);
                return;
            }
        }
    }
    else
    {
        _scanning->Serial_Free(COM);
    }
}

void MainWindow::encrypt(QString COM)
{
    _program->startEncrypt(COM);
    //_program->start("COM13");
    qDebug()<<QThread::currentThreadId();
    ui->groupBox_Encrypt->setTitle(COM+_Title);
    ui->m_progressBar->setStyleSheet(qss_default);
    _busy = 1;
    _COM = COM;
}

int MainWindow::idle(QString COM)
{
    if(0==_COM.compare(COM))
    {
        ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 0);\n" "border-color: rgb(255, 255, 255);"));
        ui->groupBox_Encrypt->setTitle(_Title);
        ui->m_progressBar->setValue(0);
        return 0;
    }
    return -1;
}

void MainWindow::create_menu()
{
    actionMultiple = new QAction("Multiple");
    actionMultiple->setObjectName(QStringLiteral("actionMultiple"));
    actionMultiple->setCheckable(true);
    actionSingle = new QAction("Single");
    actionSingle->setObjectName(QStringLiteral("actionSingle"));
    actionSingle->setCheckable(true);
    actionUSB = new QAction("USB");
    actionUSB->setObjectName(QStringLiteral("actionUSB"));
    actionUSB->setCheckable(true);
    actionWIFI = new QAction("WIFI");
    actionWIFI->setObjectName(QStringLiteral("actionWIFI"));
    actionWIFI->setCheckable(true);
    actionMSC = new QAction("MSC");
    actionMSC->setObjectName(QStringLiteral("actionMSC"));
    actionMSC->setCheckable(true);
    actionSerial = new QAction("Serial");
    actionSerial->setObjectName(QStringLiteral("actionSerial"));
    actionSerial->setCheckable(true);

    actionMSC->setEnabled(false);
    actionWIFI->setEnabled(false);

    Menu_Mode = new QMenu("Mode");
    Menu_Mode->setObjectName(QStringLiteral("actionMode"));

    Menu_Mode->addAction(actionMultiple);
    Menu_Mode->addAction(actionSingle);
    Menu_Mode->addSeparator();
    Menu_Mode->addAction(actionUSB);
    Menu_Mode->addAction(actionWIFI);
    Menu_Mode->addAction(actionMSC);
    Menu_Mode->addAction(actionSerial);
    ui->menuBar->addAction(Menu_Mode->menuAction());
    //ui->menubar->addMenu(Menu_Mode);

    connect(actionMultiple, SIGNAL(triggered()), this, SLOT(actionMode_triggered()));
    connect(actionSingle, SIGNAL(triggered()), this, SLOT(actionMode_triggered()));
    connect(actionWIFI, SIGNAL(triggered()), this, SLOT(actionMode_triggered()));
    connect(actionUSB, SIGNAL(triggered()), this, SLOT(actionMode_triggered()));
    connect(actionMSC, SIGNAL(triggered()), this, SLOT(actionMode_triggered()));
    connect(actionSerial, SIGNAL(triggered()), this, SLOT(actionMode_triggered()));

    // text
    actionMultiple->setText(tr("Multiple"));
    actionSingle->setText(tr("Single"));
//    actionUSB->setText(tr("USB"));
//    actionWIFI->setText(tr("WIFI"));
//    actionMSC->setText(tr("MSC"));
//    actionSerial->setText(tr("Serial"));
    Menu_Mode->setTitle(tr("Mode"));
//    Menu_Mode->setTitle(QApplication::translate("DownLoad", "Mode", nullptr));
//    actionMultiple->setText(QApplication::translate("DownLoad", "Multiple", nullptr));
//    actionSingle->setText(QApplication::translate("DownLoad", "Single", nullptr));
    actionAbout = new QAction("About");
    actionAbout->setObjectName(QStringLiteral("actionAbout"));
    actionAbout->setCheckable(false);
    Menu_Help = new QMenu("Help");
    Menu_Help->setObjectName(QStringLiteral("actionHelp"));
    Menu_Help->addAction(actionAbout);
    ui->menuBar->addAction(Menu_Help->menuAction());
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(actionHelp_triggered()));
}

void MainWindow::multiple(const uint8_t checked)
{
    if(checked)
    {
        uint8_t i=0;
        _Multiple = 1;
        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            _Encrypt_form[i]->setEnabled(true);
            _Encrypt_form[i]->set_title(_Encrypt_form[i]->get_title());
        }
    }
    else
    {
        uint8_t i=0;
        _Multiple = 0;
        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            _Encrypt_form[i]->setEnabled(false);
            _Encrypt_form[i]->set_title(tr("Disable"));
        }
    }
    this->adjustSize();
}

void MainWindow::mode_setting()
{
    QString mode;
    QString usb;
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    mode = setting.value(Setting_Mul).toString();
    usb = setting.value(Setting_USB).toString();
    qDebug() << "mode: " << mode;
    qDebug() << "usb: " << usb;
    // Multiple
    _Title = " [";
    if(0==mode.compare(actionMultiple->text())) { // "Multiple"
        multiple(1);
        actionMultiple->setChecked(true);
        actionSingle->setChecked(false);
        _Title = _Title + tr("Multiple-");
    }
    else // "Single" or
    {
        multiple(0);
        actionMultiple->setChecked(false);
        actionSingle->setChecked(true);
        _Title = _Title + tr("Single-");
    }
    // USB
    if (0==usb.compare(actionWIFI->text())) { // "WIFI"
        Program::set_bl_mode(Program::Bl_Mode::WIFI);
        actionUSB->setChecked(false);
        actionWIFI->setChecked(true);
        actionMSC->setChecked(false);
        actionSerial->setChecked(false);
        _Title = _Title + tr("-WIFI");
    } else if (0==usb.compare(actionMSC->objectName())) { // "MSC"
        Program::set_bl_mode(Program::Bl_Mode::MSC);
        actionUSB->setChecked(false);
        actionWIFI->setChecked(false);
        actionMSC->setChecked(true);
        actionSerial->setChecked(false);
        _Title = _Title + tr("-MSC");
    }
    // Serial
    else if (0==usb.compare(actionSerial->text())) { // "Serial"
           Program::set_bl_mode(Program::Bl_Mode::Serial);
           actionUSB->setChecked(false);
           actionWIFI->setChecked(false);
           actionMSC->setChecked(false);
           actionSerial->setChecked(true);
           _Title = _Title + tr("-Serial");
    }
    else // "USB" or
    {
        Program::set_bl_mode(Program::Bl_Mode::USB);
        actionUSB->setChecked(true);
        actionWIFI->setChecked(false);
        actionMSC->setChecked(false);
        actionSerial->setChecked(false);
        _Title = _Title + tr("-USB");
    }
    _Title = _Title + "] ";
    _Title = ui->groupBox_Encrypt->title()+_Title;
    //ui->groupBox_download->setTitle(ui->groupBox_download->title()+_Title);
    ui->groupBox_Encrypt->setTitle(_Title);
}

void MainWindow::Language_cutover(void)
{
    QTranslator translator;
    bool load=false;
    QString _language;
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    _language = setting.value(Setting_Language).toString();
    qDebug() << "Language: " << _language;
    if(_language.isEmpty())
    {
        _language = "English";
    }
    if(0==_language.compare(ui->actionSimplified_Chinese->text())) {
        load=translator.load("cn.qm");
        ui->actionEnglish->setChecked(false);
        ui->actionSimplified_Chinese->setChecked(true);
    }
    //else if (0==_language.compare("English"))
    else // English
    {
        load=translator.load("en.qm");
        ui->actionEnglish->setChecked(true);
        ui->actionSimplified_Chinese->setChecked(false);
    }
    if(true==load)
    {
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
        uint8_t i=0;
        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            _Encrypt_form[i]->Language(translator);
        }
    }
    ui->menuLanguage->setEnabled(true);
    ui->menuLanguage->setVisible(true);
}

void MainWindow::actionLanguage_triggered()
{
    //QTranslator translator;

    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QAction *action = (QAction*)sender();
    QString actionText = action->text();
    setting.setValue(Setting_Language, actionText);
    qApp->exit(7714);
}

void MainWindow::actionMode_triggered()
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QAction *action = (QAction*)sender();
    QString objectName = action->objectName();
    qDebug()<<"objectName:"<<objectName<<endl;
    //setting.setValue(Setting_Mul, actionText);
    // Multiple
    if(0==objectName.compare(actionMultiple->objectName())) {
        setting.setValue(Setting_Mul, actionMultiple->text()); // "Multiple"
    }
    //else if (0==objectName.compare("actionSingle"))
    else // "actionSingle"
    {
        setting.setValue(Setting_Mul, actionSingle->text()); // "Single"
    }
    // USB
    if (0==objectName.compare(actionWIFI->objectName())) {
        setting.setValue(Setting_USB, actionWIFI->text()); // "WIFI"
    } else if (0==objectName.compare(actionMSC->objectName())) {
        setting.setValue(Setting_USB, actionMSC->text()); // "MSC"
    }
    else if (0==objectName.compare(actionSerial->objectName())) {
           setting.setValue(Setting_USB, actionSerial->text()); // "Serial"
    }
    else
    {
        setting.setValue(Setting_USB, "USB");
    }
    qApp->exit(7714);
}

void MainWindow::actionHelp_triggered()
{
    QAction *action = (QAction*)sender();
    QString objectName = action->objectName();
    qDebug()<<"objectName:"<<objectName<<endl;
    if(0==objectName.compare("actionAbout")) {
        MyDialog _about;
        _about.setWindowModality(Qt::ApplicationModal);
        _about.exec();
    }
}

