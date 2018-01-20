#include "connectiondialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton_Connect, &QAbstractButton::clicked, this, [this](){this->openConnDiag();});
    connect(ui->actionQuit, &QAction::triggered, this, [this](){this->close();});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connected()
{
    ui->label_Connected->setText("Connected");
}

void MainWindow::disconnected()
{
    ui->label_Connected->setText("Disconnected");
}

void MainWindow::connectionFailed(QString errorMsg)
{
    QMessageBox::warning(this, "Error", errorMsg, QMessageBox::Ok);
}
