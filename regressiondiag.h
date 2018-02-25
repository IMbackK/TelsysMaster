#ifndef REGRESSIONDIAG_H
#define REGRESSIONDIAG_H

#include <QDialog>

#include "regessioncalculator.h"

namespace Ui {
class RegressionDiag;
}

class RegressionDiag : public QDialog
{
    Q_OBJECT

public:
    explicit RegressionDiag(const RegessionCalculator& reg, QWidget *parent = 0);
    ~RegressionDiag();

private:
    Ui::RegressionDiag *ui;
};

#endif // REGRESSIONDIAG_H
