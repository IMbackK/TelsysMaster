#ifndef BLESCANNER_H
#define BLESCANNER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QThread>

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
    void* _adapter;

    QThread* _scannThread = nullptr;

public:
    BleScanner(void* adapter, bool setInstance = true);
    ~BleScanner();
    static void setNotificationInstance(BleScanner* instance);
    bool getScanning();


public slots:

    void start();
    void stop();

signals:
    void finishedScanning();
    void discoverdDevice(BleDiscoveredDevice device);

private:
    void scanningThreadFinished();
    static void discoverdDeviceCallback(const char* address, const char* name);
};

#endif // BLESCANNER_H
