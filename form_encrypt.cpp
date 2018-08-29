/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : encrypt thread.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "form_encrypt.h"
#include "ui_form_encrypt.h"

Form_Encrypt::Form_Encrypt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Encrypt)
{
    ui->setupUi(this);
    ui->m_progressBar->setValue(0);
    qss_default = ui->groupBox->styleSheet();
    ui->m_progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 对齐方式
    ui->m_progressBar->setFormat(tr("Watting"));
    _busy = 0;
    _program = new ControllerProgram();
    connect(_program, SIGNAL(resultReady(int)), this, SLOT(handleResults(int)));
    connect(_program, SIGNAL(sig_progress_update(const QString &, int)), this, SLOT(slots_progress_update(const QString &, int)));
}

Form_Encrypt::~Form_Encrypt()
{
    delete ui;
}

void Form_Encrypt::set_title(QString title)
{
    ui->groupBox->setTitle(title);
    if(_title.isEmpty()) _title=title;
}

void Form_Encrypt::encrypt(QString COM)
{
    _program->startEncrypt(COM);
    qDebug()<<QThread::currentThreadId();
    ui->groupBox->setTitle(COM);
    ui->m_progressBar->setStyleSheet(qss_default);
    _busy = 1;
}

int Form_Encrypt::idle(QString COM)
{
    QString _t;
    _t = ui->groupBox->title();
    if(0==_t.compare(COM))
    {
        //ui->m_progressBar->setStyleSheet(qss_default);
        ui->m_progressBar->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 0);\n" "border-color: rgb(255, 255, 255);"));
        ui->groupBox->setTitle(_title);
        ui->m_progressBar->setValue(0);
        return 0;
    }
    return -1;
}

void Form_Encrypt::Language(QTranslator &_t)
{
    //translator = _t;
    //qApp->installTranslator(&translator);
    qApp->installTranslator(&_t);
    ui->retranslateUi(this);
}

void Form_Encrypt::handleResults(int value)
{
    this->ui->m_progressBar->setValue(value);
}

void Form_Encrypt::slots_progress_update(const QString &titel, int value)
{
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
}
