#include <QDebug>

#include "blescanner.h"
#include "gattlib.h"
#include <QThread>
#include <QCoreApplication>

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
    qRegisterMetaType<BleDiscoveredDevice>();
    _scannThread = QThread::create([this](){gattlib_adapter_scan_enable(this->_adapter, &BleScanner::discoverdDeviceCallback,  BLE_SCAN_TIMEOUT);});
    connect(_scannThread, &QThread::finished, this, &BleScanner::scanningThreadFinished);
    _scannThread->start();
}

void BleScanner::scanningThreadFinished()
{
    finishedScanning();
    delete _scannThread;
    _scannThread = nullptr;
}

bool BleScanner::getScanning()
{
    return isScanning;
}

void BleScanner::stop()
{
    if( _scannThread != nullptr )
    {
       while (_scannThread->isRunning() )QCoreApplication::processEvents();
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
