#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <mainview.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      experiment(128) {
    ui->setupUi(this);

    initMenu();
    initConnections();
    initViews();

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(update()));
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

void MainWindow::initViews() {
    ui->mainView->setExperiment(&experiment);

    ui->progressBar->setMaximum(int(experiment.getTMax()));

    instanceTableModel = new InstanceModel(this, int(experiment.getPopSize()));
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(instanceTableModel);
    proxyModel->setDynamicSortFilter(true);
    ui->tableView->setModel(proxyModel);
    proxyModel->sort(1, Qt::DescendingOrder);
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(setSelected(const QItemSelection &)));
}

MainWindow::~MainWindow() {
    delete ui;
    delete timer;
}

void MainWindow::update() {
    if (play) {
        if (experiment.getT() >= experiment.getTMax()) {
            playPause();
            return;
        }
        experiment.stepAll();
    }
    ui->mainView->update();
    ui->progressBar->setValue(int(experiment.getT()));
    updateInstanceTable();
}

void MainWindow::updateInstanceTable() {
    for (int i = 0; i < int(experiment.getPopSize()); i++) {
        instanceTableModel->fitness[i] = experiment.getIndividual(i)->getFitness();
    }
    proxyModel->invalidate();
    ui->tableView->repaint();
}

void MainWindow::newGen() {
    experiment.newGen();
    ui->tableView->clearSelection();
    updateInstanceTable();
    ui->progressBar->setValue(int(experiment.getT()));
    if (play) playPause();
    ui->mainView->update();
}

void MainWindow::playPause() {
    play = !play;
    if (experiment.getT() >= experiment.getTMax()) play = false;
    if (play) {
        timer->start(1000 / 60);
        ui->playButton->setText("◼");
    }
    else {
        timer->stop();
        ui->playButton->setText("►");
    }
}

void MainWindow::evaluateGen() {
    experiment.evaluateGen();
    updateInstanceTable();
    ui->mainView->update();
    ui->progressBar->setValue(int(experiment.getT()));
}

void MainWindow::resetGen() {
    experiment.resetGen();
    updateInstanceTable();
    ui->mainView->update();
    ui->progressBar->setValue(int(experiment.getT()));
}

void MainWindow::setSelected(const QItemSelection &selection) {
    if (selection.indexes().size() == 0) experiment.setSelected(-1);
    else {
        QItemSelection realSelection = proxyModel->mapSelectionToSource(selection);
        experiment.setSelected(realSelection.indexes().at(0).row());
    }
    ui->mainView->update();
}
