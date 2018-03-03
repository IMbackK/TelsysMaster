#include "ratedialog.h"
#include "ui_ratedialog.h"

RateDialog::RateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RateDialog)
{
    ui->setupUi(this);

#ifdef Q_OS_LINUX
    setFixedSize(size());
#endif

#ifdef Q_OS_ANDROID
    setWindowState(Qt::WindowMaximized);
#endif
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
