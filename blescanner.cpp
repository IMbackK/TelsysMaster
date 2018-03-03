#include <QDebug>

#include "blescanner.h"
#include <QCoreApplication>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>

BleScanner::BleScanner(QString adapter)
{
    if(adapter.size() != 0) _agent = new QBluetoothDeviceDiscoveryAgent(QBluetoothAddress(adapter));
    else _agent = new QBluetoothDeviceDiscoveryAgent;

     connect(_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BleScanner::discoverdDeviceCallback);
     connect(_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BleScanner::finishedScanning);
}

BleScanner::~BleScanner()
{
    stop();
    delete _agent;
}

void BleScanner::start()
{
    _agent->start();
}

bool BleScanner::getScanning()
{
    return _agent->isActive();
}

void BleScanner::stop()
{
    _agent->stop();
}

void BleScanner::discoverdDeviceCallback(const QBluetoothDeviceInfo &info)
{
    if(info.coreConfigurations() == QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        BleDiscoveredDevice newDevice(info.address().toString(), info.name());
        discoverdDevice(newDevice);
    }
}
