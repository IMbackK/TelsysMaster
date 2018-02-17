#include "bleserial.h"
#include <QDebug>

#define NORDIC_UART_SERVICE_UUID "06E400001-B5A3-F393-­E0A9-­E50E24DCCA9E"
#define NORDIC_UART_TX_UUID      "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define NORDIC_UART_RX_UUID      "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

//WARNING: HACK
//regretably due to a gattlib bug gattlib_event_handler_t dose not pass user_data. Also gatt_connect_cb_t contains no user_data parameter therfore this hack is requierd.
static BleSerial* bleSerialNotifyInstance = nullptr;

BleSerial::BleSerial(void* adapter, bool setInstance): _adapter(adapter)
{
     if(setInstance) bleSerialNotifyInstance = this;
     gattlib_string_to_uuid(NORDIC_UART_RX_UUID, strlen(NORDIC_UART_RX_UUID), &nordicUartRxUuid);
     gattlib_string_to_uuid(NORDIC_UART_TX_UUID, strlen(NORDIC_UART_TX_UUID), &nordicUartTxUuid);
}

BleSerial::~BleSerial()
{
    reset();
    if(bleSerialNotifyInstance == this) bleSerialNotifyInstance = nullptr;
}

void BleSerial::connectTo(const BleDiscoveredDevice info)
{
   connectToAdress(info.address.toStdString());
}

void BleSerial::staticConnectionCallback(gatt_connection_t* connection)
{
    qDebug()<<"Conncb\n";
    bleSerialNotifyInstance->connectionCallback(/*connection*/);
}

bool BleSerial::isConnected()
{
    return _connection != NULL;
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
    if(isConnected()) gattlib_write_char_by_uuid(_connection, &nordicUartTxUuid, data, length);
    for(int i = 0; i < length; i++)qDebug()<<(char)data[i];
}

void BleSerial::connectionCallback(/*gatt_connection_t* connection*/)
{
    delete _connectTread;
    _connectTread = nullptr;
    //_connection = connection;
    if (!isConnected())
    {
            deviceDisconnected("Can not connect to Btle Device");
    }
    else
    {
        deviceConnected();
        gattlib_characteristic_t* characteristics;
        int characteristicsLength;
        int errorCode;
        errorCode = gattlib_discover_char(_connection, &characteristics, &characteristicsLength);
        if(errorCode)
        {
            deviceDisconnected("Can not discover Characteristics");
        }
        else
        {
            uint16_t tx_handle = 0;
            uint16_t rx_handle = 0;

            for (int i = 0; i < characteristicsLength; i++)
            {
                char buffer[256];
                gattlib_uuid_to_string(&characteristics[i].uuid, buffer, 256);
                qDebug()<<buffer<<'\n';

                if(gattlib_uuid_cmp(&characteristics[i].uuid, &nordicUartRxUuid) == 0)
                {
                    rx_handle = characteristics[i].value_handle;
                    _foundNusService = true;
                }
                else if (gattlib_uuid_cmp(&characteristics[i].uuid, &nordicUartTxUuid) == 0)
                {
                    tx_handle = characteristics[i].value_handle;
                }


            }
            if (tx_handle == 0)
            {
                 qDebug()<<"Fail to get NUS TX handle.\n";
            }
            if (rx_handle == 0)
            {
                qDebug()<<"Fail to get NUS RX handle.\n";
            }
            if(_foundNusService == false) deviceDisconnected("Device Dose not Support NUS service.\n");
            else
            {
                uint16_t enable_notification = 0x0001;
                gattlib_write_char_by_uuid(_connection, &nordicUartTxUuid, &enable_notification, sizeof(enable_notification));
                gattlib_register_notification(_connection, &notificationCallback, static_cast<void*>(this)); //regretably due to a gattlib bug this pointer is not really passed.
                gattlib_notification_start(_connection, &nordicUartRxUuid);
            }
        }
    }
}

void BleSerial::connectToAdress(const std::string adress)
{
    _connectTread = QThread::create([adress, this](){this->_connection = gattlib_connect(NULL, adress.c_str(), BDADDR_LE_PUBLIC, BT_SEC_LOW, 0, 0);});
    connect(_connectTread, &QThread::finished, this, &BleSerial::connectionCallback);
    deviceConnectionInProgress();
    _connectTread->start();
}

void BleSerial::notificationCallback(const uuid_t* uuid, const uint8_t* data, size_t length,  void* instanceVoid)
{
    //qDebug()<<"got Data!";
    BleSerial* instance  = static_cast<BleSerial*>(instanceVoid); //regretably due to a gattlib bug this is gets invalid poniter.
    //for( size_t i = 0; i < length ; i++) qDebug()<<(void*)data[i];
    if(gattlib_uuid_cmp(uuid, &bleSerialNotifyInstance->nordicUartRxUuid) == 0)bleSerialNotifyInstance->recived(data, length);
}

void BleSerial::reset()
{
    _foundNusService = false;
    if(_connection != NULL)
    {
        gattlib_notification_stop(_connection, &nordicUartRxUuid );
        gattlib_disconnect(_connection);
    }
    //_connection = NULL;
}
