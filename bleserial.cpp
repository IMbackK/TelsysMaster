#include "bleserial.h"
#include <QDebug>
#include <QCoreApplication>

#define SERVICE_UUID "66410001-b565-4284-d0a8-54775812af9e"
#define TX_UUID      "66410002-b565-4284-d0a8-54775812af9e"
#define ADC_UUID      "66410003-b565-4284-d0a8-54775812af9e"
#define AUX_UUID       "66410004-b565-4284-d0a8-54775812af9e"

BleSerial::BleSerial(QString adapter): txUuid(QString(TX_UUID)), adcUuid(QString(ADC_UUID)), auxUuid(QString(AUX_UUID))
{
    qDebug()<<"uuid: "<<txUuid;
}

BleSerial::~BleSerial()
{
    reset();
}

void BleSerial::connectTo(const BleDiscoveredDevice info)
{
   connectToAdress(info.address.toStdString());
}

bool BleSerial::isConnected()
{
    return _telsysService != nullptr;
}

void BleSerial::deviceDisconnect()
{
    if(isConnected())
    {
        qDebug()<<"Disconnecting\n";
        reset();
    }
}

void BleSerial::write(uint8_t* data, size_t length)
{
    if(isConnected())
    {
        qDebug()<<"attempting write";
        _telsysService->writeCharacteristic(_txChar, QByteArray((char*)data, length), QLowEnergyService::WriteWithoutResponse);
    }
}

void BleSerial::setAdcPacketCallback(std::function<void(const uint8_t*, size_t)> cb)
{
    adcPaketCallback = cb;
}

void BleSerial::setAuxPacketCallback(std::function<void(const uint8_t*, size_t)> cb)
{
    auxPaketCallback = cb;
}

void BleSerial::connectToAdress(const std::string address)
{
    reset();
    QBluetoothDeviceInfo info(QBluetoothAddress(address.c_str()), "info", 0);
    info.setCoreConfigurations(QBluetoothDeviceInfo::LowEnergyCoreConfiguration);

    qDebug()<<"info: "<<info.address();

     _leController = QLowEnergyController::createCentral(info);
     connect(_leController, &QLowEnergyController::connected, this, [this](){this->_leController->discoverServices();});
     connect(_leController, SIGNAL(error), this, SLOT([this](QLowEnergyController::Error error){this->deviceDisconnected("Connection Failed");}));
     connect(_leController, &QLowEnergyController::discoveryFinished, this, [this](){this->serviceScanFinished();});

     qDebug()<<"connecting to device: "<<info.address()<<'\n';
     deviceConnectionInProgress();
     _leController->setRemoteAddressType(QLowEnergyController::RandomAddress);
     _leController->connectToDevice();
}

void BleSerial::serviceScanFinished()
{
    for(int i = 0; i < _leController->services().size(); i++)
    {
        QBluetoothUuid uuid = _leController->services()[i];
        qDebug()<<"service with uuid: "<<uuid;
        if (uuid == QBluetoothUuid(QString(SERVICE_UUID)))
        {
            qDebug()<<"service mached";
            _telsysService = _leController->createServiceObject(uuid, nullptr);

            if (_telsysService)
            {
                if(_telsysService->state() == QLowEnergyService::DiscoveryRequired)
                {
                    qDebug()<<"char discovery needed";
                    _telsysService->discoverDetails();
                    while(_telsysService->state() == QLowEnergyService::DiscoveringServices)QCoreApplication::processEvents();
                }
                qDebug()<<"service connected has"<<_telsysService->characteristics().size()<<"chars";
                for(int i =0; i <  _telsysService->characteristics().size(); i++)
                {
                    QLowEnergyCharacteristic tmp = _telsysService->characteristics()[i];
                    if(tmp.properties() & QLowEnergyCharacteristic::Notify)
                    {
                        auto notificationDes = tmp.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
                        _telsysService->writeDescriptor(notificationDes, QByteArray::fromHex("0100"));
                    }
                    qDebug()<<"characteristic with uuid: "<<_telsysService->characteristics().at(i).uuid();
                }
                _txChar = _telsysService->characteristic(txUuid);
                if(!_txChar.isValid())
                {
                    deviceDisconnected("Connected to tx characteristic");
                    qDebug()<<"Tx char invalid";
                }
                connect(_telsysService, &QLowEnergyService::characteristicChanged, this, &BleSerial::characteristicChange);
            }
            else reset();
        }
    }
    this->deviceConnected();
    if(_telsysService == nullptr)
    {
        reset();
        deviceDisconnected("Connected Device dose not provide Telsys Service.");
    }
    qDebug()<<"serviceScanFinished";
}

void BleSerial::characteristicChange(const QLowEnergyCharacteristic &characteristic, const QByteArray &data)
{
    if(characteristic.uuid() == adcUuid)
    {
        adcPaketCallback((const uint8_t*)data.constData(), data.size());
    }
    else if(characteristic.uuid() == auxUuid)
    {
        auxPaketCallback((const uint8_t*)data.constData(), data.size());
    }
}

void BleSerial::reset()
{
    if(_telsysService != nullptr)
    {
        delete _telsysService;
        _telsysService = nullptr;
    }
    if(_leController != nullptr)
    {
        _leController->disconnectFromDevice();
        delete _leController;
        _leController = nullptr;
    }
}
