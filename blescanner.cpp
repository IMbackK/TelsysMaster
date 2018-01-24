#include <QDebug>

#include "blescanner.h"
#include "gattlib.h"

//WARNING: HACK
//regretably user data support for scanning is not supported by gattlib at this time.
BleScanner* bleScannerNotifyInstance = nullptr;

BleScanner::BleScanner(void* adapter, bool setInstance): _adapter(adapter)
{
 if(setInstance) bleScannerNotifyInstance = this;
}

BleScanner::~BleScanner()
{
    if(bleScannerNotifyInstance == this) bleScannerNotifyInstance = nullptr;
    stop();
}

void BleScanner::start()
{
    isScanning = true;
    if(gattlib_adapter_scan_enable(_adapter, &discoverdDeviceCallback,  BLE_SCAN_TIMEOUT))
    {
        qDebug()<<"Failed To enable Scanner\n";
    }
}

bool BleScanner::getScanning()
{
    return isScanning;
}

void BleScanner::stop()
{
    if(isScanning)
    {
        isScanning = false;
        gattlib_adapter_scan_disable(_adapter);
    }
}

void BleScanner::setNotificationInstance(BleScanner *instance)
{
    bleScannerNotifyInstance = instance;
}

void BleScanner::discoverdDeviceCallback(const char* address, const char* name)
{
    BleDiscoveredDevice newDevice(address, name);
    if(bleScannerNotifyInstance != nullptr)bleScannerNotifyInstance->discoverdDevice(newDevice);
}
