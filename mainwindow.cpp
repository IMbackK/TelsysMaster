#include "connectiondialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QTextStream>
#include <QString>
#include <QFileDialog>
#include <QPen>
#include <QColor>


#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

#include "plot.h"

#include "replotdiag.h"
#include "limitdialog.h"
#include "ratedialog.h"
#include "scaleoffsetdiag.h"
#include "abouttelsys.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->plot->setLabel("ADC Value");
    ui->plot_accl->setLabel("Accelleration [m/s^2]");

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
    connect(ui->actionAbout_Qt, &QAction::triggered, this, [this](){QMessageBox::aboutQt(this);});
    connect(ui->actionScale_and_Offset, &QAction::triggered, this, &MainWindow::showScaleOffsetDialog);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDiag);

    //buttons
    connect(ui->pushButton_Connect, &QAbstractButton::clicked, this, &MainWindow::openConnDiag);
    connect(ui->pushButton_disconnect,  &QAbstractButton::clicked, this, &MainWindow::sigDeviceDisconnect);
    connect(ui->pushButton_disconnect,  &QAbstractButton::clicked, this, &MainWindow::deviceDisconnected);
    connect(ui->pushButton_run,  &QAbstractButton::clicked, this, &MainWindow::start);
    connect(ui->pushButton_stop,  &QAbstractButton::clicked, this, &MainWindow::stop);
    connect(ui->pushButton_Reset, &QAbstractButton::clicked, this, &MainWindow::sigReset);
    connect(ui->pushButton_Reset,  &QAbstractButton::clicked, this, &MainWindow::sigDeviceDisconnect);
    connect(ui->pushButton_Reset,  &QAbstractButton::clicked, this, &MainWindow::deviceDisconnected);


    //submenus dont work sanely on android
#ifdef Q_OS_ANDROID

    ui->actionSave_CSV->setText("CSV");
    ui->actionSave_PNG->setText("PDF");
    ui->actionRecalibrate_Offset->setText("Recal");
    ui->actionCalibration->setText("Cal");
    ui->actionScale_and_Offset->setText("Scl&Ofst");

    ui->menuBar->addAction(ui->actionSave_CSV);
    ui->menuBar->addAction(ui->actionSave_PNG);
    ui->menuBar->addAction(ui->actionRecalibrate_Offset);
    ui->menuBar->addAction(ui->actionCalibration);
     ui->menuBar->addAction(ui->actionScale_and_Offset);
    ui->menuBar->addAction(ui->actionReplot);
    ui->menuBar->addAction(ui->actionRates);
    ui->menuBar->addAction(ui->actionClear);
    ui->menuBar->addAction(ui->actionAbout);

    ui->menuBar->removeAction(ui->menuConfigure->menuAction());
    ui->menuBar->removeAction(ui->menuhello->menuAction());
    ui->menuBar->removeAction(ui->menuHelp->menuAction());
    ui->menuBar->removeAction(ui->menuSamples->menuAction());

    QFont font = ui->menuBar->font();
    font.setPointSize(24);
    ui->menuBar->setFont(font);

    ui->label_LatestAuxSample->hide();
    ui->label_LatestSample->hide();
    ui->menuBar->setNativeMenuBar(false);

#endif

}

MainWindow::~MainWindow()
{
    children();
    delete ui;
}

void MainWindow::setScaleAndOffset(double scale, double offset)
{
    ui->plot->yAxis->setRange(offset*scale, (65535+offset)*scale);
    sigReplot(0, sampleMemoryLimit);
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
    ui->actionCalibration->setEnabled(true);
    wasConnected = true;
    clearGraphs();
    sigClear();
}

void MainWindow::deviceDisconnected()
{
    ui->label_Connected->setText("Disconnected");
    ui->pushButton_Connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
    ui->pushButton_run->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_Reset->setEnabled(false);
    ui->pushButton_Reset->setEnabled(false);
    ui->actionLoad_CSV->setEnabled(true);
    ui->actionCalibration->setEnabled(false);
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

    #ifndef Q_OS_ANDROID
    ui->label_LatestSample->setText("");
    #endif

    //aux view
    ui->plot_accl->clear();
    #ifndef Q_OS_ANDROID
    ui->label_LatestAuxSample->setText("");
    #endif

}

void MainWindow::newAuxSample(const AuxSample& sample)
{
    Point3D<double> scaledAccel = sample.accel.scale(sample.accelScale);

    ui->plot_accl->addData(sample.timeStamp/(float)1000000, scaledAccel.amplitude());

    #ifndef Q_OS_ANDROID
    QString buffer;
    QTextStream ss(&buffer);
    ss<<"Timestamp: "<<sample.timeStamp<<"  Temperature: "<<sample.temperature<<" Acceleration Amplitude: "<<scaledAccel.amplitude();
    ui->label_LatestAuxSample->setText(buffer);
    #endif

    if(!ui->tab_2->isHidden()) ui->plot_accl->replot(QCustomPlot::rpQueuedRefresh);
}

void MainWindow::newAdcSamples(std::vector<AdcSample>::iterator begin, std::vector<AdcSample>::iterator end, unsigned number, bool reLimit)
{
    if((end-1)->deltaTime != 0)ui->lcdNumber_SampleRate->display(1000000/(float)(end-1)->deltaTime);
    ui->lcdNumber_Samples->display((int)number);
    if(end-begin > sampleMemoryLimit) sampleMemoryLimit = end-begin;
    ui->lcdNumber_bufferPercent->display((number/(float)sampleMemoryLimit)*100);

    #ifndef Q_OS_ANDROID
    QString buffer;
    QTextStream ss(&buffer);
    ss<<"id: "<<(end-1)->id<<"  Value: "<<(end-1)->getScaledValue()<<"  dt: "<<(end-1)->deltaTime<<"  timestamp: "<<(end-1)->timeStamp;
    ui->label_LatestSample->setText(buffer);
    #endif

    QVector<double> keys;
    keys.reserve(end-begin);
    QVector<double> values;
    values.reserve(end-begin);
    for(std::vector<AdcSample>::iterator item = ((end-begin) > ui->plot->getLimit() && !reLimit) ? end-ui->plot->getLimit() : begin; item < end; ++item)
    {
        keys.push_back(item->timeStamp/(double)1000000);
        values.push_back(item->getScaledValue());
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

void MainWindow::showAboutDiag()
{
    AboutTelsys aboutDiag(this);
    aboutDiag.show();
    aboutDiag.exec();
}

void MainWindow::showScaleOffsetDialog()
{
    ScaleOffsetDiag scaleDiag(this);
    scaleDiag.show();
    if(scaleDiag.exec() == 0)
    {
        setScaleAndOffset(scaleDiag.getScale(), scaleDiag.getOffset());
        if(!scaleDiag.isManual()) ui->plot->setLabel("Deflection [mV/V]");
        ui->plot->replot();
        if(scaleDiag.rescale())
        {
            uint64_t sampleNumber = ui->lcdNumber_Samples->value();
            sigScaleAndOffset(scaleDiag.getScale(), scaleDiag.getOffset(), false);
            clearGraphs();
            sigReplot(0, sampleNumber);
        }
        else sigScaleAndOffset(scaleDiag.getScale(), scaleDiag.getOffset(), true);
    }
}

void MainWindow::tabChanged()
{
    ui->plot->replot();
    ui->plot_accl->replot();
}

void MainWindow::savePdf()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
    QAndroidJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );

    QString fileName = mediaPath.toString()+"/graph.pdf";

    QMessageBox::information(this, "Saved", "saved to "+fileName, QMessageBox::Ok);

#else
    QString fileName = QFileDialog::getSaveFileName(this, "Save graph as PDF", "./", "*.pdf" );
#endif

    if(!fileName.isEmpty())
    {
        if(ui->tabWidget->currentIndex() == 0)ui->plot->savePdf(fileName);
        else ui->plot_accl->savePdf(fileName);
    }
}

void MainWindow::saveCsv()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
    QAndroidJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );

    QString fileName = mediaPath.toString()+"/samples.csv";

    QMessageBox::information(this, "Saved", "saved to "+fileName, QMessageBox::Ok);

#else
    QString fileName = QFileDialog::getSaveFileName(this, "Save CSV", "./", "*.csv" );
#endif
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
