#include "plot.h"

#include <QKeySequence>
#include <QFont>

#include "statisticsdialog.h"
#include "regressiondiag.h"

Plot::Plot(QWidget* parent):
    QCustomPlot(parent),
    actionStatistics("Show statistics", nullptr),
    actionAdd_Regression("Add regression", nullptr),
    actionDelete_Regression("Delete regression", nullptr),
    actionExport_Selection("Export selection", nullptr),
    actionSetValueString("Set Y Axis Label", nullptr)
{
    xAxis->setLabel("Time [s]");
    yAxis->setLabel(lable);
    xAxis->setRange(0, 10);
    yAxis->setRange(0, 65535);
    addMainGraph();
    setInteraction(QCP::iSelectPlottables, true);
    setInteraction(QCP::iRangeDrag, true);
    setInteraction(QCP::iRangeZoom);
    axisRect()->setRangeDrag(Qt::Horizontal);
    axisRect()->setRangeZoom(Qt::Horizontal);
    setSelectionRectMode(QCP::srmNone);

    //setup actions
    actionStatistics.setShortcut(Qt::Key_S);
    actionStatistics.setShortcutVisibleInContextMenu(true);
    addAction(&actionStatistics);

    actionAdd_Regression.setShortcut(Qt::Key_R);
    actionAdd_Regression.setShortcutVisibleInContextMenu(true);
    addAction(&actionAdd_Regression);

    actionDelete_Regression.setShortcut(Qt::Key_Delete);
    actionDelete_Regression.setShortcutVisibleInContextMenu(true);
    addAction(&actionDelete_Regression);

    actionExport_Selection.setShortcut(Qt::CTRL + Qt::Key_E);
    actionExport_Selection.setShortcutVisibleInContextMenu(true);
    addAction(&actionExport_Selection);

    //graph context menu

    connect(&actionStatistics, &QAction::triggered, this, &Plot::showStatistics);
    connect(&actionAdd_Regression, &QAction::triggered, this, &Plot::addRegression);
    connect(&actionDelete_Regression, &QAction::triggered, this, &Plot::deleteRegression);
    connect(&actionExport_Selection, &QAction::triggered, this, &Plot::saveCsvDiag);
    connect(&actionSetValueString, &QAction::triggered, this, &Plot::askForValueString);

#ifdef Q_OS_ANDROID

    graphContextMenu.addAction(&actionStatistics);
    graphContextMenu.addSeparator();
    graphContextMenu.addAction(&actionAdd_Regression);
    graphContextMenu.addSeparator();
    graphContextMenu.addAction(&actionDelete_Regression);
    graphContextMenu.addSeparator();
    graphContextMenu.addAction(&actionSetValueString);

    grabGesture(Qt::TapAndHoldGesture);
    graphContextMenu.setWindowState(Qt::WindowMaximized);
    QFont font = graphContextMenu.font();
    font.setPointSize(24);
    graphContextMenu.setFont(font);
#else
    graphContextMenu.addAction(&actionStatistics);
    graphContextMenu.addAction(&actionAdd_Regression);
    graphContextMenu.addAction(&actionDelete_Regression);
    graphContextMenu.addAction(&actionExport_Selection);
    graphContextMenu.addAction(&actionSetValueString);
#endif
}

Plot::~Plot()
{

}

bool Plot::event(QEvent *event)
{
    if(event->type()==QEvent::Gesture) graphContextMenu.show();
    return QCustomPlot::event(event);
}

void Plot::setLable(QString lable)
{
    this->lable = lable;
    yAxis->setLabel(lable);
}

void Plot::addMainGraph()
{
    addGraph();
    graph(graphCount()-1)->setSelectable(QCP::stDataRange);
    QPen selectionPen = graph(graphCount()-1)->pen();
    selectionPen.setColor(QColor(255,0,0));
    graph(graphCount()-1)->selectionDecorator()->setPen(selectionPen);
}

void Plot::clear()
{
    clearGraphs();
    addMainGraph();
    xAxis->setRange(0, 10);
    replot();
}

void Plot::askForValueString()
{
    bool ok = false;
    QString label = QInputDialog::getText(this, "Y Axis Label", "New Label:", QLineEdit::Normal, yAxis->label(), &ok);
    if(ok)
    {
        yAxis->setLabel(label);
        replot();
    }
}

void Plot::setMaxValue(double maxVal)
{
    yAxis->setRange(0, maxVal);
}

void Plot::saveCsvDiag()
{
    if(graphCount() > 0 && !graph(0)->selection().dataRanges().at(0).isEmpty())
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Save selection as CSV", "./", "*.csv" );
        saveCsv(fileName);
    }
    else QMessageBox::warning(this, "Warning", "No selection has been made", QMessageBox::Ok);
}

void Plot::saveCsv(QString fileName)
{
    if(!fileName.isEmpty())
    {
        QCPDataRange range = graph(0)->selection().dataRanges().at(0);
        QCPGraphDataContainer::const_iterator begin = graph(0)->data()->at(range.begin());
        QCPGraphDataContainer::const_iterator end = graph(0)->data()->at(range.end());

        std::vector<double> keys;
        keys.resize(end-begin);

        std::vector<double> values;
        values.resize(end-begin);

        for (QCPGraphDataContainer::const_iterator item=begin; item != end; ++item)
        {
            keys[item-begin]=item->key*1000000;
            values[item-begin]=item->value;
        }
        saveToCsv(fileName, keys, values, "Time[us]", lable);
    }
}


void Plot::addRegression()
{
    if(graphCount() > 0 &&  !graph(0)->selection().dataRanges().at(0).isEmpty())
    {
        QCPDataRange range = graph(0)->selection().dataRanges().at(0);
        QCPGraphDataContainer::const_iterator begin = graph(0)->data()->at(range.begin());
        QCPGraphDataContainer::const_iterator end = graph(0)->data()->at(range.end());

        std::vector<double> values;
        std::vector<double> keys;

        values.resize(end-begin);
        keys.resize(end-begin);

        for (QCPGraphDataContainer::const_iterator item=begin; item != end; ++item)
        {
            values[item-begin] = item->value;
            keys[item-begin] = item->key;
        }

        regressions.push_back(RegessionCalculator(keys, values));

        QPen regressionPen =graph(0)->pen();
        regressionPen.setColor(QColor(0,255,0));

        QPen selectionPen = graph(0)->pen();
        selectionPen.setColor(QColor(255,0,0));

        QCPGraphDataContainer::const_iterator center = begin + (end - begin)/2;

        addGraph();
        graph(graphCount()-1)->setPen(regressionPen);
        graph(graphCount()-1)->addData(100+center->key, regressions.back().slope*(100+center->key) + regressions.back().offset);
        graph(graphCount()-1)->addData(-100+center->key, regressions.back().slope*(-100+center->key) + regressions.back().offset);
        graph(graphCount()-1)->selectionDecorator()->setPen(selectionPen);
        replot();

    }
    else QMessageBox::warning(this, "Warning", "No selection has been made", QMessageBox::Ok);
}

void Plot::deleteRegression()
{
    if(graphCount() > 0 && selectedGraphs().size() > 1 && selectedGraphs().at(1) != graph(0))
    {
        int i = 0;
        while(selectedGraphs().at(1) != graph(i) ) i++;
        regressions.erase(regressions.begin()+i-1);
        removeGraph(selectedGraphs().at(1));
        replot();
    }
}


void Plot::showStatistics()
{
    if(graphCount() > 0 && !graph(0)->selection().dataRanges().at(0).isEmpty())
    {
        StatisticsDialog statDiag(graph(0)->selection().dataRanges().at(0), graph(0)->data(), this);
        statDiag.show();
        statDiag.exec();
    }
    else if(graphCount() > 0 && selectedGraphs().size() > 1 && selectedGraphs().at(1) != graph(0))
    {
        unsigned i = 0;
        while(selectedGraphs().at(1) != graph(i)) i++;
        RegressionDiag regDiag(regressions.at(i-1), this);
        regDiag.show();
        regDiag.exec();
    }
    else QMessageBox::warning(this, "Warning", "No selection has been made", QMessageBox::Ok);
}

void Plot::addData(QVector<double> keys, QVector<double> values, bool inOrder, bool ignoreLimit)
{
    if(graphCount() > 0)
    {
        graph(0)->addData(keys, values, inOrder);
        if(!ignoreLimit)while(graph(0)->data()->size() > graphPointLimit) graph(0)->data()->remove(graph(0)->data()->begin()->key);
        xAxis->setRange( graph(0)->data()->begin()->key, keys.back() );
    }
}

void Plot::addData(double key, double value, bool ignoreLimit)
{
    if(graphCount() > 0)
    {
        graph(0)->addData(key, value);
        if(!ignoreLimit)while(graph(0)->data()->size() > graphPointLimit) graph(0)->data()->remove(graph(0)->data()->begin()->key);
        xAxis->setRange( graph(0)->data()->begin()->key, key );
    }
}

void Plot::setLimit(unsigned graphPointLimit)
{
    this->graphPointLimit = graphPointLimit;
    if(graphCount() > 0) while(graph(0)->data()->size() > graphPointLimit) graph(0)->data()->remove(graph(0)->data()->begin()->key);
}

unsigned Plot::getLimit()
{
    return graphPointLimit;
}

void Plot::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"press";
    if(event->button() == Qt::MiddleButton || (event->button() == Qt::LeftButton && event->modifiers() == Qt::Modifier::SHIFT) )
    {
        setCursor(Qt::ClosedHandCursor);
        setSelectionRectMode(QCP::srmNone);
    }
    else if(event->button() == Qt::LeftButton)
    {
        setSelectionRectMode(QCP::srmSelect);
    }
    else if(event->button() == Qt::RightButton)
    {
        graphContextMenu.popup(event->globalPos());
        setSelectionRectMode(QCP::srmNone);
    }
    QCustomPlot::mousePressEvent(event);
}

void Plot::mouseReleaseEvent(QMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    QCustomPlot::mouseReleaseEvent(event);
}
