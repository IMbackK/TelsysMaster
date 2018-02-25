#ifndef LIMITDIALOG_H
#define LIMITDIALOG_H

#include <QDialog>

namespace Ui {
class LimitDialog;
}

class LimitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LimitDialog(QWidget *parent = 0);
    ~LimitDialog();

    int getSampleMemoryLimit();
    int getGraphLimit();
    void setGraphPointLimit(int i);
    void setSampleMemoryLimit(int i);

public slots:

    void accept();
    void reject();

signals:


private:
    Ui::LimitDialog *ui;
};

#endif // LIMITDIALOG_H
