#include "connectiondialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QTextStream>
#include <QString>
#include <QFileDialog>
#include <QPen>
#include <QColor>
#include <replotdiag.h>

#include "plot.h"

#include "limitdialog.h"
#include "ratedialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->plot->setLable("ADC Value");
    ui->plot_accl->setLable("Accelleration [m/s^2]");

    ui->plot_accl->setMaxValue(100);

    //menu actions
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionSave_PNG, &QAction::triggered, this, &MainWindow::savePdf);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clearGraphs);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::sigClear);
    connect(ui->actionLimits, &QAction::triggered, this, &MainWindow::showLimitDialog);
    connect(ui->actionSave_CSV, &QAction::triggered, this, &MainWindow::saveCsv);
    connect(ui->actionLoad_CSV, &QAction::triggered, this, &MainWindow::loadCsv);
    connect(ui->actionCalibration, &QAction::triggered, this, &MainWindow::sigOpenCalDiag);
    connect(ui->actionReplot, &QAction::triggered, this, &MainWindow::replot);
    connect(ui->actionRecalibrate_Offset, &QAction::triggered, this, &MainWindow::sigRecalOfset);
    connect(ui->actionRates, &QAction::triggered, this, &MainWindow::showRateDialog);

    //buttons
    connect(ui->pushButton_Connect, &QAbstractButton::clicked, this, &MainWindow::openConnDiag);
    connect(ui->pushButton_disconnect,  &QAbstractButton::clicked, this, &MainWindow::sigDeviceDisconnect);
    connect(ui->pushButton_disconnect,  &QAbstractButton::clicked, this, &MainWindow::deviceDisconnected);
    connect(ui->pushButton_run,  &QAbstractButton::clicked, this, &MainWindow::start);
    connect(ui->pushButton_stop,  &QAbstractButton::clicked, this, &MainWindow::stop);
    connect(ui->pushButton_Reset, &QAbstractButton::clicked, this, &MainWindow::sigReset);
    connect(ui->pushButton_Reset,  &QAbstractButton::clicked, this, &MainWindow::sigDeviceDisconnect);
    connect(ui->pushButton_Reset,  &QAbstractButton::clicked, this, &MainWindow::deviceDisconnected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setGraphLimit(double in)
{
    ui->plot->yAxis->setRange(0, 65535*in);
    ui->plot->replot();
}

void MainWindow::deviceConnected()
{
    ui->label_Connected->setText("Connected");
    ui->pushButton_Connect->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_run->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
    ui->pushButton_Reset->setEnabled(true);
    ui->actionRates->setEnabled(true);
    ui->actionRecalibrate_Offset->setEnabled(true);
    ui->actionLoad_CSV->setEnabled(false);
    wasConnected = true;
    clearGraphs();
    sigClear();
}

void MainWindow::deviceDisconnected()
{
    ui->label_Connected->setText("Disconnected");
    if(wasConnected)QMessageBox::information(this, "Info", "Due to a bug in gattlib it is currently not possible to reconnect. Please restart the application to reconnect to device.", QMessageBox::Ok);
    else ui->pushButton_Connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
    ui->pushButton_run->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_Reset->setEnabled(false);
    ui->pushButton_Reset->setEnabled(false);
    ui->actionLoad_CSV->setEnabled(true);

    ui->actionRates->setEnabled(false);
    ui->actionRecalibrate_Offset->setEnabled(false);
}

void MainWindow::deviceConnectionInProgress()
{
    ui->label_Connected->setText("Connecting...");
    ui->pushButton_Connect->setEnabled(false);
}

void MainWindow::clearGraphs()
{
    //adc view
    ui->plot->clear();
    ui->lcdNumber_Samples->display(0);
    ui->lcdNumber_SampleRate->display(0);
    ui->label_LatestSample->setText("");

    //aux view
    ui->plot_accl->clear();
    ui->label_LatestAuxSample->setText("");

}

void MainWindow::newAuxSample(const AuxSample& sample)
{
    QString buffer;
    QTextStream ss(&buffer);

    Point3D<double> scaledAccel = sample.accel.scale(sample.accelScale);

    ui->plot_accl->addData(sample.timeStamp/(float)1000000, scaledAccel.amplitude());

    ss<<"Timestamp: "<<sample.timeStamp<<" Acceleration Amplitude: "<<scaledAccel.amplitude()
      <<"  Acceleration xyz: "<<scaledAccel.x<<','<<scaledAccel.y<<','<<scaledAccel.z
      <<"  Magnetic vec xyz: "<<sample.magn.x<<','<<sample.magn.y<<','<<sample.magn.z
      <<"  Temperature: "<<sample.temperature;
    ui->label_LatestAuxSample->setText(buffer);

    if(!ui->tab_2->isHidden()) ui->plot_accl->replot(QCustomPlot::rpQueuedRefresh);
}

void MainWindow::newAdcSamples(std::vector<AdcSample>::iterator begin, std::vector<AdcSample>::iterator end, unsigned number, bool reLimit)
{
    if((end-1)->deltaTime != 0)ui->lcdNumber_SampleRate->display(1000000/(float)(end-1)->deltaTime);
    ui->lcdNumber_Samples->display((int)number);
    if(end-begin > sampleMemoryLimit) sampleMemoryLimit = end-begin;
    ui->lcdNumber_bufferPercent->display((number/(float)sampleMemoryLimit)*100);

    QString buffer;
    QTextStream ss(&buffer);
    ss<<"id: "<<(end-1)->id<<"  Value: "<<(end-1)->value<<"  dt: "<<(end-1)->deltaTime<<"  timestamp: "<<(end-1)->timeStamp;
    ui->label_LatestSample->setText(buffer);

    QVector<double> keys;
    keys.reserve(end-begin);
    QVector<double> values;
    values.reserve(end-begin);
    for(std::vector<AdcSample>::iterator item = ((end-begin) > ui->plot->getLimit() && !reLimit) ? end-ui->plot->getLimit() : begin; item < end; ++item)
    {
        keys.push_back(item->timeStamp/(double)1000000);
        values.push_back(item->value);
    }

    ui->plot->addData(keys, values, true, reLimit);
    if(!ui->tab->isHidden())ui->plot->replot(QCustomPlot::rpQueuedRefresh);
}

void MainWindow::showLimitDialog()
{
    LimitDialog limitDialog(this);

    limitDialog.setGraphPointLimit(ui->plot->getLimit());
    limitDialog.setSampleMemoryLimit(sampleMemoryLimit);
    limitDialog.show();

    if(limitDialog.exec() == 0)
    {
        sigSampleLimit(limitDialog.getSampleMemoryLimit());

        uint64_t samplenumber = ui->lcdNumber_bufferPercent->value()*sampleMemoryLimit;
        sampleMemoryLimit = limitDialog.getSampleMemoryLimit();
        ui->lcdNumber_bufferPercent->display((int)(samplenumber/(float)sampleMemoryLimit*100));

        ui->plot->setLimit(limitDialog.getGraphLimit());
        ui->plot_accl->setLimit(limitDialog.getGraphLimit()/10);
        ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
    }
}

void MainWindow::showRateDialog()
{
    RateDialog rateDialog(this);
    rateDialog.show();
    if(rateDialog.exec() == 0)
    {
        sigSetRate(rateDialog.getRate());
    }
}

void MainWindow::tabChanged()
{
    ui->plot->replot();
    ui->plot_accl->replot();
}

void MainWindow::savePdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save graph as PDF", "./", "*.pdf" );
    if(!fileName.isEmpty())
    {
        if(ui->tabWidget->currentIndex() == 0)ui->plot->savePdf(fileName);
        else ui->plot_accl->savePdf(fileName);
    }
}

void MainWindow::saveCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save data as CSV", "./", "*.csv" );
    if(!fileName.isEmpty()) sigSaveCsv(fileName);
}

void MainWindow::loadCsv()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load CSV", "./", "*.csv" );
    if(!fileName.isEmpty())
    {
        clearGraphs();
        sigLoadCsv(fileName);
    }
}

void MainWindow::replot()
{
    if(ui->lcdNumber_Samples->value() != 0)
    {

        ReplotDiag replotDiag(ui->lcdNumber_Samples->value(), this);
        replotDiag.show();
        if(replotDiag.exec() == 0 && replotDiag.getFrom() < replotDiag.getTo())
        {
            clearGraphs();
            ui->plot->setLimit(replotDiag.getTo() - replotDiag.getFrom());
            sigReplot(replotDiag.getFrom(), replotDiag.getTo());
        }
    }
    else QMessageBox::warning(this, "Warning", "No samples", QMessageBox::Ok);
}

void MainWindow::connectionFailed(QString errorMsg)
{
    QMessageBox::warning(this, "Error", errorMsg, QMessageBox::Ok);
    deviceDisconnected();
}
