#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <mainview.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    experiment(64) {
    ui->setupUi(this);

    ui->menuView->addAction(ui->settings->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    timer->start(1000.0f / 60.0f);

    connect(ui->pushButton, SIGNAL(released()), SLOT(evaluate()));
    connect(ui->pushButton_2, SIGNAL(released()), SLOT(newGen()));
    connect(ui->playButton, SIGNAL(clicked(bool)), SLOT(playPause(bool)));

    ui->mainView->setScene(experiment.scene);

    ui->progressBar->setMaximum(experiment.tMax);

    instanceTableModel = new InstanceModel(0, experiment.popSize);
    proxyModel = new QSortFilterProxyModel(0);
    proxyModel->setSourceModel(instanceTableModel);
    ui->tableView->setModel(proxyModel);

    play = false;
}

MainWindow::~MainWindow() {
    delete ui;
    delete timer;
}

void MainWindow::setPlaying(bool playing) {
    play = playing;
    if (experiment.t >= experiment.tMax) play = false;
    if (play) ui->playButton->setText("◼");
    else ui->playButton->setText("►");
}

void MainWindow::update() {
    if (play) {
        if (experiment.t >= experiment.tMax) {
            playPause(false);
            return;
        }
        experiment.stepAll(true);
    }
    //ui->mainView->fitInView(ui->mainView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    //ui->mainView->fitInView(ui->mainView->scene()->items().at(0), Qt::KeepAspectRatio);
    QTransform zoomTransform;
    zoomTransform.scale(1.0f, 1.0f);
    ui->mainView->setTransform(zoomTransform, false);
    //ui->mainView->centerOn(ui->mainView->scene()->items().at(1));
    //ui->mainView->centerOn(QPoint(0.0f, 0.0f));
    ui->mainView->update();
    ui->progressBar->setValue(experiment.t);
}

void MainWindow::updateInstanceTable() {
    for (unsigned int i = 0; i < experiment.popSize; i++) {
        instanceTableModel->fitness[i] = experiment.gens.at(experiment.currentGen).pop.at(i).fitness / experiment.tMax;
    }
    //proxyModel->sort(1, Qt::DescendingOrder);
    ui->tableView->repaint();
}

void MainWindow::evaluate() {
    experiment.evaluateGen();
    updateInstanceTable();
}

void MainWindow::newGen() {
    experiment.newGen();
}

void MainWindow::playPause(bool checked) {
    play = checked;
    if (experiment.t >= experiment.tMax) play = false;
    if (play) ui->playButton->setText("◼");
    else ui->playButton->setText("►");
}
