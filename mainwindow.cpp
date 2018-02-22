#include "connectiondialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QTextStream>
#include <QListWidgetItem>
#include <QString>
#include <QFileDialog>
#include <QPen>
#include <QColor>
#include <replotdiag.h>

#include "qcustomplot/qcustomplot.h"

#include "limitdialog.h"
#include "ratedialog.h"
#include "statisticsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    graphContextMenu(this),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //menu actions
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
    ui->plot->setInteraction(QCP::iSelectPlottables, true);
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom);
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->plot->setSelectionRectMode(QCP::srmNone);
    ui->plot->graph(0)->setSelectable(QCP::stDataRange);
    QPen selectionPen = ui->plot->graph(0)->pen();
    selectionPen.setColor(QColor(255,0,0));
    ui->plot->graph(0)->selectionDecorator()->setPen(selectionPen);

    connect(ui->plot, &QCustomPlot::mousePress, this, &MainWindow::graphMousePressed);
    connect(ui->plot, &QCustomPlot::mouseRelease, this, &MainWindow::graphMouseReleased);

    //graph context menu
    graphContextMenu.addAction(ui->actionStatistics);
    graphContextMenu.addAction(ui->actionAdd_Regression);
    graphContextMenu.addAction(ui->actionDelete_Regression);
    graphContextMenu.addAction(ui->actionExport_Selection);

    connect(ui->actionStatistics, &QAction::triggered, this, &MainWindow::showStatistics);
    connect(ui->actionAdd_Regression, &QAction::triggered, this, &MainWindow::addRegression);
    connect(ui->actionDelete_Regression, &QAction::triggered, this, &MainWindow::deleteRegression);

    //sample list
    ui->listWidget->setUniformItemSizes(true);
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
    //ui->actionCallibration->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
    ui->pushButton_Reset->setEnabled(true);
    ui->actionRates->setEnabled(true);
    ui->actionRecalibrate_Offset->setEnabled(true);
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

    ui->actionRates->setEnabled(false);
    ui->actionRecalibrate_Offset->setEnabled(false);
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
    if(number > sampleMemoryLimit) sampleMemoryLimit = number;

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

void MainWindow::newAdcSamples(std::vector<AdcSample>::iterator begin, std::vector<AdcSample>::iterator end, unsigned number, bool reLimit)
{
    ui->lcdNumber_Samples->display((int)number);
    if(end-begin > sampleMemoryLimit) sampleMemoryLimit = end-begin;

    for(std::vector<AdcSample>::iterator item = end-begin > listViewPointLimit ? end-listViewPointLimit : begin; item < end; item++ )
    {
        QString buffer;
        QTextStream ss(&buffer);
        ss<<item->id<<": "<<item->value<<"   |   dt: "<<item->deltaTime<<" | "<<item->timeStamp;
        new QListWidgetItem( *ss.string(), ui->listWidget );
        if(ui->listWidget->count() > listViewPointLimit) delete ui->listWidget->takeItem(0);
        ui->listWidget->scrollToBottom();
    }

    if(reLimit && (end-begin) > graphPointLimit) graphPointLimit = end-begin;

    QVector<double> keys;
    keys.reserve(end-begin);
    QVector<double> values;
    values.reserve(end-begin);
    for(std::vector<AdcSample>::iterator item = (end-begin) > graphPointLimit ? end-graphPointLimit : begin; item < end; item++)
    {
        keys.push_back(item->timeStamp/(double)1000000);
        values.push_back(item->value);
    }

    ui->plot->graph(0)->addData(keys, values, true);
    QSharedPointer<QCPDataContainer<QCPGraphData>> data = ui->plot->graph(0)->data();
    while(data->size() > graphPointLimit) data->remove(data->begin()->key);
    ui->plot->xAxis->rescale();
    ui->plot->replot();
}

void MainWindow::showLimitDialog()
{
    LimitDialog limitDialog(this);

    limitDialog.setGraphPointLimit(graphPointLimit);
    limitDialog.setListViewPointLimit(listViewPointLimit);
    limitDialog.setSampleMemoryLimit(sampleMemoryLimit);
    limitDialog.show();

    if(limitDialog.exec() == 0)
    {
        sigSampleLimit(limitDialog.getSampleMemoryLimit());

        uint64_t samplenumber = ui->lcdNumber_bufferPercent->value()*sampleMemoryLimit;
        sampleMemoryLimit = limitDialog.getSampleMemoryLimit();
        ui->lcdNumber_bufferPercent->display((int)(samplenumber/(float)sampleMemoryLimit*100));

        graphPointLimit = limitDialog.getGraphLimit();
        QSharedPointer<QCPDataContainer<QCPGraphData>> data = ui->plot->graph(0)->data();
        while(data->size() > graphPointLimit) data->remove(data->begin()->key);
        ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);

        listViewPointLimit = limitDialog.getListLimit();
        while(ui->listWidget->count() > listViewPointLimit) delete ui->listWidget->takeItem(0);
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
            graphPointLimit = replotDiag.getTo() - replotDiag.getFrom();
            sigReplot(replotDiag.getFrom(), replotDiag.getTo());
        }
    }
    else QMessageBox::warning(this, "Warning", "No samples", QMessageBox::Ok);
}

void MainWindow::graphMousePressed(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() == Qt::Modifier::SHIFT) )
    {
        setCursor(Qt::ClosedHandCursor);
        ui->plot->setSelectionRectMode(QCP::srmNone);
    }
    else if(event->button() == Qt::LeftButton)
    {
        ui->plot->setSelectionRectMode(QCP::srmSelect);
    }
    else if(event->button() == Qt::RightButton)
    {
        graphContextMenu.popup(event->globalPos());
        ui->plot->setSelectionRectMode(QCP::srmNone);
    }
}

void MainWindow::graphMouseReleased(QMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
}

void MainWindow::showStatistics()
{
    if(!ui->plot->graph(0)->selection().dataRanges().at(0).isEmpty())
    {
        StatisticsDialog statDiag(ui->plot->graph(0)->selection().dataRanges().at(0), ui->plot->graph(0)->data(), this);
        statDiag.show();
        statDiag.exec();
    }
    else QMessageBox::warning(this, "Warning", "No selection has been made", QMessageBox::Ok);
}

void MainWindow::addRegression()
{
    if(!ui->plot->graph(0)->selection().dataRanges().at(0).isEmpty())
    {
        QCPDataRange range = ui->plot->graph(0)->selection().dataRanges().at(0);
        QSharedPointer< QCPGraphDataContainer > data = ui->plot->graph(0)->data();

        QCPGraphDataContainer::const_iterator begin = data->at(range.begin());
        QCPGraphDataContainer::const_iterator end = data->at(range.end());

        double sumValues = 0;
        double sumKeys = 0;
        double sumValuesTimesKeys = 0;
        double sumSquaredKeys = 0;
        for (QCPGraphDataContainer::const_iterator item=begin; item != end; item++)
        {
            sumValues += item->value;
            sumKeys += item->key;
            sumSquaredKeys += item->key*item->key;
            sumValuesTimesKeys += item->value*item->key;
        }

        double slope = (sumValuesTimesKeys - (sumKeys*sumValues)/range.size()) / (sumSquaredKeys - (sumKeys*sumKeys)/range.size());
        double offset = sumValues/range.size() - slope*(sumKeys/range.size());

        QPen regressionPen = ui->plot->graph(0)->pen();
        regressionPen.setColor(QColor(0,255,0));

        QPen selectionPen = ui->plot->graph(0)->pen();
        selectionPen.setColor(QColor(255,0,0));

        QCPGraphDataContainer::const_iterator center = begin + (end - begin)/2;

        ui->plot->addGraph();
        ui->plot->graph(ui->plot->graphCount()-1)->setPen(regressionPen);
        ui->plot->graph(ui->plot->graphCount()-1)->addData(100+center->key, slope*(100+center->key) + offset);
        ui->plot->graph(ui->plot->graphCount()-1)->addData(-100+center->key, slope*(-100+center->key) + offset);
        ui->plot->graph(ui->plot->graphCount()-1)->selectionDecorator()->setPen(selectionPen);
        ui->plot->replot();

    }
    else QMessageBox::warning(this, "Warning", "No selection has been made", QMessageBox::Ok);
}

void MainWindow::deleteRegression()
{
    if( ui->plot->selectedGraphs().size() > 1 && ui->plot->selectedGraphs().at(1) != ui->plot->graph(0))
    {
        ui->plot->removeGraph(ui->plot->selectedGraphs().at(1));
        ui->plot->replot();
    }
}

void MainWindow::connectionFailed(QString errorMsg)
{
    QMessageBox::warning(this, "Error", errorMsg, QMessageBox::Ok);
    deviceDisconnected();
}
