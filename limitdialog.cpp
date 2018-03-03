#include "limitdialog.h"
#include "ui_limitdialog.h"

LimitDialog::LimitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LimitDialog)
{
    ui->setupUi(this);


#ifdef Q_OS_LINUX
    setFixedSize(size());
#endif

#ifdef Q_OS_ANDROID
    setWindowState(Qt::WindowMaximized);
#endif
}

LimitDialog::~LimitDialog()
{
    delete ui;
}

void LimitDialog::accept()
{
    done(0);
}

void LimitDialog::reject()
{
    done(1);
}

void LimitDialog::setGraphPointLimit(int i)
{
    ui->spinBox_graph->setValue(i);
}

void LimitDialog::setSampleMemoryLimit(int i)
{
    ui->spinBox_sampleMemory->setValue(i);
}

int LimitDialog::getSampleMemoryLimit()
{
    return ui->spinBox_sampleMemory->value();
}

int LimitDialog::getGraphLimit()
{
    return ui->spinBox_graph->value();
}
