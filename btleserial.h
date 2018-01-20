#ifndef BTLESERIAL_H
#define BTLESERIAL_H
#include <QObject>
#include <QLowEnergyController>
#include <QString>

class BtleSerial : public QObject
{
    Q_OBJECT
private:
    QLowEnergyController* _telemetrySystem = nullptr;
    QLowEnergyService*    _nusService      = nullptr;

    bool _foundNusService = false;

public:
    BtleSerial();
    ~BtleSerial();

private:
    void reset();

signals:
    void connectionFailed(QString errorMsg);
    void connected();
    void disconnected();
    void recived(const QByteArray &value);

public slots:
    void connectTo(const QBluetoothDeviceInfo info);

private slots:
    void btError(QLowEnergyController::Error error);
    void serviceFound(const QBluetoothUuid &uuid);
    void serviceScanFinished();

    void characteristicChange(const QLowEnergyCharacteristic &c, const QByteArray &data);
};

#endif // BTLESERIAL_H
