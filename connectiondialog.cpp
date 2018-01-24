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
}

ConnectionDialog::~ConnectionDialog()
{
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
    if(ui->listWidget->selectedItems().size() != 1)
    {
        QMessageBox::warning(this, "Info", "Please Select a device.", QMessageBox::Ok);
    }
    else
    {
        int index = ui->listWidget->row(ui->listWidget->selectedItems()[0]);
         _discoverer->stop();
        deviceSelected(_discoverdDevices[index]);
        done(0);
    }
}

void ConnectionDialog::deviceFound(const BleDiscoveredDevice info)
{
    _discoverdDevices.push_back(info);
    new QListWidgetItem(info.name, ui->listWidget);
}

void ConnectionDialog::stopScan()
{
    _discoverer->stop();
    ui->label_Scanning->setText("Scanning Stopped");
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
