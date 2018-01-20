#include "connectiondialog.h"
#include "ui_connectiondialog.h"

#include <QListWidgetItem>
#include <QDebug>
#include <QMessageBox>

ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton_scann, &QPushButton::clicked, this, [this](){this->scann();});
    connect(&_discoverer, &QBluetoothDeviceDiscoveryAgent::finished, this, [this](){this->scanningFinished();});
    connect(&_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, [this](const QBluetoothDeviceInfo &info){this->addDevice(info);});

    //connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this](){this->isAccepted();});
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
         _discoverer.stop();
        deviceSelected(_discoverdDevices[index]);
        done(0);
    }

    //ui->listWidget->
}

void ConnectionDialog::addDevice(const QBluetoothDeviceInfo &info)
{
    if(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        new QListWidgetItem(info.name(), ui->listWidget);
        _discoverdDevices.append(info);
        qDebug()<<"found device\n";
    }
    else qDebug()<<"found device not le\n";
}

void ConnectionDialog::scanningFinished()
{
    ui->label_Scanning->setText("Scanning Finished");
}

void ConnectionDialog::scann()
{

    _discoverer.stop();

    ui->listWidget->clear();
    _discoverdDevices.clear();

    _discoverer.start();
    ui->label_Scanning->setText("Scanning");
}
