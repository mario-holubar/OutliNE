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
    connect(ui->playButton, SIGNAL(clicked(bool)), SLOT(playPause(bool)));

    ui->mainView->setScene(experiment.scene);
    ui->mainView->setBackgroundBrush(QBrush(QColor(8, 8, 8)));
    ui->mainView->show();

    play = false;
}

MainWindow::~MainWindow() {
    delete ui;
    delete timer;
}

void MainWindow::update() {
    if (play) experiment.stepAll(true);
    //ui->mainView->fitInView(ui->mainView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    //ui->mainView->fitInView(ui->mainView->scene()->items().at(0), Qt::KeepAspectRatio);
    QTransform zoomTransform;
    zoomTransform.scale(0.5f, 0.5f);
    ui->mainView->setTransform(zoomTransform, false);
    ui->mainView->centerOn(ui->mainView->scene()->items().at(1));
    ui->mainView->update();
}

void MainWindow::printText() {
    qDebug() << ui->lineEdit->text();
}

void MainWindow::playPause(bool checked) {
    play = checked;
    if (checked) ui->playButton->setText("◼");
    else ui->playButton->setText("►");
}
