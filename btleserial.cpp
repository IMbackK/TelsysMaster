#include "btleserial.h"
#include <QDebug>

#define NORDIC_UART_SERVICE_UUID QString("06E400001-B5A3-F393-­E0A9-­E50E24DCCA9E")
#define NORDIC_UART_TX_UUID 0x0002
#define NORDIC_UART_RX_UUID 0x0003

BtleSerial::BtleSerial()
{
}

BtleSerial::~BtleSerial()
{
}

void BtleSerial::connectTo(const BLEPP::AdvertisingResponse info)
{
   connectToAdress(info.address);
}

void BtleSerial::connectToAdress(const std::string adress)
{
    reset();

    std::function<void()> serviceFuction = std::function<void()>([this](){this->serviceFound();});
   _telemetrySystem.setup_standard_scan( serviceFuction );
   _telemetrySystem.cb_connected = [this](){this->connected();};
   _telemetrySystem.cb_disconnected = [this](BLEPP::BLEGATTStateMachine::Disconnect d){this->disconnect(BLEPP::BLEGATTStateMachine::get_disconnect_string(d));};

   //qDebug()<<"connecting to device: "<<QString::fromStdString(info.address)<<'\n';
   _telemetrySystem.connect_blocking(adress);
}

/*void BtleSerial::btError(QLowEnergyController::Error error)
{
    if(error == QLowEnergyController::ConnectionError) connectionFailed("Can not connect To device.");
    else connectionFailed("General error");
}*/

void BtleSerial::serviceFound()
{
    for(unsigned int i = 0; i < _telemetrySystem.primary_services.size(); i++)
    {
        if(_telemetrySystem.primary_services[i].uuid == BLEPP::UUID("06E400001-B5A3-F393-­E0A9-­E50E24DCCA9E"))
        {
            for(BLEPP::Characteristic& characteristic: _telemetrySystem.primary_services[i].characteristics)
            {
                _foundNusService = true;
                if(characteristic.uuid == BLEPP::UUID(NORDIC_UART_RX_UUID))
                {
                    characteristic.cb_notify_or_indicate = [this](const BLEPP::PDUNotificationOrIndication& notification ){this->characteristicChange(notification);};
                    characteristic.set_notify_and_indicate(true, false);
                }
            }

        }
    }
}

void BtleSerial::characteristicChange(const BLEPP::PDUNotificationOrIndication& notification)
{
    qDebug()<<"got Data!";
    //recived(data);
}

void BtleSerial::serviceScanFinished()
{
    if(!_foundNusService)
    {
        reset();
        disconnected("Connected Device dose not provide Nordic Uart Service.");
    }
}

void BtleSerial::reset()
{
    _foundNusService = false;
    _telemetrySystem.close();
}
