#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

StatisticsDialog::StatisticsDialog(QCPDataRange dataRange, QSharedPointer< QCPGraphDataContainer > data, QWidget *parent):
    QDialog(parent),
    ui(new Ui::StatisticsDialog)
{
    ui->setupUi(this);

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
    ui->lcdNumber_tsTo->display(end->key);

}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}
