#ifndef BTLESERIAL_H
#define BTLESERIAL_H
#include <QObject>
#include <QString>
#include <QThread>

#include "gattlib.h"
#include "blescanner.h"


class BleSerial : public QObject
{
    Q_OBJECT
private:
    void* _adapter;

    gatt_connection_t* _connection = NULL;

    uuid_t nordicUartTxUuid;
    uuid_t nordicUartRxUuid;

    bool _foundNusService = false;

    QThread* _connectTread = nullptr;

public:
    BleSerial(void* adapter, bool setInstance = true);
    ~BleSerial();

    bool isConnected();

private:
    void reset();

signals:
    void deviceDisconnected(QString message);
    void deviceConnected();
    void recived(const uint8_t* data, size_t length);
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
