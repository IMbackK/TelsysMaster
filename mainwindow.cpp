#include "connectiondialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QTextStream>
#include <QListWidgetItem>
#include <QString>
#include <QFileDialog>
#include <replotdiag.h>

#include "qcustomplot/qcustomplot.h"

#include "limitdialog.h"
#include "ratedialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //actions
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionSave_PNG, &QAction::triggered, this, &MainWindow::savePdf);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clearGraphAndListView);
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

    //plot
    ui->plot->xAxis->setLabel("Time [s]");
    ui->plot->yAxis->setLabel("ADC value");
    ui->plot->xAxis->setRange(0, 10);
    ui->plot->yAxis->setRange(0, 65535);
    ui->plot->addGraph();
    ui->plot->setOpenGl(true);

    //sample list
    ui->listWidget->setUniformItemSizes(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setGraphPointLimit(int in)
{
    graphPointLimit = in;
    QSharedPointer<QCPDataContainer<QCPGraphData>> data = ui->plot->graph(0)->data();
    while(data->size() > graphPointLimit) data->remove(data->begin()->key);
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MainWindow::setGraphLimit(double in)
{
    ui->plot->yAxis->setRange(0, 65535*in);
    ui->plot->replot();
}

void MainWindow::setListViewPointLimit(int in)
{
    listViewPointLimit = in;
    while(ui->listWidget->count() > listViewPointLimit) delete ui->listWidget->takeItem(0);
}

void MainWindow::deviceConnected()
{
    ui->label_Connected->setText("Connected");
    ui->pushButton_Connect->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_run->setEnabled(true);
    //ui->actionCallibration->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
    ui->pushButton_Reset->setEnabled(true);
    wasConnected = true;
    clearGraphAndListView();
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
    //ui->actionCallibration->setEnabled(false);
    ui->pushButton_Reset->setEnabled(false);
}

void MainWindow::deviceConnectionInProgress()
{
    ui->label_Connected->setText("Connecting...");
    ui->pushButton_Connect->setEnabled(false);
}

void MainWindow::clearGraphAndListView()
{
    ui->listWidget->clear();
    ui->plot->graph(0)->data().data()->clear();
    ui->plot->xAxis->setRange(0, 10);
    ui->plot->replot();

    ui->lcdNumber_Samples->display(0);
    ui->lcdNumber_SampleRate->display(0);
}

void MainWindow::newAdcSample(AdcSample sample, int number)
{

    ui->lcdNumber_Samples->display(number);

    ui->lcdNumber_SampleRate->display((1/((float)sample.deltaTime/1000000)));

    QString buffer;
    QTextStream ss(&buffer);
    ss<<sample.id<<": "<<sample.value<<"   |   dt: "<<sample.deltaTime<<" | "<<sample.timeStamp;
    new QListWidgetItem( *ss.string(), ui->listWidget );
    if(ui->listWidget->count() > listViewPointLimit) delete ui->listWidget->takeItem(0);
    ui->listWidget->scrollToBottom();

    ui->plot->graph(0)->addData(((double)sample.timeStamp)/1000000, sample.value);
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
    QSharedPointer<QCPDataContainer<QCPGraphData>> data = ui->plot->graph(0)->data();
    if(data->size() > graphPointLimit) data->remove(data->begin()->key);

    if(sample.timeStamp/1000000 > 10)   ui->plot->xAxis->rescale();

}

void MainWindow::showLimitDialog()
{
    LimitDialog limitDialog(this);

    limitDialog.setGraphPointLimit(graphPointLimit);
    limitDialog.setListViewPointLimit(listViewPointLimit);

    connect(&limitDialog, &LimitDialog::graphPointLimit, this, &MainWindow::setGraphPointLimit);
    connect(&limitDialog, &LimitDialog::listViewPointLimit, this, &MainWindow::setListViewPointLimit);

    limitDialog.show();
    limitDialog.exec();
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

void MainWindow::savePdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save graph as PDF", "./", "*.pdf" );
    if(!fileName.isEmpty()) ui->plot->savePdf(fileName);
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
        clearGraphAndListView();
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
            clearGraphAndListView();
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
