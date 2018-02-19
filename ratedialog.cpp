#include "ratedialog.h"
#include "ui_ratedialog.h"

RateDialog::RateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RateDialog)
{
    ui->setupUi(this);
    setFixedSize(size());
}

RateDialog::~RateDialog()
{
    delete ui;
}

int RateDialog::getRate()
{
    return ui->spinBox->value();
}

void RateDialog::accept()
{
    done(0);
}

void RateDialog::reject()
{
    done(1);
}
