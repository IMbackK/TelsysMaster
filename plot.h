#ifndef PLOT_H
#define PLOT_H

#include<QVector>
#include<QAction>
#include<QWidget>

#include "qcustomplot/qcustomplot.h"
#include "regessioncalculator.h"
#include "statisticsdialog.h"
#include "utilitys.h"

class Plot : public QCustomPlot
{
private:
    std::vector<RegessionCalculator> regressions;

    QMenu graphContextMenu;

#ifndef Q_OS_ANDROID
    unsigned graphPointLimit = 10000;
#else
    unsigned graphPointLimit = 2000;
#endif

    QAction actionStatistics;
    QAction actionAdd_Regression;
    QAction actionDelete_Regression;
    QAction actionExport_Selection;
    QAction actionSetValueString;

    QString lable = "Value";

public:
    Plot(QWidget* parent = nullptr);
    ~Plot();

    void setLable(QString lable);

    void clear();
    void setLimit(unsigned graphPointLimit);
    unsigned getLimit();

    void setMaxValue(double maxVal);

signals:
    void sigSaveCsv();

public slots:
    void addPoints(QVector<double> keys, QVector<double> values);
    void saveCsv(QString fileName);
    void saveCsvDiag();
    void showStatistics();
    void deleteRegression();
    void addRegression();
    void askForValueString();

    void addData(QVector<double> keys, QVector<double> values, bool inOrder = false, bool ignoreLimit = false);
    void addData(double key, double value, bool ignoreLimit = false);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual bool event(QEvent *event);

private:
    void addMainGraph();

};

#endif // PLOT_H

