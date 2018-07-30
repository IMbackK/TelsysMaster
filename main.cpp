#include <array>

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QLoggingCategory>
#include <QCommandLineParser>
#include <functional>
#include <QBluetoothLocalDevice>
#include <QBluetoothAddress>

#include "mainwindow.h"
#include "connectiondialog.h"
#include "bleserial.h"
#include "blescanner.h"
#include "sampleparser.h"
#include "callibrationdialog.h"
#include "utilitys.h"
#include "abouttelsys.h"

void selectDeviceToConnect(BleSerial* bleSerial, MainWindow* w)
{

    BleScanner scanner;
    ConnectionDialog conDiag(&scanner, w);

    QObject::connect(&conDiag, &ConnectionDialog::deviceSelected, bleSerial, &BleSerial::connectTo);

    conDiag.show();
    conDiag.exec();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //set info
    QCoreApplication::setOrganizationName("UVOS");
    QCoreApplication::setOrganizationDomain("uvos.xyz");
    QCoreApplication::setApplicationName("BAMaster");
    QCoreApplication::setApplicationVersion("0.3");

    //parse comand line
    QCommandLineParser parser;
    parser.setApplicationDescription("BTLE strain gauge controller programm");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption adapterOption(QStringList() << "a" << "adapter", QCoreApplication::translate("main", "Use specified BTLE adapter"));
    parser.addOption(adapterOption);
    parser.process(a);

    MainWindow w;
    w.show();

    QBluetoothLocalDevice* device;
    if(parser.isSet(adapterOption)) device = new QBluetoothLocalDevice(QBluetoothAddress(parser.value(adapterOption)));
    else device = new QBluetoothLocalDevice;
    if(!device->isValid())
    {
        QMessageBox::critical(&w, "error", "No Bluetooth device Found", QMessageBox::Ok);
        delete device;
        return 1;
    }
    device->powerOn();
    delete device;

    BleSerial bleSerial;
    SampleParser sampleParser;
    bleSerial.setAdcPacketCallback([&sampleParser](const uint8_t *data, size_t length){sampleParser.decodeAdcData(data, length);});
    bleSerial.setAuxPacketCallback([&sampleParser](const uint8_t *data, size_t length){sampleParser.decodeAuxData(data, length);});

    std::function<void(std::vector<AdcSample>::iterator, std::vector<AdcSample>::iterator, unsigned, bool)>windowAdcCb =
        [&w](std::vector<AdcSample>::iterator begin, std::vector<AdcSample>::iterator end, unsigned count, bool reLimit)
        {
            w.newAdcSamples(begin,end,count,reLimit);
        };
    sampleParser.setAdcSampleCallback(windowAdcCb);

    sampleParser.setAuxSampleCallback([&w](const AuxSample& sample){w.newAuxSample(sample);});

    QObject::connect(&w, &MainWindow::openConnDiag, [&bleSerial, &w](){selectDeviceToConnect(&bleSerial, &w);});
    QObject::connect(&w, &MainWindow::sigClear, &sampleParser, &SampleParser::clear);
    QObject::connect(&w, &MainWindow::sigSaveCsv, &sampleParser, &SampleParser::saveCsv);
    QObject::connect(&w, &MainWindow::sigLoadCsv, &sampleParser, &SampleParser::loadCsv);
    QObject::connect(&w, &MainWindow::sigReplot, &sampleParser, &SampleParser::resendRange);
    QObject::connect(&w, &MainWindow::sigScaleAndOffset, &sampleParser, &SampleParser::setScaleAndOffset);
    QObject::connect(&w, &MainWindow::sigSampleLimit, &sampleParser, &SampleParser::setLimit);
    QObject::connect(&w, &MainWindow::start, [&bleSerial](){sendStart(&bleSerial);});
    QObject::connect(&w, &MainWindow::stop, [&bleSerial](){sendStop(&bleSerial);});
    QObject::connect(&w, &MainWindow::sigReset, [&bleSerial](){sendReset(&bleSerial);});
    QObject::connect(&w, &MainWindow::sigRecalOfset, [&bleSerial](){sendOfset(&bleSerial);});
    QObject::connect(&w, &MainWindow::sigSetRate, [&bleSerial](int rate){sendRate(&bleSerial, rate);});


    QObject::connect(&bleSerial, &BleSerial::deviceConnected, &w, &MainWindow::deviceConnected);
    QObject::connect(&bleSerial, &BleSerial::deviceDisconnected, &w, &MainWindow::connectionFailed);
    QObject::connect(&bleSerial, &BleSerial::deviceConnectionInProgress, &w, &MainWindow::deviceConnectionInProgress);
    QObject::connect(&w, &MainWindow::sigDeviceDisconnect, &bleSerial, &BleSerial::deviceDisconnect);

    QObject::connect(&a, &QApplication::lastWindowClosed, [&bleSerial, &a](){bleSerial.disconnect(); a.quit();}); //needed for android.

    CallibrationDialog calDiag(&w);
    QObject::connect(&calDiag, &CallibrationDialog::sigCalValues,  [&bleSerial](std::array<double, 10> in){sendCalValues(&bleSerial, in);});
    QObject::connect(&w, &MainWindow::sigOpenCalDiag, &calDiag, &CallibrationDialog::show);

    return a.exec();
}
