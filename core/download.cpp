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

#include "download.h"
#include "ui_download.h"
#include "mydialog.h"


DownLoad::DownLoad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DownLoad)
{
    //QTranslator translator;
    uint8_t i=0;
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    bool ok;
    uint32_t count;
    ui->setupUi(this);
    count = setting.value("count").toInt(&ok);
    qDebug() << "count:" << count;
    setting.setValue("count", QString::number(++count));
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->menuLanguage->setEnabled(false);
    //ui->menubar->setVisible(false);
    my_port=new user_port();
    my_port->flush_serial();
    fw = new Firmware;
    _download=0;
    _busy = 0;
    _download_form[0] = ui->widget_D1;
    _download_form[1] = ui->widget_D2;
    _download_form[2] = ui->widget_D3;
    _download_form[3] = ui->widget_D4;
    _download_form[4] = ui->widget_D5;
    _download_form[5] = ui->widget_D6;
    _download_form[6] = ui->widget_D7;
    _download_form[7] = ui->widget_D8;
    _download_form[8] = ui->widget_D9;
    _download_form[9] = ui->widget_D10;
    //create_menu();
    Language_cutover();
    for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
    {
        _download_form[i]->set_title(tr("Download Thread") + " " +QString::number(i+1));
    }
    create_menu();
    mode_setting();
    ui->Button_Download->setText(tr("Download"));
    ui->Button_Loadfile->setText(tr("Loadfile"));
#if 1
    ui->checkBox_mav->setChecked(false);
    ui->checkBox_mav->setVisible(false);
#else
    ui->checkBox_mav->setChecked(true);
    ui->checkBox_mav->setVisible(true);
#endif
    ui->checkBox_mav->setText(tr("safe"));
    ui->checkBox_mav->setEnabled(false);
    ui->checkBox_mul->setVisible(false);
//    if(ui->checkBox_mav->isChecked())
//    {
//        Program::set_mavlink(1);
//    }
//    else
//    {
//        Program::set_mavlink(0);
//    }
    ui->m_progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 对齐方式
    ui->m_progressBar->setFormat(QString(tr("watting...  ")));
    qss_default = ui->m_progressBar->styleSheet();
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
    _scanning->start();

//    _port = new user_port();
//    up = new uploader_safe(this);
//    up->setting(_port, "");
//    if(0!=_port->OpenPortDefault("COM13"))
//    {
//        qDebug()<<"open failed";
//    }
#if 1
    connect(ui->actionEnglish, SIGNAL(triggered()), this, SLOT(actionLanguage_triggered()));
    connect(ui->actionSimplified_Chinese, SIGNAL(triggered()), this, SLOT(actionLanguage_triggered()));
//    if(translator.load("cn.qm"))
//    {
//        ui->actionEnglish->setChecked(false);
//        ui->actionSimplified_Chinese->setChecked(true);
//        qApp->installTranslator(&translator);
//        ui->retranslateUi(this);
//        //uint8_t i=0;
//        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
//        {
//            _download_form[i]->Language("Simplified Chinese");
//        }
//        ui->menuLanguage->setEnabled(true);
//    }
#endif
    //Language_cutover();
}

DownLoad::~DownLoad()
{
    //delete ui;
    Scanning::exit_thread();
    Program::exit_thread();
    QThread::msleep(100);
    delete ui;
}

void DownLoad::on_Button_Loadfile_clicked()
{
    open_firmware();
}

void DownLoad::on_Button_Download_clicked()
{
#if 0
    uint8_t buf[2];
    buf[0] = uploader::Code::GET_SYNC;
    //buf[0] = uploader::Code::GET_CRC;
    //buf[0] = uploader::Code::CHIP_ERASE;
    buf[1] = uploader::Code::EOC;
    up->clear();
    QThread::msleep(1);
    up->__send(buf, 2);
    //QThread::msleep(1);
    up->have_data(100);
    up->__getSync();
#endif
    //up->identify();
    //up->__erase();
#if 1
    if(fileName.isEmpty()) // open
    {
        open_firmware();
        if(fileName.isEmpty()) return;
    }
    if(_download)
    {
//        translator.load("en.qm");
//        qApp->installTranslator(&translator);
//        ui->retranslateUi(this);
        ui->Button_Download->setText(tr("Download"));
        ui->Button_Loadfile->setEnabled(true);
        //ui->checkBox_mav->setEnabled(true);
        ui->checkBox_mul->setEnabled(true);
        ui->menuLanguage->setEnabled(true);
        _download = 0;
        Scanning::scanning(0);
//        Scanning::stop(1);
        return;
    }
//    Scanning::stop(0);
//    _scanning->start();
    ui->Button_Download->setText(tr("STOP"));
    ui->Button_Loadfile->setEnabled(false);
    //ui->checkBox_mav->setEnabled(false);
    ui->checkBox_mul->setEnabled(false);
    ui->menuLanguage->setEnabled(false);
//    if(ui->checkBox_mav->isChecked())
//    {
//        Program::set_mavlink(1);
//    }
//    else
//    {
//        Program::set_mavlink(0);
//    }
    _download = 1;
    Scanning::scanning(1);
    qDebug()<<QThread::currentThreadId();
    //_download_form[0]->download("COM13");
#endif
}

void DownLoad::slots_progress_update(const QString &titel, int value)
{
//    switch (value) {
//    case Program::Code::DOWN:
//        _scanning->Serial_Free(titel); // 释放串口资源
//        _busy = 0;
//        break;
//    case Program::Code::SCAN:
//        ui->label_watting->setText(titel);
//        break;
//    default:
//        this->ui->label_watting->setText(titel);
//        this->ui->m_progressBar->setValue(value);
//        break;
//    }
    switch (value) {
    case Program::Code::PULL:
        qDebug()<< "pull out:" << titel;
//        ui->m_progressBar->setStyleSheet(qss_default);
//        ui->m_progressBar->setFormat(titel);
        idle(titel);
        for(int i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            _download_form[i]->idle(titel);
        }
        break;
    case Program::Code::ERR:
        ui->m_progressBar->setValue(0);
        ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n" "border-color: rgb(255, 255, 255);"));
        break;
    case Program::Code::SCAN:
        ui->label_watting->setText(titel);
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

void DownLoad::slots_Serial_Idle(const QString COM)
{
    qDebug() << "Serial Idle: " << COM;
    if(_download)
    {
        uint8_t i=0;
#if 1
        if(0==busy())
        {
            download(COM);
            return;
        }
#endif
        if(1!=_Multiple)
        {
//            if(0==busy())
//            {
//                download(COM);
//                return;
//            }
//            else
                _scanning->Serial_Free(COM);
            return;
        }
        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            if(0==_download_form[i]->busy())
            {
                _download_form[i]->download(COM);
                return;
            }
        }
    }
    else
    {
        _scanning->Serial_Free(COM);
    }
}

void DownLoad::open_firmware()
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
    QString lastPath = setting.value("LastFilePath").toString();
    //打开传输的文件
    //QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::currentPath(),tr("HEX File(*.hex);;BIN File(*.bin);;ALL File (*.*)"));
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),lastPath,tr("BIN File(*.bin);;HEX File(*.hex)"));
    if (fileName.isEmpty())
        return;
    setting.setValue("LastFilePath", fileName);
    ui->label_file->setText(fileName);
    fw->ProcessFirmware(fileName);
//    ui->widget_1->set_path(fileName);
//    ui->widget_2->set_path(fileName);
    uint8_t i=0;
    for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
    {
        _download_form[i]->set_path(fileName);
    }
}

void DownLoad::download(QString COM)
{
    _program->start(COM, fw);
    //_program->start("COM13");
    qDebug()<<QThread::currentThreadId();
    ui->groupBox_download->setTitle(COM+_Title);
    ui->m_progressBar->setStyleSheet(qss_default);
    _busy = 1;
    _COM = COM;
}

int DownLoad::idle(QString COM)
{
    if(0==_COM.compare(COM))
    {
        ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 0);\n" "border-color: rgb(255, 255, 255);"));
        ui->groupBox_download->setTitle(_Title);
        ui->m_progressBar->setValue(0);
        return 0;
    }
    return -1;
}

void DownLoad::create_menu()
{
#if 0
    ProjectMenu=new QMenu(tr("工程"));
    act_project_creat=new QAction(tr("新建"));
    act_project_open=new QAction(tr("打开"));
    act_project_save=new QAction(tr("保存"));
    act_project_close=new QAction(tr("关闭"));
    ProjectMenu->addAction(act_project_creat);
    ProjectMenu->addAction(act_project_open);
    ProjectMenu->addSeparator();
    ProjectMenu->addAction(act_project_save);
    ProjectMenu->addAction(act_project_close);
    ui->menubar->addMenu(ProjectMenu);
#endif
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
    ui->menubar->addAction(Menu_Mode->menuAction());
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
    ui->menubar->addAction(Menu_Help->menuAction());
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(actionHelp_triggered()));
}

void DownLoad::multiple(const uint8_t checked)
{
    if(checked)
    {
        uint8_t i=0;
        _Multiple = 1;
        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            //_download_form[i]->setVisible(true);
            _download_form[i]->setEnabled(true);
            //_download_form[i]->show();
            //_download_form[i]->adjustSize();
            _download_form[i]->set_title(_download_form[i]->get_title());
        }
    }
    else
    {
        uint8_t i=0;
        _Multiple = 0;
        for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
        {
            //_download_form[i]->setVisible(false);
            _download_form[i]->setEnabled(false);
            //_download_form[i]->hide();
            //_download_form[i]->adjustSize();
            _download_form[i]->set_title(tr("Disable"));
        }
    }
    //ui->groupBox_download->adjustSize();
    this->adjustSize();
    //setFixedSize(this->width(), this->height());
}

void DownLoad::mode_setting()
{
#define Multiple_Visible  0
    QString mode;
    QString usb;
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    mode = setting.value(Setting_Mul).toString();
    usb = setting.value(Setting_USB).toString();
    qDebug() << "mode: " << mode;
    qDebug() << "usb: " << usb;
    // Multiple
    _Title = " [";
    if(0==mode.compare("Multiple")) {
        multiple(1);
        actionMultiple->setChecked(true);
        actionSingle->setChecked(false);
        _Title = _Title + tr("Multiple-");
#if Multiple_Visible
        ui->widget_Mul->setVisible(true);
        ui->verticalLayout_5->addWidget(ui->widget_Mul);
#endif
    }
    else // "Single" or
    {
        multiple(0);
        actionMultiple->setChecked(false);
        actionSingle->setChecked(true);
        _Title = _Title + tr("Single-");
#if Multiple_Visible
        ui->widget_Mul->setVisible(false);
#endif
    }
    // USB
    if (0==usb.compare("WIFI")) {
        Program::set_bl_mode(Program::Bl_Mode::WIFI);
        actionUSB->setChecked(false);
        actionWIFI->setChecked(true);
        actionMSC->setChecked(false);
        actionSerial->setChecked(false);
        _Title = _Title + tr("-WIFI");
    } else if (0==usb.compare("MSC")) {
        Program::set_bl_mode(Program::Bl_Mode::MSC);
        actionUSB->setChecked(false);
        actionWIFI->setChecked(false);
        actionMSC->setChecked(true);
        actionSerial->setChecked(false);
        _Title = _Title + tr("-MSC");
    }
    // Serial
    else if (0==usb.compare("Serial")) {
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
    _Title = ui->groupBox_download->title()+_Title;
    //ui->groupBox_download->setTitle(ui->groupBox_download->title()+_Title);
    ui->groupBox_download->setTitle(_Title);
}

void DownLoad::Language_cutover(void)
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
    if(0==_language.compare("Simplified Chinese")) {
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
            _download_form[i]->Language(_language);
        }
    }
    ui->menuLanguage->setEnabled(true);
    ui->menuLanguage->setVisible(true);
}

void DownLoad::on_checkBox_mul_stateChanged(int arg1)
{
//    if(Qt::Checked==arg1)  // 选中
//    {
//        multiple(1);
//    }
//    else
//    {
//        multiple(0);
//    }
}

void DownLoad::actionLanguage_triggered()
{
#if 0
    QTranslator translator;

    QAction *action = (QAction*)sender();
    QString actionText = action->text();

    qDebug()<<actionText<<"triggerred!"<<endl;

//    if("1_1" == actionText) {
//        //...
//    } else if("1_2" == actionText) {
//        //...
//    }
    if(0==actionText.compare("Simplified Chinese")) {
        translator.load("cn.qm");
        ui->actionEnglish->setChecked(false);
        ui->actionSimplified_Chinese->setChecked(true);
    } else if (0==actionText.compare("English")) {
        translator.load("en.qm");
        ui->actionEnglish->setChecked(true);
        ui->actionSimplified_Chinese->setChecked(false);
    }
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
    uint8_t i=0;
    for(i=0; i<DOWNLOAD_FORM_SIZE; i++)
    {
        _download_form[i]->Language(actionText);
    }
#else
    //QTranslator translator;

    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QAction *action = (QAction*)sender();
    QString actionText = action->text();
    setting.setValue(Setting_Language, actionText);
    qApp->exit(7714);
#endif
}

void DownLoad::actionMode_triggered()
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QAction *action = (QAction*)sender();
#if 0
    QString actionText = action->text();
    qDebug()<<actionText<<"triggerred!"<<endl;
    //setting.setValue(Setting_Mul, actionText);
    // Multiple
    if(0==actionText.compare("Multiple")) {
        setting.setValue(Setting_Mul, actionText);
    } else if (0==actionText.compare("Single")) {
        setting.setValue(Setting_Mul, actionText);
    }
    // USB
    if(0==actionText.compare("USB")) {
        setting.setValue(Setting_USB, actionText);
    } else if (0==actionText.compare("WIFI")) {
        setting.setValue(Setting_USB, actionText);
    } else if (0==actionText.compare("MSC")) {
        setting.setValue(Setting_USB, actionText);
    }
#endif
    QString objectName = action->objectName();
    qDebug()<<"objectName:"<<objectName<<endl;
    //setting.setValue(Setting_Mul, actionText);
    // Multiple
    if(0==objectName.compare("actionMultiple")) {
        setting.setValue(Setting_Mul, "Multiple");
    }
    //else if (0==objectName.compare("actionSingle"))
    else // "actionSingle"
    {
        setting.setValue(Setting_Mul, "Single");
    }
    // USB
//    if(0==objectName.compare("actionUSB")) {
//        setting.setValue(Setting_USB, actionText);
//    } else
    if (0==objectName.compare("actionWIFI")) {
        setting.setValue(Setting_USB, "WIFI");
    } else if (0==objectName.compare("actionMSC")) {
        setting.setValue(Setting_USB, "MSC");
    }
    else if (0==objectName.compare("actionSerial")) {
           setting.setValue(Setting_USB, "Serial");
    }
    else
    {
        setting.setValue(Setting_USB, "USB");
    }
    qApp->exit(7714);
}

void DownLoad::actionHelp_triggered()
{
    QAction *action = (QAction*)sender();
    QString objectName = action->objectName();
    qDebug()<<"objectName:"<<objectName<<endl;
    if(0==objectName.compare("actionAbout")) {
        MyDialog _about;
        _about.setWindowModality(Qt::ApplicationModal);
        //_about.show();
        _about.exec();
    }
}

