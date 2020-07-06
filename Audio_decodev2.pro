#-------------------------------------------------
#
# Project created by QtCreator 2018-09-28T17:38:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia core

TARGET = Audio_decodev2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++17
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "$$PWD/openssl-1.1.1b-win32-mingw/include"

LIBS += "$$PWD/openssl-1.1.1b-win32-mingw/lib/libssl.a"
LIBS += "$$PWD/openssl-1.1.1b-win32-mingw/lib/libcrypto.a"

SOURCES += main.cpp\
           mainwindow.cpp\
           cryptohandler.cpp\
           aesencryption.cpp


HEADERS  += mainwindow.h\
            cryptohandler.h\
            aesencryption.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
