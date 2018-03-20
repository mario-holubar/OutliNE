#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    experiment(8, 4, 2)
{
    ui->setupUi(this);

    ui->menuView->addAction(ui->settings->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());

    connect(ui->pushButton, SIGNAL(released()), SLOT(update()));
    connect(ui->pushButton_2, SIGNAL(released()), SLOT(printText()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update() {
    QVector<float> input(4);
    input[0] = 1.0f;
    input[1] = 2.0f;
    input[2] = 3.0f;
    input[3] = 4.0f;
    QVector<float> output = experiment.task.update(input);
    qDebug() << output[0] << ", " << output[1];
    qDebug() << experiment.gens[0].pop[7].x;
}

void MainWindow::printText() {
    qDebug() << ui->lineEdit->text();
}
