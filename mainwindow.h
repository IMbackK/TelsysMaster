#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sampleparser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    bool wasConnected = false;

    int graphPointLimit = 10000;
    int listViewPointLimit = 100;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void openConnDiag();
    void stop();
    void start();
    void sigReset();
    void sigDeviceDisconnect();
    void sigClear();
    void sigSaveCsv(QString fileName);
    void sigLoadCsv(QString fileName);
    void sigOpenCalDiag();
    void sigReplot(unsigned from, unsigned to);
    void sigRecalOfset();

public slots:
    void deviceConnected();
    void deviceConnectionInProgress();
    void deviceDisconnected();
    void connectionFailed(QString errorMsg);
    void setGraphPointLimit(int in);
    void setListViewPointLimit(int in);
    void setGraphLimit(double in);
    void newAdcSample(AdcSample sample, int amountNowStored);

private slots:


    void savePdf();
    void saveCsv();
    void loadCsv();
    void clearGraphAndListView();
    void showLimitDialog();
    void replot();

private:
    void onStart();
    void onStop();
    void toggleConnectState();
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
