#-------------------------------------------------
#
# Project created by QtCreator 2021-04-06T12:46:36
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += multimedia
QT += multimediawidgets
QT += network
QT += serialport

TARGET = SPF_Configurator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dev_machine.cpp \
    dev_usbcam.cpp

HEADERS += \
        mainwindow.h \
    dev_machine.h \
    dev_usbcam.h

INCLUDEPATH += C:\opencv\build_minGW\install\include
LIBS += C:\opencv\build_minGW\install\x64\mingw\bin\libopencv_*.dll

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
