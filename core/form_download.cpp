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

#include "form_download.h"
#include "ui_form_download.h"
#include <QFileDialog>
#include "scanning.h"

Form_Download::Form_Download(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Download)
{
    ui->setupUi(this);
    ui->Button_Download->setVisible(false);
    ui->Button_Loadfile->setVisible(false);
    ui->Button_Download->setEnabled(false);
    ui->Button_Loadfile->setEnabled(false);
    ui->label_file->setVisible(false);
    ui->label_watting->setVisible(false);
    ui->Button_Download->setText(tr("Download"));
    ui->Button_Loadfile->setText(tr("Loadfile"));
    ui->m_progressBar->setValue(0);
//    ui->m_progressBar->setMinimum(0);
//    ui->m_progressBar->setMaximum(100);
    //ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n" "border-color: rgb(255, 255, 127);"));
    qss_default = ui->groupBox_download->styleSheet();
    //ui->groupBox_download->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n" "border-color: rgb(255, 255, 255);"));
    //ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n" "border-color: rgb(255, 255, 255);"));
    //ui->groupBox_download->setStyleSheet(qss_default);
    //ui->groupBox_download->styleSheet();
//    this->adjustSize();
//    setFixedSize(this->width(), this->height());
    //ui->m_progressBar->setTextVisible(false);
    //ui->m_progressBar->setFormat(QString::fromLocal8Bit("当前进度为：%1%").arg(QString::number(41.7, 'f', 1)));
    ui->m_progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 对齐方式
    //ui->m_progressBar->setFormat(QString("当前进度为：%1%").arg(QString::number(41.7, 'f', 1)));
    //ui->m_progressBar->setFormat(QString("watting...  %1%").arg(QString::number(0)));
    //ui->m_progressBar->setFormat(QString(tr("watting...")));
    ui->m_progressBar->setFormat(tr("Watting"));
    //ui->m_progressBar->setValue(20);
//    ui->m_progressBar->setMinimum(0);
//    ui->m_progressBar->setMaximum(0);
    _busy = 0;
    fw = new Firmware;
    _program = new ControllerProgram();
    //connect(_program, &ControllerProgram::resultReady, this, handleResults);
    connect(_program, SIGNAL(resultReady(int)), this, SLOT(handleResults(int)));
    connect(_program, SIGNAL(sig_progress_update(const QString &, int)), this, SLOT(slots_progress_update(const QString &, int)));
}

Form_Download::~Form_Download()
{
    delete ui;
}

void Form_Download::set_title(QString title)
{
    ui->groupBox_download->setTitle(title);
    if(_title.isEmpty()) _title=title;
    //qDebug() << title;
}

void Form_Download::set_path(QString _path)
{
    fileName = _path;
    if (fileName.isEmpty())
        return;
    ui->label_file->setText(fileName);
    fw->ProcessFirmware(fileName);
}

void Form_Download::download(QString COM)
{
    _program->start(COM, fw);
    //_program->start("COM13");
    qDebug()<<QThread::currentThreadId();
    ui->groupBox_download->setTitle(COM);
    ui->m_progressBar->setStyleSheet(qss_default);
    _busy = 1;
}

int Form_Download::idle(QString COM)
{
    QString _t;
    _t = ui->groupBox_download->title();
    if(0==_t.compare(COM))
    {
        //ui->m_progressBar->setStyleSheet(qss_default);
        ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 0);\n" "border-color: rgb(255, 255, 255);"));
        ui->groupBox_download->setTitle(_title);
        ui->m_progressBar->setValue(0);
        return 0;
    }
    return -1;
}

void Form_Download::Language(QString actionText)
{
    if(0==actionText.compare("Simplified Chinese")) {
        translator.load("cn.qm");
    } else if (0==actionText.compare("English")) {
        translator.load("en.qm");
    }
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
}

void Form_Download::on_Button_Loadfile_clicked()
{
    open_firmware();
}

void Form_Download::on_Button_Download_clicked()
{
    if(fileName.isEmpty()) // open
    {
        open_firmware();
    }
    _program->start("COM6", fw);
    //_program->start("COM13");
    qDebug()<<QThread::currentThreadId();
    _busy = 1;
}

void Form_Download::handleResults(int value)
{
    this->ui->m_progressBar->setValue(value);
}

void Form_Download::slots_progress_update(const QString &titel, int value)
{
#if 0
    this->ui->label_watting->setText(titel);
    this->ui->m_progressBar->setValue(value);
#else
    switch (value) {
    case Program::Code::PULL:
        ui->m_progressBar->setStyleSheet(qss_default);
        ui->m_progressBar->setFormat(titel);
        break;
    case Program::Code::ERR:
        ui->m_progressBar->setValue(0);
        ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);\n" "border-color: rgb(255, 255, 255);"));
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
#endif
}

void Form_Download::open_firmware()
{
    QSettings setting("./Setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
    QString lastPath = setting.value("LastFilePath").toString();
    //打开传输的文件
    //QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::currentPath(),tr("HEX File(*.hex);;BIN File(*.bin);;ALL File (*.*)"));
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),lastPath,tr("BIN File(*.bin);;HEX File(*.hex)"));
    if (fileName.isEmpty())
        return;
    ui->label_file->setText(fileName);
    fw->ProcessFirmware(fileName);
}
