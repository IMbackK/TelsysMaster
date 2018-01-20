#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QList>


namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
Q_OBJECT
private:
    QBluetoothDeviceDiscoveryAgent _discoverer;
    QList<QBluetoothDeviceInfo> _discoverdDevices;

public:
    explicit ConnectionDialog(QWidget *parent = 0);
    ~ConnectionDialog();

public slots:
    void addDevice(const QBluetoothDeviceInfo &info);
    void scann();
    void scanningFinished();
    void accept();
    void reject();

signals:
    void deviceSelected(const QBluetoothDeviceInfo info);


private:
    Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
