#include <array>

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QLoggingCategory>
#include <QCommandLineParser>
#include <functional>

#include "mainwindow.h"
#include "connectiondialog.h"
#include "bleserial.h"
#include "blescanner.h"
#include "gattlib.h"
#include "sampleparser.h"
#include "callibrationdialog.h"
#include "utilitys.h"


void selectDeviceToConnect(void* blteDevice, BleSerial* bleSerial, MainWindow* w)
{

    BleScanner scanner(blteDevice);
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

    void* blteDevice;
    int errorCode = gattlib_adapter_open(parser.isSet(adapterOption) ? parser.value(adapterOption).toLatin1().data() : NULL, &blteDevice);
    if(errorCode)
    {
        QMessageBox::critical(nullptr, "Error", "No active BLTE Adapter Detected.", QMessageBox::Ok);
        return -1;
    }

    BleSerial bleSerial(blteDevice);
    SampleParser sampleParser;
    bleSerial.setAdcPacketCallback([&sampleParser](const uint8_t *data, size_t length){sampleParser.decodeAdcData(data, length);});
    bleSerial.setAuxPacketCallback([&sampleParser](const uint8_t *data, size_t length){sampleParser.decodeAuxData(data, length);});

    MainWindow w;

    std::function<void(std::vector<AdcSample>::iterator, std::vector<AdcSample>::iterator, unsigned, bool)>windowAdcCb =
        [&w](std::vector<AdcSample>::iterator begin, std::vector<AdcSample>::iterator end, unsigned count, bool reLimit)
        {
            w.newAdcSamples(begin,end,count,reLimit);
        };
    sampleParser.setAdcSampleCallback(windowAdcCb);

    QObject::connect(&w, &MainWindow::openConnDiag, [&blteDevice, &bleSerial, &w](){selectDeviceToConnect(&blteDevice, &bleSerial, &w);});
    QObject::connect(&w, &MainWindow::sigClear, &sampleParser, &SampleParser::clear);
    QObject::connect(&w, &MainWindow::sigSaveCsv, &sampleParser, &SampleParser::saveCsv);
    QObject::connect(&w, &MainWindow::sigLoadCsv, &sampleParser, &SampleParser::loadCsv);
    QObject::connect(&w, &MainWindow::sigReplot, &sampleParser, &SampleParser::resendRange);
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

    CallibrationDialog calDiag(&w);
    QObject::connect(&calDiag, &CallibrationDialog::sigCalValues,  [&bleSerial](std::array<double, 10> in){sendCalValues(&bleSerial, in);});
    QObject::connect(&calDiag, &CallibrationDialog::sigOffset, &w, &MainWindow::setGraphLimit);
    QObject::connect(&calDiag, &CallibrationDialog::sigOffset, &sampleParser, &SampleParser::setOffset);
    QObject::connect(&w, &MainWindow::sigOpenCalDiag, &calDiag, &CallibrationDialog::show);

    w.show();
    return a.exec();
}
