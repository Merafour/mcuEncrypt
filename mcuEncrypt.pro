#-------------------------------------------------
#
# Project created by QtCreator 2018-08-29T14:05:11
#
#-------------------------------------------------

QT       += core gui xml serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TRANSLATIONS += cn.ts en.ts

TARGET = mcuEncrypt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += core
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    core/about.cpp \
    core/download.cpp \
    core/Firmware.cpp \
    core/form_download.cpp \
    core/mydialog.cpp \
    core/program.cpp \
    core/scanning.cpp \
    core/uploader.cpp \
    core/user_port.cpp \
    form_encrypt.cpp \
    core/tea.c

HEADERS += \
        mainwindow.h \
    core/about.h \
    core/download.h \
    core/Firmware.h \
    core/form_download.h \
    core/mydialog.h \
    core/program.h \
    core/scanning.h \
    core/uploader.h \
    core/user_port.h \
    form_encrypt.h \
    core/tea.h

FORMS += \
        mainwindow.ui \
    core/about.ui \
    core/download.ui \
    core/form_download.ui \
    core/mydialog.ui \
    form_encrypt.ui
