#ifndef BLESCANNER_H
#define BLESCANNER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QThread>

#include "blepp/blestatemachine.h"
#include "blepp/lescan.h"

#define BLE_SCAN_TIMEOUT 4

class BleDiscoveredDevice
{

public:
    BleDiscoveredDevice(QString addressIn, QString nameIn): address(addressIn), name(nameIn){}
    BleDiscoveredDevice(){};
    QString address;
    QString name;
};

Q_DECLARE_METATYPE(BleDiscoveredDevice)

class BleScanner: public QObject
{
    Q_OBJECT
private:
    bool isScanning = false;
    BLEPP::HCIScanner _scanner;

    QThread* _scannThread = nullptr;

public:
    BleScanner();
    ~BleScanner();
    bool getScanning();

public slots:

    void start();
    void stop();

signals:
    void finishedScanning();
    void discoverdDevice(BleDiscoveredDevice device);

private:
    void scanningThreadFinished();
    void scannWorker();
};

#endif // BLESCANNER_H
