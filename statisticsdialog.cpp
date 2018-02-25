#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

StatisticsDialog::StatisticsDialog(QCPDataRange dataRange, QSharedPointer< QCPGraphDataContainer > data, QWidget *parent):
    QDialog(parent),
    ui(new Ui::StatisticsDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton_1, &QPushButton::clicked, this, &StatisticsDialog::copyCount);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &StatisticsDialog::copyTimestamps);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &StatisticsDialog::copyDeltaT);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &StatisticsDialog::copyMean);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &StatisticsDialog::copyMinMax);

    ui->lcdNumber_Count->display(dataRange.size());

    QCPGraphDataContainer::const_iterator begin = data->at(dataRange.begin());
    QCPGraphDataContainer::const_iterator end = data->at(dataRange.end());

    double mean = 0;
    double max = begin->value;
    double min = begin->value;
    for (QCPGraphDataContainer::const_iterator item=begin; item != end; item++)
    {
        mean += item->value;
        if(item->value < min) min = item->value;
        if(item->value > max) max = item->value;
    }
    mean = mean / dataRange.size();
    ui->lcdNumber_Mean->display(mean);

    ui->lcdNumber_Max->display(max);
    ui->lcdNumber_Min->display(min);

    ui->lcdNumber_tsFrom->display(begin->key);
    ui->lcdNumber_tsTo->display((end-1)->key);

    ui->lcdNumber_deltaT->display((end-1)->key-begin->key);

}

void StatisticsDialog::copyCount()
{
    QString buffer(QString::number(ui->lcdNumber_Count->value(), 'g', 10));
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Selection);
}

void StatisticsDialog::copyTimestamps()
{
    QString buffer(QString::number(ui->lcdNumber_tsFrom->value(), 'g', 10));
    buffer.append(",");
    buffer.append(QString::number(ui->lcdNumber_tsTo->value(), 'g', 10));
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Selection);
}

void StatisticsDialog::copyDeltaT()
{
    QString buffer(QString::number(ui->lcdNumber_deltaT->value(), 'g', 10));
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Selection);
}

void StatisticsDialog::copyMean()
{
    QString buffer(QString::number(ui->lcdNumber_Mean->value(), 'g', 10));
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Selection);
}

void StatisticsDialog::copyMinMax()
{
    QString buffer(QString::number(ui->lcdNumber_Min->value(), 'g', 10));
    buffer.append(",");
    buffer.append(QString::number(ui->lcdNumber_Max->value(), 'g', 10));
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Selection);
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

