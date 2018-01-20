#include "btleserial.h"
#include <QString>

#define NORDIC_UART_SERVICE_UUID QString("06E400001-B5A3-F393-­E0A9-­E50E24DCCA9E")
#define NORDIC_UART_TX_UUID 0x0002
#define NORDIC_UART_RX_UUID 0x0003

BtleSerial::BtleSerial()
{

}

BtleSerial::~BtleSerial()
{
    if(_telemetrySystem != nullptr) delete _telemetrySystem;
    if(_nusService != nullptr) delete _nusService;
}

void BtleSerial::connectTo(const QBluetoothDeviceInfo info)
{
    reset();

   _telemetrySystem = QLowEnergyController::createCentral(info);
   connect(_telemetrySystem, &QLowEnergyController::connected, this, [this](){this->connected();});
   connect(_telemetrySystem, &QLowEnergyController::disconnected, this, [this](){this->disconnected();});
   connect(_telemetrySystem, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(btError(QLowEnergyController::Error)));
   connect(_telemetrySystem, &QLowEnergyController::serviceDiscovered, this, [this](const QBluetoothUuid &uuid){this->serviceFound(uuid);});
   connect(_telemetrySystem, &QLowEnergyController::discoveryFinished, this, [this](){this->serviceScanFinished();});

   qDebug()<<"connecting to device: "<<info.address()<<'\n';
   _telemetrySystem->setRemoteAddressType(QLowEnergyController::RandomAddress);
   _telemetrySystem->connectToDevice();

}

void BtleSerial::btError(QLowEnergyController::Error error)
{
    if(error == QLowEnergyController::ConnectionError) connectionFailed("Can not connect To device.");
    else connectionFailed("General error");
}

void BtleSerial::serviceFound(const QBluetoothUuid &uuid)
{
    if (uuid == QBluetoothUuid(NORDIC_UART_SERVICE_UUID))
    {
        _foundNusService = true;
        _nusService = _telemetrySystem->createServiceObject(QBluetoothUuid(QBluetoothUuid::HeartRate), nullptr);
        if (_nusService)
        {
            connect(_nusService, &QLowEnergyService::characteristicChanged, this, &BtleSerial::characteristicChange);
        }
        else reset();
    }
    else qDebug()<<"found service with uuid: "<<uuid;
}

void BtleSerial::characteristicChange(const QLowEnergyCharacteristic &characteristic, const QByteArray &data)
{
    QString uuid("06E400001-B5A3-F393-­E0A9-­E50E24DCCA9E");
    if(characteristic.uuid() == QBluetoothUuid(NORDIC_UART_SERVICE_UUID))
    {
        qDebug()<<"got Data!";
        recived(data);
    }
    else qDebug()<<"got something!";
}

void BtleSerial::serviceScanFinished()
{
    if(!_foundNusService)
    {
        reset();
        connectionFailed("Connected Device dose not provide Nordic Uart Service.");
    }
}

void BtleSerial::reset()
{
    _foundNusService = false;
    if(_nusService != nullptr) delete _nusService;
    if(_telemetrySystem != nullptr)
    {
        _telemetrySystem->disconnectFromDevice();
        delete _telemetrySystem;
        _telemetrySystem = nullptr;
    }
}
