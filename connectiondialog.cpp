#include "connectiondialog.h"
#include "ui_connectiondialog.h"

#include <QMessageBox>
#include <QDebug>

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

ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog),
    _discoverer()
{
    ui->setupUi(this);

    connect(ui->pushButton_scann, &QPushButton::clicked, this, [this](){this->scann();});

}

ConnectionDialog::~ConnectionDialog()
{
    if(_timer != nullptr)
    {
        _timer->stop();
        delete _timer;
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
}

void ConnectionDialog::scanloop()
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 30000;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_discoverer.get_fd(), &fds);
    int err = select(_discoverer.get_fd()+1, &fds, NULL, NULL,  &timeout);

    if(FD_ISSET(_discoverer.get_fd(), &fds))
    {
        //Only read id there's something to read
        std::vector<BLEPP::AdvertisingResponse> tmpFoundDevices = _discoverer.get_advertisements();

        for(unsigned int i = 0; i < tmpFoundDevices.size(); i++)
        {
            qDebug()<<"Found a device: "<<QString::fromStdString(tmpFoundDevices[i].address)<<'\n';

            if(tmpFoundDevices[i].type == BLEPP::LeAdvertisingEventType::ADV_IND || tmpFoundDevices[i].type == BLEPP::LeAdvertisingEventType::ADV_IND )
            {
                qDebug()<<"Is connectable.\n";
                for(unsigned int j = 0; j < tmpFoundDevices[i].UUIDs.size(); j++)
                {
                    qDebug()<<"Has service with UUID: "<<BLEPP::to_str(tmpFoundDevices[i].UUIDs[j]).c_str()<<'\n';
                }
                _discoverdDevices.push_back(tmpFoundDevices[i]);
                new QListWidgetItem(QString::fromStdString(tmpFoundDevices[i].address), ui->listWidget);
            }
        }
    }
}

void ConnectionDialog::scanningFinished()
{
    if(_timer != nullptr)
    {
        _timer->stop();
        delete _timer;
        _timer=nullptr;
    }
    ui->label_Scanning->setText("Scanning Finished");
}

void ConnectionDialog::scann()
{

    if(_timer != nullptr)
    {
        _timer->stop();
        delete _timer;
    }
    _discoverer.stop();

    ui->listWidget->clear();
    _discoverdDevices.clear();

    _discoverer.start();
    ui->label_Scanning->setText("Scanning");
    _timer = new QTimer();
    connect(_timer, &QTimer::timeout, this, &ConnectionDialog::scanloop);
    _timer->setSingleShot(false);
    _timer->setTimerType(Qt::PreciseTimer);
    _timer->start(5);
}
