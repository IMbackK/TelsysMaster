#include "bleserial.h"
#include <QDebug>

#define NORDIC_UART_SERVICE_UUID "06E400001-B5A3-F393-­E0A9-­E50E24DCCA9E"
#define NORDIC_UART_TX_UUID      "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define NORDIC_UART_RX_UUID      "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

//WARNING: HACK
//regretably due to a gattlib bug gattlib_event_handler_t dose not pass user_data. Also gatt_connect_cb_t contains no user_data parameter therfore this hack is requierd.
BleSerial* bleSerialNotifyInstance = nullptr;

BleSerial::BleSerial(void* adapter, bool setInstance): _adapter(adapter)
{
     if(setInstance) bleSerialNotifyInstance = this;
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
    bleSerialNotifyInstance->connectionCallback(connection);
}

void BleSerial::connectionCallback(gatt_connection_t* connection)
{
    _connection = connection;
    if (_connection == NULL)
    {
            disconnect("Can not connect to Btle Device");
    }
    else
    {
        connected();
        gattlib_characteristic_t* characteristics;
        int characteristicsLength;
        int errorCode;
        errorCode = gattlib_discover_char(_connection, &characteristics, &characteristicsLength);
        if(errorCode)
        {
            disconnect("Can not discover Characteristics");
        }
        else
        {
            uuid_t nordicUartRxUuid;
            gattlib_string_to_uuid(NORDIC_UART_RX_UUID, strlen(NORDIC_UART_RX_UUID), &nordicUartRxUuid);
            uuid_t nordicUartTxUuid;
            gattlib_string_to_uuid(NORDIC_UART_TX_UUID, strlen(NORDIC_UART_TX_UUID), &nordicUartTxUuid);
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
            if(_foundNusService == false) disconnect("Device Dose not Support NUS service.\n");
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
    gattlib_connect_async(NULL, adress.c_str(), BDADDR_LE_PUBLIC, BT_SEC_LOW, 0, 0, &staticConnectionCallback );
}

void BleSerial::notificationCallback(const uuid_t* uuid, const uint8_t* data, size_t length,  void* instanceVoid)
{
    qDebug()<<"got Data!";
    uuid_t nordicUartRxUuid;
    gattlib_string_to_uuid(NORDIC_UART_RX_UUID, strlen(NORDIC_UART_RX_UUID), &nordicUartRxUuid);
    BleSerial* instance  = static_cast<BleSerial*>(instanceVoid); //regretably due to a gattlib bug this is gets invalid poniter.
    if(gattlib_uuid_cmp(uuid, &nordicUartRxUuid) == 0)bleSerialNotifyInstance->recived(data, length);
}

void BleSerial::reset()
{
    _foundNusService = false;
    if(_connection != nullptr) gattlib_disconnect(_connection);
}
