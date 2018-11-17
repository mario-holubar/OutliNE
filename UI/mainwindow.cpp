#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <UI/mainview.h>
#include "paramdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      experiment(new Experiment()) {
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
    ui->menuView->addAction(ui->control->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());
}

void MainWindow::initConnections() {
    connect(ui->button_newGen, SIGNAL(released()), SLOT(newGen()));
    connect(ui->button_newPool, SIGNAL(released()), SLOT(newPool()));
    connect(ui->button_newTask, SIGNAL(released()), SLOT(newTask()));
    connect(ui->button_randomizeTask, SIGNAL(released()), SLOT(randomizeTask()));
    connect(ui->button_play, SIGNAL(released()), SLOT(playPause()));
    connect(ui->button_reset, SIGNAL(released()), SLOT(resetGen()));
    connect(ui->button_evaluate, SIGNAL(released()), SLOT(evaluateGen()));
    connect(ui->button_step, SIGNAL(released()), SLOT(step()));
}

void MainWindow::initViews() {
    ui->mainView->setExperiment(experiment);
    ui->netView->setExperiment(experiment);

    ui->progressBar->setMaximum(int(experiment->getTMax()));

    instanceTableModel = new InstanceModel(this, int(experiment->getPopSize()));
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
        if (experiment->getT() >= experiment->getTMax()) {
            playPause();
            return;
        }
        experiment->stepAll();
    }
    ui->mainView->update();
    ui->netView->update();
    ui->progressBar->setValue(int(experiment->getT()));
    updateInstanceTable();
}

void MainWindow::updateInstanceTable() {
    for (int i = 0; i < int(experiment->getPopSize()); i++) {
        instanceTableModel->fitness[i] = experiment->getIndividual(i)->getFitness();
    }
    proxyModel->invalidate();
    ui->tableView->repaint();
}

void MainWindow::newGen() {
    experiment->newGen();
    ui->mainView->following = false;
    ui->tableView->clearSelection();
    updateInstanceTable();
    ui->progressBar->setValue(int(experiment->getT()));
    if (play) playPause();
    ui->mainView->update();
}

void MainWindow::newPool() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->newPool(&d);
    initViews();
    update();
}

void MainWindow::newTask() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->newTask(&d);
    update();
}

void MainWindow::randomizeTask() {
    experiment->randomizeTask();
    update();
}

void MainWindow::playPause() {
    play = !play;
    if (play && experiment->getT() >= experiment->getTMax()) {
        experiment->resetGen();
    }
    if (play) {
        timer->start(1000 / 30);
        ui->button_play->setText("◼"); // Resets widget sizes unless one of them has been moved
        //ui->button_play->setIcon(QIcon(":/icons/pause-16.ico"));
    }
    else {
        timer->stop();
        ui->button_play->setText("►");
        //ui->button_play->setIcon(QIcon(":/icons/play-16.ico"));
    }
}

void MainWindow::evaluateGen() {
    experiment->evaluateGen();
    updateInstanceTable();
    ui->mainView->update();
    ui->progressBar->setValue(int(experiment->getT()));
}

void MainWindow::resetGen() {
    experiment->resetGen();
    updateInstanceTable();
    ui->mainView->update();
    ui->progressBar->setValue(int(experiment->getT()));
}

void MainWindow::step() {
    experiment->stepAll();
    updateInstanceTable();
    ui->mainView->update();
    ui->netView->update();
    ui->progressBar->setValue(int(experiment->getT()));
}

void MainWindow::setSelected(const QItemSelection &selection) {
    if (selection.indexes().size() == 0) experiment->setSelected(-1);
    else {
        QItemSelection realSelection = proxyModel->mapSelectionToSource(selection);
        experiment->setSelected(realSelection.indexes().at(0).row());
        ui->mainView->following = true;
    }
    ui->mainView->update();
    ui->netView->update();
}