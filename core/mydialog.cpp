/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 29/8/2018
* Description        : About UI.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "mydialog.h"
#include "ui_mydialog.h"

MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    char time[256];
    ui->setupUi(this);
    //ui->buttonBox->setVisible(false);
    setWindowTitle("About");
    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    ui->groupBox_version->setTitle("Version");
    ui->label_version->setText("  Version: V1.12.0");
    memset(time, 0, sizeof(time));
    snprintf(time, sizeof(time)-1, "  %s %s", __DATE__, __TIME__);
    ui->label_time->setText(time);
    ui->textBrowser_version->setEnabled(false);
    ui->textBrowser_version->setText("支持功能：");
    ui->textBrowser_version->append("1.中英文切换；");
    ui->textBrowser_version->append("2.批量烧录；");
    ui->textBrowser_version->append("3.USB、串口升级；");
    ui->textBrowser_version->append("版本更新:");
    ui->textBrowser_version->append("1.增加串口错误检查，擦除时断开连接立即提示；");
    ui->textBrowser_version->append("2.升级完成拔出设备改变进度条颜色以提示；");
    adjustSize();
    setFixedSize(this->width(), this->height());
}

MyDialog::~MyDialog()
{
    delete ui;
}
