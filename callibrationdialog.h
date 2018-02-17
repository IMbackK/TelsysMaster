#ifndef CALLIBRATIONDIALOG_H
#define CALLIBRATIONDIALOG_H

#include <QDialog>
#include <array>

namespace Ui {
class CallibrationDialog;
}

class CallibrationDialog : public QDialog
{
    Q_OBJECT

private:
    std::array<double, 10> getSpinBoxValues();

public:
    explicit CallibrationDialog(QWidget *parent = 0);
    ~CallibrationDialog();

public slots:
    void accept();

private slots:
    void copyAmp();
    void copyTemp();

signals:
    void sigOffset(double offset);
    void sigCalValues(std::array<double, 10> values);

private:
    Ui::CallibrationDialog *ui;
};

#endif // CALLIBRATIONDIALOG_H

