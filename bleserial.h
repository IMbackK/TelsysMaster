#ifndef BTLESERIAL_H
#define BTLESERIAL_H
#include <QObject>
#include <QString>
#include <QThread>
#include <functional>
#include <QtBluetooth/qbluetooth.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qbluetoothuuid.h>

#include "blescanner.h"


class BleSerial : public QObject
{
    Q_OBJECT
private:
    void* _adapter;

    QLowEnergyController* _leController = nullptr;
    QLowEnergyService* _telsysService = nullptr;
    QLowEnergyCharacteristic _txChar;

    QBluetoothUuid txUuid;
    QBluetoothUuid adcUuid;
    QBluetoothUuid auxUuid;

    std::function<void(const uint8_t*, size_t)> adcPaketCallback;
    std::function<void(const uint8_t*, size_t)> auxPaketCallback;

public:
    BleSerial(QString adapter = "");
    ~BleSerial();

    bool isConnected();

    void setAdcPacketCallback(std::function<void(const uint8_t*, size_t)> cb);
    void setAuxPacketCallback(std::function<void(const uint8_t*, size_t)> cb);

private:
    void reset();

signals:
    void deviceDisconnected(QString message);
    void deviceConnected();
    void deviceConnectionInProgress();

public slots:
    void connectTo(const BleDiscoveredDevice info);
    void connectToAdress(const std::string adress);


    void write(uint8_t* data, size_t length);

    void deviceDisconnect();

private slots:
    void characteristicChange(const QLowEnergyCharacteristic &characteristic, const QByteArray &data);
    void serviceScanFinished();

};

#endif // BTLESERIAL_H
