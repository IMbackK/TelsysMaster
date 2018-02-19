#ifndef BTLESERIAL_H
#define BTLESERIAL_H
#include <QObject>
#include <QString>
#include <string>

#include "blepp/logging.h"
#include "blepp/pretty_printers.h"
#include "blepp/blestatemachine.h"


#include "blescanner.h"

class BleSerial : public QObject
{
    Q_OBJECT
private:
    BLEPP::BLEGATTStateMachine _telemetrySystem;

    bool _foundService = false;

public:
    BleSerial();
    ~BleSerial();

    bool isConnected();

private:
    void reset();

signals:
    void deviceDisconnected(QString message);
    void deviceConnected();
    void recivedAdcPacket(const uint8_t* data, size_t length);
    void recivedAuxPacket(const uint8_t* data, size_t length);
    void deviceConnectionInProgress();

public slots:
    void connectTo(const BleDiscoveredDevice info);
    void connectToAdress(const std::string adress);

    void write(uint8_t* data, size_t length);

    void deviceDisconnect();

private slots:
    void serviceFound();
    void serviceScanFinished();

private:
    void adcCharacteristicChange(const BLEPP::PDUNotificationOrIndication& notification);
    void auxCharacteristicChange(const BLEPP::PDUNotificationOrIndication& notification);
};

#endif // BTLESERIAL_H
