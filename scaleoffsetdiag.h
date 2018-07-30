#ifndef SCALEOFFSETDIAG_H
#define SCALEOFFSETDIAG_H

#include <QDialog>

namespace Ui {
class ScaleOffsetDiag;
}

class ScaleOffsetDiag : public QDialog
{
    Q_OBJECT

    bool _manual = false;

public:
    explicit ScaleOffsetDiag(QWidget *parent = 0);
    ~ScaleOffsetDiag();

    double getScale();
    double getOffset();
    bool rescale();
    bool isManual();

public slots:
    void accept();
    void reject();

private slots:
    void manualToggeled(bool checked);
    void calculateValues();

private:
    Ui::ScaleOffsetDiag *ui;
};

#endif // SCALEOFFSETDIAG_H
