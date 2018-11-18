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

    nextGen();
}

void MainWindow::initMenu() {
    ui->menuView->addAction(ui->control->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());
}

void MainWindow::initConnections() {
    connect(ui->button_nextGen, SIGNAL(released()), SLOT(nextGen()));
    connect(ui->button_changePool, SIGNAL(released()), SLOT(changePool()));
    connect(ui->button_newPool, SIGNAL(released()), SLOT(newPool()));
    connect(ui->button_changeTask, SIGNAL(released()), SLOT(changeTask()));
    connect(ui->button_randomizeTask, SIGNAL(released()), SLOT(randomizeTask()));
    connect(ui->button_play, SIGNAL(clicked(bool)), SLOT(playPause(bool)));
    connect(ui->button_reset, SIGNAL(released()), SLOT(resetGen()));
    connect(ui->button_evaluate, SIGNAL(released()), SLOT(evaluateGen()));
    connect(ui->button_step, SIGNAL(released()), SLOT(step()));
    connect(ui->checkbox_evaluate, SIGNAL(stateChanged(int)), SLOT(immediateEvaluation(int)));
}

void MainWindow::initViews() {
    ui->mainView->setExperiment(experiment);
    ui->netView->setExperiment(experiment);

    ui->progressBar->setValue(int(experiment->getT()));
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
    if (ui->button_play->isChecked()) {
        if (experiment->getT() >= experiment->getTMax()) {
            playPause(false);
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

// Necessary because of an actual bug (https://bugreports.qt.io/browse/QTBUG-68195)
void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    QMainWindow::restoreState(QMainWindow::saveState());
}

void MainWindow::nextGen() {
    experiment->nextGen();
    ui->mainView->following = false;
    ui->tableView->clearSelection();
    updateInstanceTable();
    ui->progressBar->setValue(int(experiment->getT()));
    playPause(false);
    ui->mainView->update();
}

void MainWindow::changePool() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->changePool(&d);
    initViews();
    updateInstanceTable();
    update();
}

void MainWindow::newPool() {
    experiment->newPool();
    initViews();
    updateInstanceTable();
    update();
}

void MainWindow::changeTask() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->changeTask(&d);
    update();
}

void MainWindow::randomizeTask() {
    experiment->randomizeTask();
    update();
}

void MainWindow::playPause(bool play) {
    if (play && experiment->getT() >= experiment->getTMax()) {
        experiment->resetGen();
    }
    if (play) {
        timer->start(1000 / 30);
        ui->button_play->setIcon(QIcon(":/icons/pause.png"));
    }
    else {
        timer->stop();
        ui->button_play->setIcon(QIcon(":/icons/play.png"));
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

void MainWindow::immediateEvaluation(int eval) {
    experiment->immediateEvaluation = eval;
}
