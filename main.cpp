#include "mainwindow.h"
#include "connectiondialog.h"
#include <QApplication>
#include <QLowEnergyController>
#include <QMessageBox>
#include <QDebug>
#include <QLoggingCategory>

#include "blepp/logging.h"

#include "btleserial.h"


void selectDeviceToConnect(BtleSerial* blte)
{
    ConnectionDialog conDiag;

    QObject::connect(&conDiag, &ConnectionDialog::deviceSelected, blte, &BtleSerial::connectTo);

    conDiag.show();
    conDiag.exec();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QLoggingCategory::setFilterRules("*.debug=false\n" "qt.bluetooth.bluez.debug=true\n" "qt.bluetooth.debug=true");

    BtleSerial btle;

    BLEPP::log_level = BLEPP::LogLevels::Info;

    qDebug()<<"Printing Debug\n";

    btle.connectToAdress("04:69:F8:AB:49:E6");
/*
    MainWindow w;

    QObject::connect(&w, &MainWindow::openConnDiag, [&btle](){selectDeviceToConnect(&btle);});

    QObject::connect(&btle, &BtleSerial::connected, &w, &MainWindow::connected);
    //QObject::connect(&btle, &BtleSerial::disconnected, &w, &MainWindow::disconnected);
    QObject::connect(&btle, &BtleSerial::disconnected, &w, &MainWindow::connectionFailed);

    w.show();

    return a.exec();
    //return 0;*/
}
