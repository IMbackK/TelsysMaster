#include "scaleoffsetdiag.h"
#include "ui_scaleoffsetdiag.h"

ScaleOffsetDiag::ScaleOffsetDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScaleOffsetDiag)
{
    ui->setupUi(this);
    connect(ui->checkBox_manual, &QCheckBox::toggled, this, &ScaleOffsetDiag::manualToggeled);
    connect(ui->checkBox_j1, &QCheckBox::stateChanged, this, &ScaleOffsetDiag::calculateValues);
    connect(ui->checkBox_j2, &QCheckBox::stateChanged, this, &ScaleOffsetDiag::calculateValues);

#ifdef Q_OS_LINUX
    setFixedSize(size());
#endif

#ifdef Q_OS_ANDROID
    setWindowState(Qt::WindowMaximized);
#endif
}

ScaleOffsetDiag::~ScaleOffsetDiag()
{
    delete ui;
}

bool ScaleOffsetDiag::isManual()
{
    return ui->checkBox_manual->isChecked();
}

void ScaleOffsetDiag::manualToggeled(bool checked)
{
    if( checked )
    {
        ui->checkBox_j1->setEnabled(false);
        ui->checkBox_j2->setEnabled(false);
        ui->doubleSpinBox->setEnabled(true);
        ui->doubleSpinBox_2->setEnabled(true);
    }
    else
    {
        ui->checkBox_j1->setEnabled(true);
        ui->checkBox_j2->setEnabled(true);
        ui->doubleSpinBox->setEnabled(false);
        ui->doubleSpinBox_2->setEnabled(false);
    }
}

void ScaleOffsetDiag::calculateValues()
{
    if( !ui->checkBox_j2->isChecked() && !ui->checkBox_j1->isChecked() )
    {
        ui->doubleSpinBox->setValue(0.000005);
        ui->doubleSpinBox_2->setValue(-32768);
    }
    else if( ui->checkBox_j2->isChecked() && !ui->checkBox_j1->isChecked() )
    {
        ui->doubleSpinBox->setValue(0.001);
        ui->doubleSpinBox_2->setValue(-32768);
    }
    else if( !ui->checkBox_j2->isChecked() && ui->checkBox_j1->isChecked() )
    {
        ui->doubleSpinBox->setValue(0.005);
        ui->doubleSpinBox_2->setValue(-32768);
    }
    else if( ui->checkBox_j2->isChecked() && ui->checkBox_j1->isChecked() )
    {
        ui->doubleSpinBox->setValue(1);
        ui->doubleSpinBox_2->setValue(-32768);
    }
}

double ScaleOffsetDiag::getOffset()
{
    return ui->doubleSpinBox_2->value();
}


double ScaleOffsetDiag::getScale()
{
    return ui->doubleSpinBox->value();
}

bool ScaleOffsetDiag::rescale()
{
    return ui->checkBox_rescaleSamples->isChecked();
}

void ScaleOffsetDiag::accept()
{
    done(0);
}

void ScaleOffsetDiag::reject()
{
    done(1);
}
