#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "qcustomplot/qcustomplot.h"

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QCPDataRange dataRange, QSharedPointer< QCPGraphDataContainer > data, QWidget *parent = 0);
    ~StatisticsDialog();

private:
    Ui::StatisticsDialog *ui;
};

#endif // STATISTICSDIALOG_H
