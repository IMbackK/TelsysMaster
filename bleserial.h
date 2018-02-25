#ifndef BTLESERIAL_H
#define BTLESERIAL_H
#include <QObject>
#include <QString>
#include <QThread>
#include <functional>

#include "gattlib.h"
#include "blescanner.h"


class BleSerial : public QObject
{
    Q_OBJECT
private:
    void* _adapter;

    gatt_connection_t* _connection = NULL;

    uuid_t txUuid;
    uuid_t adcUuid;
    uuid_t auxUuid;

    bool _foundService = false;

    QThread* _connectTread = nullptr;

    std::function<void(const uint8_t*, size_t)> adcPaketCallback;
    std::function<void(const uint8_t*, size_t)> auxPaketCallback;

public:
    BleSerial(void* adapter, bool setInstance = true);
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
    void connectionCallback(/*gatt_connection_t* connection*/);

private:

    static void notificationCallback(const uuid_t* uuid, const uint8_t* data, size_t length,  void* instanceVoid);
    static void staticConnectionCallback(gatt_connection_t* connection);
};

#endif // BTLESERIAL_H
