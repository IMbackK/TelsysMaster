#include <QDebug>

#include "blescanner.h"

#include <QThread>
#include <QCoreApplication>
#include <vector>

BleScanner::BleScanner(): _scanner(false, BLEPP::HCIScanner::FilterDuplicates::Software, BLEPP::HCIScanner::ScanType::Active )
{
}

BleScanner::~BleScanner()
{
    stop();
    if( _scannThread != nullptr ) delete _scannThread;
}

void BleScanner::start()
{
    _scanner.start();
    qRegisterMetaType<BleDiscoveredDevice>();
    _scannThread = QThread::create([this](){_scanner.start(); this->scannWorker();});
    connect(_scannThread, &QThread::finished, this, &BleScanner::scanningThreadFinished);
    _scannThread->start();
}

void BleScanner::scannWorker()
{
    std::vector<BLEPP::AdvertisingResponse> advertisments = _scanner.get_advertisements();

    for(unsigned i = 0; i < advertisments.size(); i++)
    {
        BleDiscoveredDevice tmp;
        tmp.address = QString::fromUtf8(advertisments[i].address.c_str());
        tmp.name = "nan";//QString::fromUtf8(advertisments[i].local_name->name.c_str());
        discoverdDevice(tmp);
    }
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
