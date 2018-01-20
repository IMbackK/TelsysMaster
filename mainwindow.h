#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void openConnDiag();

public slots:
    void connected();
    void disconnected();
    void connectionFailed(QString errorMsg);


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
