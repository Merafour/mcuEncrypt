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

#ifndef FORM_ENCRYPT_H
#define FORM_ENCRYPT_H

#include <QWidget>
#include <QTranslator>
#include "core/user_port.h"
#include "core/program.h"
#include "core/scanning.h"

namespace Ui {
class Form_Encrypt;
}

class Form_Encrypt : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Encrypt(QWidget *parent = 0);
    ~Form_Encrypt();

    void set_title(QString title);
    QString get_title(void)
    {
        return _title;
    }
    void encrypt(QString COM);
    int idle(QString COM);
    uint8_t busy(void)
    {
        return _busy;
    }
    void Language(QTranslator &_t);

private slots:

    void handleResults(int value);
    void slots_progress_update(const QString &titel, int value);
private:
    Ui::Form_Encrypt *ui;

    QString fileName;
    ControllerProgram *_program;
    uint8_t _busy;
    QString _title;
    QString qss_default;
    //QTranslator translator;
};

#endif // FORM_ENCRYPT_H
