#include "replotdiag.h"
#include "ui_replotdiag.h"

ReplotDiag::ReplotDiag(unsigned maxVal, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplotDiag)
{
    ui->setupUi(this);
    ui->spinBox_from->setMaximum(maxVal);
    ui->spinBox_to->setMaximum(maxVal);
    setFixedSize(size());
}

void ReplotDiag::accept()
{

    done(0);
}

void ReplotDiag::reject()
{
    done(1);
}

unsigned int ReplotDiag::getFrom()
{
    return ui->spinBox_from->value();
}

unsigned int ReplotDiag::getTo()
{
    return ui->spinBox_to->value();
}

ReplotDiag::~ReplotDiag()
{
    delete ui;
}

