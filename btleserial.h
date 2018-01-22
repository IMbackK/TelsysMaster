#ifndef BTLESERIAL_H
#define BTLESERIAL_H
#include <QObject>
#include <QString>
#include <string>

#include "blepp/logging.h"
#include "blepp/pretty_printers.h"
#include "blepp/blestatemachine.h"
#include "blepp/lescan.h"

class BtleSerial : public QObject
{
    Q_OBJECT
private:
    BLEPP::BLEGATTStateMachine _telemetrySystem;

    bool _foundNusService = false;

public:
    BtleSerial();
    ~BtleSerial();

private:
    void reset();

signals:
    void disconnected(QString message);
    void connected();
    void recived(const QByteArray &value);

public slots:
    void connectTo(const BLEPP::AdvertisingResponse info);
    void connectToAdress(const std::string adress);

private slots:
   // void btError(QLowEnergyController::Error error);
    void serviceFound();
    void serviceScanFinished();

    void characteristicChange(const BLEPP::PDUNotificationOrIndication& notification);
};

#endif // BTLESERIAL_H
