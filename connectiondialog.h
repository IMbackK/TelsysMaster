#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QTimer>
#include <vector>

#include "blescanner.h"

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
Q_OBJECT
private:
    BleScanner* _discoverer;
    std::vector<BleDiscoveredDevice> _discoverdDevices;

public:
    explicit ConnectionDialog(BleScanner* discoverer, QWidget *parent = 0);
    ~ConnectionDialog();

public slots:
    void toggleScann();
    void stopScan();
    void deviceFound(const BleDiscoveredDevice info);
    void accept();
    void reject();

signals:
    void deviceSelected(const BleDiscoveredDevice info);


private:
    Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
