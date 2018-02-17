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

public slots:

    void accept();
    void setGraphPointLimit(int i);
    void setListViewPointLimit(int i);

signals:

    void graphPointLimit(int i);
    void listViewPointLimit(int i);
    void memoryDepth(int i);

private:
    Ui::LimitDialog *ui;
};

#endif // LIMITDIALOG_H
