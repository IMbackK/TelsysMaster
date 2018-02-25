#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QMenu>
#include "sampleparser.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    bool wasConnected = false;

    int sampleMemoryLimit = 100000;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void newAdcSamples(std::vector<AdcSample>::iterator begin, std::vector<AdcSample>::iterator end, unsigned number, bool reLimit);

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
    void sigSetRate(int rate);
    void sigSampleLimit(int limit);


public slots:
    void deviceConnected();
    void deviceConnectionInProgress();
    void deviceDisconnected();
    void connectionFailed(QString errorMsg);
    void setGraphLimit(double in);
    void showRateDialog();

private slots:

    //samples
    void savePdf();
    void saveCsv();
    void loadCsv();
    void clearGraphAndListView();

    //virtual void show();

    //dialogs
    void showLimitDialog();
    void replot();

private:
    void onStart();
    void onStop();
    void toggleConnectState();
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
