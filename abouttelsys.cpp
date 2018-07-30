#include "abouttelsys.h"
#include "ui_abouttelsys.h"
#include <QPalette>

AboutTelsys::AboutTelsys(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutTelsys)
{
    ui->setupUi(this);
    setFixedSize(size());

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(52,52,60));
    pal.setColor(QPalette::WindowText, QColor(255,255,255));
     pal.setColor(QPalette::Text, QColor(255,255,255));
    setAutoFillBackground(true);
    setPalette(pal);

}

AboutTelsys::~AboutTelsys()
{
    delete ui;
}
