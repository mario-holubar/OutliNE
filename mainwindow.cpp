#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    experiment(8) {
    ui->setupUi(this);

    ui->menuView->addAction(ui->settings->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    timer->start(1000.0f / 60.0f);

    connect(ui->pushButton_2, SIGNAL(released()), SLOT(printText()));

    ui->mainView->setScene(&(experiment.scene));
    ui->mainView->show();
}

MainWindow::~MainWindow() {
    delete ui;
    delete timer;
}

void MainWindow::update() {
    experiment.updateAll();
    ui->mainView->update();
}

void MainWindow::printText() {
    qDebug() << ui->lineEdit->text();
}
