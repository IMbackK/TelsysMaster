#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QTimer>
#include <vector>

#include "blepp/lescan.h"

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
Q_OBJECT
private:
    BLEPP::HCIScanner _discoverer;
    std::vector<BLEPP::AdvertisingResponse> _discoverdDevices;

    QTimer* _timer = nullptr;

public:
    explicit ConnectionDialog(QWidget *parent = 0);
    ~ConnectionDialog();

public slots:
    void scann();
    void scanningFinished();
    void accept();
    void reject();
    void scanloop();

signals:
    void deviceSelected(const BLEPP::AdvertisingResponse info);


private:
    Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
