#-------------------------------------------------
#
# Project created by QtCreator 2018-01-17T11:31:03
#
#-------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



TARGET = BAMaster
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS +=-lbluetooth

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    connectiondialog.cpp \
    btleserial.cpp \
    blepp/att_pdu.cc blepp/att.cc blepp/bledevice.cc blepp/blestatemachine.cc blepp/float.cc blepp/lescan.cc blepp/logging.cc blepp/pretty_printers.cc blepp/uuid.cc

HEADERS += \
        mainwindow.h \
    connectiondialog.h \
    btleserial.h

FORMS += \
        mainwindow.ui \
    connectiondialog.ui