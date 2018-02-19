#include "bleserial.h"
#include <QDebug>

#define SERVICE_UUID "66410001-b565-4284-d0a8-54775812af9e"
#define TX_UUID      "66410002-b565-4284-d0a8-54775812af9e"
#define ADC_UUID      "66410003-b565-4284-d0a8-54775812af9e"
#define AUX_UUID       "66410004-b565-4284-d0a8-54775812af9e"

BleSerial::BleSerial()
{
}

BleSerial::~BleSerial()
{
}

void BleSerial::connectTo(const BleDiscoveredDevice info)
{
   connectToAdress(info.address.toStdString());
}

void BleSerial::connectToAdress(const std::string adress)
{
    reset();

    std::function<void()> serviceFuction = std::function<void()>([this](){this->serviceFound();});
   _telemetrySystem.setup_standard_scan( serviceFuction );
   _telemetrySystem.cb_connected = [this](){this->deviceConnected();};
   _telemetrySystem.cb_disconnected = [this](BLEPP::BLEGATTStateMachine::Disconnect d){this->deviceDisconnected(QString::fromStdString(BLEPP::BLEGATTStateMachine::get_disconnect_string(d)));};

   //qDebug()<<"connecting to device: "<<QString::fromStdString(info.address)<<'\n';
   _telemetrySystem.connect_blocking(adress);
}

/*void BleSerial::btError(QLowEnergyController::Error error)
{
    if(error == QLowEnergyController::ConnectionError) connectionFailed("Can not connect To device.");
    else connectionFailed("General error");
}*/

void BleSerial::serviceFound()
{
    for(unsigned int i = 0; i < _telemetrySystem.primary_services.size(); i++)
    {
        if(_telemetrySystem.primary_services[i].uuid == BLEPP::UUID(SERVICE_UUID))
        {
            _foundService = true;
            for(BLEPP::Characteristic& characteristic: _telemetrySystem.primary_services[i].characteristics)
            {
                if(characteristic.uuid == BLEPP::UUID(ADC_UUID))
                {
                    characteristic.cb_notify_or_indicate = [this](const BLEPP::PDUNotificationOrIndication& notification ){this->adcCharacteristicChange(notification);};
                    characteristic.set_notify_and_indicate(true, false);
                }
                else if(characteristic.uuid == BLEPP::UUID(AUX_UUID))
                {
                    characteristic.cb_notify_or_indicate = [this](const BLEPP::PDUNotificationOrIndication& notification ){this->auxCharacteristicChange(notification);};
                    characteristic.set_notify_and_indicate(true, false);
                }
            }

        }
    }
}

void BleSerial::deviceDisconnect()
{

}

bool BleSerial::isConnected()
{
    return _telemetrySystem.is_idle();
}

void BleSerial::adcCharacteristicChange(const BLEPP::PDUNotificationOrIndication& notification)
{
    qDebug()<<"got Data!";
    //recived(data);
}

void BleSerial::auxCharacteristicChange(const BLEPP::PDUNotificationOrIndication& notification)
{
    qDebug()<<"got Data!";
    //recived(data);
}

void BleSerial::serviceScanFinished()
{
    if(!_foundService)
    {
        reset();
        deviceDisconnected("Connected Device dose not provide Telemetrysytem Service.");
    }
}

void BleSerial::write(uint8_t* data, size_t length)
{

}

void BleSerial::reset()
{
    _foundService = false;
    _telemetrySystem.close();
}
