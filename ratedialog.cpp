#include "ratedialog.h"
#include "ui_ratedialog.h"

RateDialog::RateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RateDialog)
{
    ui->setupUi(this);
}

RateDialog::~RateDialog()
{
    delete ui;
}
