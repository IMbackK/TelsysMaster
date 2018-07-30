#ifndef ABOUTTELSYS_H
#define ABOUTTELSYS_H

#include <QDialog>

namespace Ui {
class AboutTelsys;
}

class AboutTelsys : public QDialog
{
    Q_OBJECT

public:
    explicit AboutTelsys(QWidget *parent = 0);
    ~AboutTelsys();

private:
    Ui::AboutTelsys *ui;
};

#endif // ABOUTTELSYS_H
