#include "callibrationdialog.h"
#include "ui_callibrationdialog.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QString>
#include <QPushButton>

CallibrationDialog::CallibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallibrationDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton_copyAmp, &QPushButton::clicked, this, &CallibrationDialog::copyAmp);
    connect(ui->pushButton_copyTemp, &QPushButton::clicked, this, &CallibrationDialog::copyTemp);
}

CallibrationDialog::~CallibrationDialog()
{
    delete ui;
}

std::array<double, 10> CallibrationDialog::getSpinBoxValues()
{
    std::array<double, 10> tmp;
    tmp[0] = ui->doubleSpinBox_amp1->value();
    tmp[1] = ui->doubleSpinBox_amp2->value();
    tmp[2] = ui->doubleSpinBox_amp3->value();
    tmp[3] = ui->doubleSpinBox_amp4->value();
    tmp[4] = ui->doubleSpinBox_amp5->value();

    tmp[5] = ui->doubleSpinBox_temp1->value();
    tmp[6] = ui->doubleSpinBox_temp2->value();
    tmp[7] = ui->doubleSpinBox_temp3->value();
    tmp[8] = ui->doubleSpinBox_temp4->value();
    tmp[9] = ui->doubleSpinBox_temp5->value();
    return tmp;
}

void CallibrationDialog::accept()
{
    sigCalValues(getSpinBoxValues());

    sigOffset(ui->doubleSpinBox_offset->value());
    done(0);
}

void CallibrationDialog::copyAmp()
{
    std::array<double, 10> tmp = getSpinBoxValues();
    QString buffer;
    for(unsigned int i = 0; i < 5; i++)
    {
        buffer.append(QString::number(tmp[i]));
        buffer.append(" ,");
    }
    buffer.remove(buffer.size()-2,2);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Selection);
}

void CallibrationDialog::copyTemp()
{
    std::array<double, 10> tmp = getSpinBoxValues();
    QString buffer;
    for(unsigned int i = 5; i < 10; i++)
    {
        buffer.append(QString::number(tmp[i]));
        buffer.append(" ,");
    }
    buffer.remove(buffer.size()-2,2);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QGuiApplication::clipboard()->setText(buffer, QClipboard::Selection);
}
