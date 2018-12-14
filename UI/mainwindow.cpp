#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <UI/mainview.h>
#include "paramdialog.h"

#define FPS 30

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      experiment(new Experiment()) {
    ui->setupUi(this);
    QLocale::setDefault(QLocale::c());

    initMenu();
    initConnections();
    initViews();

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(stepUpdate()));

    thread = new QThread;
    experiment->moveToThread(thread);
    connect(experiment, SIGNAL(updateView()), this, SLOT(updateViews()), Qt::BlockingQueuedConnection);
    //connect(experiment, SIGNAL(updateView()), this, SLOT(update()), Qt::DirectConnection);
    thread->start();

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
    connect(ui->button_changePool, SIGNAL(released()), SLOT(queuePoolDialog()));
    connect(ui->button_newPool, SIGNAL(released()), SLOT(newPool()));
    connect(ui->button_changeTask, SIGNAL(released()), SLOT(queueTaskDialog()));
    connect(ui->button_randomizeTask, SIGNAL(released()), SLOT(randomizeTask()));
    connect(ui->button_play, SIGNAL(clicked(bool)), SLOT(playPause(bool)));
    connect(ui->button_reset, SIGNAL(released()), SLOT(resetGen()));
    connect(ui->button_evaluate, SIGNAL(released()), SLOT(evaluateGen()));
    connect(ui->button_step, SIGNAL(released()), SLOT(step()));

    connect(this, SIGNAL(experiment_nextGen()), experiment, SLOT(nextGen()));
    connect(this, SIGNAL(experiment_changePool()), experiment, SLOT(changePool()));
    connect(experiment, SIGNAL(requestPoolDialog()), this, SLOT(makePoolDialog()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(experiment_newPool()), experiment, SLOT(newPool()));
    connect(this, SIGNAL(experiment_changeTask()), experiment, SLOT(changeTask()));
    connect(experiment, SIGNAL(requestTaskDialog()), this, SLOT(makeTaskDialog()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(experiment_randomizeTask()), experiment, SLOT(randomizeTask()));
    connect(this, SIGNAL(experiment_evaluateGen()), experiment, SLOT(evaluateGen()));
    connect(this, SIGNAL(experiment_resetGen()), experiment, SLOT(resetGen()));
    connect(this, SIGNAL(experiment_step()), experiment, SLOT(stepAll()));
}

// Necessary because of an actual bug (https://bugreports.qt.io/browse/QTBUG-68195)
void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    QMainWindow::restoreState(QMainWindow::saveState());
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

void MainWindow::updateViews() {
    ui->mainView->update();
    ui->netView->update();
    ui->progressBar->setMaximum(int(experiment->getTMax()));
    ui->progressBar->setValue(int(experiment->getT()));
    updateInstanceTable();
}

void MainWindow::updateInstanceTable() {
    instanceTableModel->fitness.resize(int(experiment->getPopSize()));
    for (int i = 0; i < int(experiment->getPopSize()); i++) {
        instanceTableModel->fitness[i] = experiment->getIndividual(i)->getFitness();
    }
    proxyModel->invalidate();
    ui->tableView->repaint();
}

void MainWindow::playPause(bool play) {
    ui->button_play->setChecked(play);
    if (play && experiment->getT() >= experiment->getTMax()) {
        resetGen();
    }
    if (play) {
        timer->start(1000 / FPS);
        ui->button_play->setIcon(QIcon(":/icons/pause.png"));
    }
    else {
        timer->stop();
        ui->button_play->setIcon(QIcon(":/icons/play.png"));
    }
}

void MainWindow::stepUpdate() {
    if (ui->button_play->isChecked()) {
        if (experiment->getT() >= experiment->getTMax()) {
            playPause(false);
            return;
        }
        step();
    }
}

void MainWindow::setSelected(const QItemSelection &selection) {
    if (selection.indexes().size() == 0) experiment->setSelected(-1);
    else {
        QItemSelection realSelection = proxyModel->mapSelectionToSource(selection);
        experiment->setSelected(realSelection.indexes().at(0).row());
        ui->mainView->following = true;
    }
    updateViews();
}

void MainWindow::nextGen() {
    emit experiment_nextGen();
    //if (ui->checkbox_evaluate->isChecked()) evaluateGen();
    ui->mainView->following = false;
    ui->tableView->clearSelection();
    playPause(false);
}

void MainWindow::queuePoolDialog() {
    emit experiment_changePool();
    //if (ui->checkbox_evaluate->isChecked()) evaluateGen();
    //emit experiment_resetGen();
    //emit experiment_nextGen();
}

/*void MainWindow::changePool() {
    ParamDialog *d = new ParamDialog(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->changePool(d);
    if (ui->checkbox_evaluate->isChecked()) evaluateGen();
    initViews();
}*/

void MainWindow::makePoolDialog() {
    //ParamDialog *d = new ParamDialog(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->ne->paramDialog(&d);
    experiment->poolChanged = d.exec();
}

void MainWindow::newPool() {
    emit experiment_newPool();
    //if (ui->checkbox_evaluate->isChecked()) evaluateGen();
    initViews();
}

void MainWindow::queueTaskDialog() {
    emit experiment_changeTask();
    //if (ui->checkbox_evaluate->isChecked()) evaluateGen();
    //emit experiment_resetGen();
}

/*void MainWindow::changeTask() {
    ParamDialog *d = new ParamDialog(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->changeTask(d);
    if (ui->checkbox_evaluate->isChecked()) evaluateGen();
}*/

void MainWindow::makeTaskDialog() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->taskparams->paramDialog(&d);
    experiment->taskChanged = d.exec();
}

void MainWindow::randomizeTask() {
    emit experiment_randomizeTask();
    //if (ui->checkbox_evaluate->isChecked()) evaluateGen();
}

void MainWindow::evaluateGen() {
    emit experiment_evaluateGen();
}

void MainWindow::resetGen() {
    emit experiment_resetGen();
}

void MainWindow::step() {
    emit experiment_step();
    if (experiment->getT() >= experiment->getTMax()) playPause(false);
}
