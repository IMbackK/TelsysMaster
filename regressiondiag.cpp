#include "regressiondiag.h"
#include "ui_regressiondiag.h"

RegressionDiag::RegressionDiag(const RegessionCalculator& reg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegressionDiag)
{
    ui->setupUi(this);

    ui->lcdNumber_slope->display(reg.slope);
    ui->lcdNumber_offset->display(reg.offset);
    ui->lcdNumber_StdError->display(reg.stdError);
    ui->lcdNumbe_count->display((int)reg.xValues.size());

#ifdef Q_OS_ANDROID
    setWindowState(Qt::WindowMaximized);
#endif
}

RegressionDiag::~RegressionDiag()
{
    delete ui;
}

