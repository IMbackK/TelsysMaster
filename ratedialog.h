#ifndef RATEDIALOG_H
#define RATEDIALOG_H

#include <QDialog>

namespace Ui {
class RateDialog;
}

class RateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RateDialog(QWidget *parent = 0);
    ~RateDialog();

    int getRate();

public slots:
    void accept();
    void reject();

private:
    Ui::RateDialog *ui;
};

#endif // RATEDIALOG_H
