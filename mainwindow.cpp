#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    experiment(8) {
    ui->setupUi(this);

    ui->menuView->addAction(ui->settings->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());

    connect(ui->pushButton, SIGNAL(released()), SLOT(update()));
    connect(ui->pushButton_2, SIGNAL(released()), SLOT(printText()));

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->mainView->setScene(scene);
    QPen pen(QColor(255, 255, 255));
    pen.setWidth(1);
    QGraphicsRectItem *rect = scene->addRect(0, 0, 60, 100, pen);
    experiment.gens[0].pop[0].graphic = rect;
    ui->mainView->show();
}

MainWindow::~MainWindow() {
    delete ui;
    delete scene;
}

void MainWindow::update() {
    experiment.updateAll();
    ui->mainView->update();
}

void MainWindow::printText() {
    qDebug() << ui->lineEdit->text();
}
