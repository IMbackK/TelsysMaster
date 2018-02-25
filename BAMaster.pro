#-------------------------------------------------
#
# Project created by QtCreator 2018-01-17T11:31:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = BAMaster
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QCUSTOMPLOT_USE_OPENGL

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -L./ -l:libgattlib.so --verbose

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    connectiondialog.cpp \
    blescanner.cpp \
    bleserial.cpp \
    sampleparser.cpp\
    qcustomplot/qcustomplot.cpp \
    limitdialog.cpp \
    callibrationdialog.cpp \
    replotdiag.cpp \
    ratedialog.cpp \
    statisticsdialog.cpp \
    utilitys.cpp \
    regessioncalculator.cpp \
    plot.cpp \
    regressiondiag.cpp

HEADERS += \
        mainwindow.h \
    connectiondialog.h \
    blescanner.h \
    gattlib.h \
    bleserial.h \
    sampleparser.h\
    point3D.h \
    qcustomplot/qcustomplot.h \
    limitdialog.h \
    callibrationdialog.h \
    replotdiag.h \
    ratedialog.h \
    statisticsdialog.h \
    utilitys.h \
    regessioncalculator.h \
    plot.h \
    regressiondiag.h

FORMS += \
        mainwindow.ui \
    connectiondialog.ui \
    limitdialog.ui \
    callibrationdialog.ui \
    replotdiag.ui \
    ratedialog.ui \
    statisticsdialog.ui \
    regressiondiag.ui
