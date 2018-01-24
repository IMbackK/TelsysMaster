#include "mainwindow.h"
#include "connectiondialog.h"
#include <QApplication>
#include <QLowEnergyController>
#include <QMessageBox>
#include <QDebug>
#include <QLoggingCategory>

#include "bleserial.h"
#include "blescanner.h"
#include "gattlib.h"


void selectDeviceToConnect(void* blteDevice, BleSerial* bleSerial)
{

    BleScanner scanner(blteDevice);
    ConnectionDialog conDiag(&scanner);

    QObject::connect(&conDiag, &ConnectionDialog::deviceSelected, bleSerial, &BleSerial::connectTo);

    conDiag.show();
    conDiag.exec();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    void* blteDevice;
    int errorCode = gattlib_adapter_open(NULL, &blteDevice);
    if (errorCode)
    {
        QMessageBox::critical(nullptr, "Error", "No active BLTE Adapter Detected.", QMessageBox::Ok);
        return -1;
    }

    BleSerial bleSerial(blteDevice);

    MainWindow w;

    QObject::connect(&w, &MainWindow::openConnDiag, [&blteDevice, &bleSerial](){selectDeviceToConnect(&blteDevice, &bleSerial);});

    QObject::connect(&bleSerial, &BleSerial::connected, &w, &MainWindow::connected);
    //QObject::connect(&bleSerial, &BleSerial::disconnected, &w, &MainWindow::disconnected);
    QObject::connect(&bleSerial, &BleSerial::disconnected, &w, &MainWindow::connectionFailed);

    w.show();

    return a.exec();
    //return 0;*/
}
