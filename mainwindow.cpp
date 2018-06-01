#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <mainview.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      experiment(8) {
    ui->setupUi(this);
    initMenu();
    initConnections();

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    timer->start(1000.0f / 60.0f);

    ui->progressBar->setMaximum(experiment.tMax);

    instanceTableModel = new InstanceModel(this, experiment.popSize);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(instanceTableModel);
    proxyModel->setDynamicSortFilter(true);
    ui->tableView->setModel(proxyModel);
    proxyModel->sort(1, Qt::DescendingOrder);
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(setSelected(const QItemSelection &)));

    play = false;

    newGen();
}

void MainWindow::initMenu() {
    ui->menuView->addAction(ui->settings->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());
}

void MainWindow::initConnections() {
    connect(ui->pushButton_2, SIGNAL(released()), SLOT(newGen()));
    connect(ui->playButton, SIGNAL(released()), SLOT(playPause()));
    connect(ui->resetButton, SIGNAL(released()), SLOT(resetGen()));
    connect(ui->evaluateButton, SIGNAL(released()), SLOT(evaluateGen()));
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
            playPause();
            return;
        }
        experiment.stepAll();
    }
    ui->mainView->experiment = &experiment;
    ui->mainView->update();
    ui->progressBar->setValue(experiment.t);
    updateInstanceTable();
}

void MainWindow::updateInstanceTable() {
    for (unsigned int i = 0; i < experiment.popSize; i++) {
        instanceTableModel->fitness[i] = experiment.gens.at(experiment.currentGen).pop.at(i).fitness / experiment.t;
    }
    proxyModel->invalidate();
    ui->tableView->repaint();
}

void MainWindow::newGen() {
    experiment.newGen();
    ui->tableView->clearSelection();
    updateInstanceTable();
}

void MainWindow::playPause() {
    play = !play;
    if (experiment.t >= experiment.tMax) play = false;
    if (play) ui->playButton->setText("◼");
    else ui->playButton->setText("►");
}

void MainWindow::evaluateGen() {
    experiment.evaluateGen();
    updateInstanceTable();
}

void MainWindow::resetGen() {
    experiment.resetGen();
    updateInstanceTable();
}

void MainWindow::setSelected(const QItemSelection &selection) {
    if (selection.indexes().size() == 0) experiment.setSelected(-1);
    else experiment.setSelected(selection.indexes().at(0).row());
}
