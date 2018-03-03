#ifndef BLESCANNER_H
#define BLESCANNER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QBluetoothDeviceDiscoveryAgent>

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
    QBluetoothDeviceDiscoveryAgent* _agent;

public:
    BleScanner(QString adapter = "");
    ~BleScanner();
    bool getScanning();


public slots:

    void start();
    void stop();

signals:
    void finishedScanning();
    void discoverdDevice(BleDiscoveredDevice device);

private slots:
    void discoverdDeviceCallback(const QBluetoothDeviceInfo &info);
};

#endif // BLESCANNER_H
