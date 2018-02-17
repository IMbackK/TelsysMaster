#ifndef REPLOTDIAG_H
#define REPLOTDIAG_H

#include <QDialog>

namespace Ui {
class ReplotDiag;
}

class ReplotDiag : public QDialog
{
    Q_OBJECT

public:
    explicit ReplotDiag(unsigned maxVal = 1000, QWidget *parent = 0);
    ~ReplotDiag();

    unsigned int getFrom();
    unsigned int getTo();

public slots:
    void accept();
    void reject();

private:
    Ui::ReplotDiag *ui;
};

#endif // REPLOTDIAG_H
