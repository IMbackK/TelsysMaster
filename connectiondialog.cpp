#include "connectiondialog.h"
#include "ui_connectiondialog.h"

#include <QMessageBox>
#include <QDebug>
#include <QListWidgetItem>


#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <cerrno>
#include <unistd.h>

#include "blepp/logging.h"
#include "blepp/pretty_printers.h"
#include "blepp/blestatemachine.h"
#include "blepp/lescan.h"

ConnectionDialog::ConnectionDialog(BleScanner* discoverer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog),
    _discoverer(discoverer)
{
    ui->setupUi(this);

    connect(ui->pushButton_scann, &QPushButton::clicked, this, [this](){this->toggleScann();});
    connect(_discoverer, &BleScanner::discoverdDevice, this, &ConnectionDialog::deviceFound);
    connect(_discoverer, &BleScanner::finishedScanning, this, &ConnectionDialog::scannFinished);

}

ConnectionDialog::~ConnectionDialog()
{
    if(_discoverer->getScanning())
    {
        stopScan();
    }
    ui->listWidget->clear();
    _discoverdDevices.clear();
    delete ui;

}

void ConnectionDialog::reject()
{
    ui->listWidget->clear();
    _discoverdDevices.clear();
    done(0);
}

void ConnectionDialog::accept()
{
    if(ui->listWidget->selectedItems().size() != 1 && ui->lineEdit->text().isEmpty() )
    {
        QMessageBox::warning(this, "Info", "Please Select a device or input a valid custom address.", QMessageBox::Ok);
    }
    else
    {
        if(ui->listWidget->selectedItems().size() == 1)
        {
            int index = ui->listWidget->row(ui->listWidget->selectedItems()[0]);
             _discoverer->stop();
            deviceSelected(_discoverdDevices[index]);

        }
        else
        {
            BleDiscoveredDevice device(ui->lineEdit->text(), "Custom Device");
            _discoverer->stop();
            deviceSelected(device);
        }
        done(0);
    }
}


void ConnectionDialog::deviceFound(const BleDiscoveredDevice info)
{
    _discoverdDevices.push_back(info);

    QString itemString;

    if(!info.name.isEmpty())
    {
        itemString =info.name;
        itemString.append("    Addr: ");
        itemString.append(info.address);
    }
    else itemString = info.address;
    new QListWidgetItem(itemString, ui->listWidget);
}

void ConnectionDialog::stopScan()
{
    _discoverer->stop();
    scannFinished();
}


void ConnectionDialog::scannFinished()
{
    ui->label_Scanning->setText("Scanning Finished");
    ui->pushButton_scann->setText("Scann");
}

void ConnectionDialog::toggleScann()
{
    if(_discoverer->getScanning())
    {
        stopScan();
    }
    else
    {
        ui->listWidget->clear();
        _discoverdDevices.clear();
        _discoverer->start();
        ui->pushButton_scann->setText("Stop");
        ui->label_Scanning->setText("Scanning...");
    }
}
