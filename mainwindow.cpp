#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>

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

    ui->mainView->setScene(experiment.scene);
    ui->mainView->setRenderHint(QPainter::Antialiasing);
    ui->mainView->show();

    qsrand(QTime::currentTime().msec());
}

MainWindow::~MainWindow() {
    delete ui;
    delete timer;
}

void MainWindow::update() {
    experiment.updateAll();
    ui->mainView->centerOn(ui->mainView->scene()->items().at(1));
    //ui->mainView->fitInView(ui->mainView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    //ui->mainView->centerOn(ui->mainView->scene()->items().at(1)->pos());
    ui->mainView->update();
}

void MainWindow::printText() {
    qDebug() << ui->lineEdit->text();
}
