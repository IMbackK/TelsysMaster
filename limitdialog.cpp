#include "limitdialog.h"
#include "ui_limitdialog.h"

LimitDialog::LimitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LimitDialog)
{
    ui->setupUi(this);
}

LimitDialog::~LimitDialog()
{
    delete ui;
}

void LimitDialog::accept()
{
    graphPointLimit(ui->spinBox_graph->value());
    listViewPointLimit(ui->spinBox_list->value());
    memoryDepth(ui->spinBox_sampleMemory->value());
    done(0);
}

void LimitDialog::setGraphPointLimit(int i)
{
    ui->spinBox_graph->setValue(i);
}


void LimitDialog::setListViewPointLimit(int i)
{
    ui->spinBox_list->setValue(i);
}
