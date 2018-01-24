#ifndef BTLESERIAL_H
#define BTLESERIAL_H
#include <QObject>
#include <QString>
#include <string>

#include "gattlib.h"
#include "blescanner.h"


class BleSerial : public QObject
{
    Q_OBJECT
private:
    void* _adapter;

    gatt_connection_t* _connection = nullptr;

    bool _foundNusService = false;

public:
    BleSerial(void* adapter, bool setInstance = true);
    ~BleSerial();

private:
    void reset();

signals:
    void disconnected(QString message);
    void connected();
    void recived(const uint8_t* data, size_t length);

public slots:
    void connectTo(const BleDiscoveredDevice info);
    void connectToAdress(const std::string adress);

private:
    void connectionCallback(gatt_connection_t* connection);

    static void notificationCallback(const uuid_t* uuid, const uint8_t* data, size_t length,  void* instanceVoid);
    static void staticConnectionCallback(gatt_connection_t* connection);
};

#endif // BTLESERIAL_H
