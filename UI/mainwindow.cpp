#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <UI/mainview.h>
#include "paramdialog.h"
#include <fstream>
#include "NE/racingtask.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      experiment(new Experiment()) {
    ui->setupUi(this);
    QLocale::setDefault(QLocale::c());

    thread = new QThread;
    experiment->moveToThread(thread);
    connect(experiment, SIGNAL(updateView()), this, SLOT(updateViews()), Qt::BlockingQueuedConnection);
    connect(experiment, SIGNAL(updatePerformance(int, unsigned, float, float)), ui->performanceView, SLOT(updatePerformance(int, unsigned, float, float)), Qt::BlockingQueuedConnection);
    thread->start();

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(step()));

    initMenu();
    initConnections();
    initViews();

    newPool();

    //NE::n_genomes = 16;
    //while (int(experiment->ne->n_genomes) > experiment->individuals.size()) experiment->individuals.append(new RacingIndividual(experiment->task));
    //NE::neuronsPerGenome = 32;
    //newPool();

    /*std::ofstream file;
    file.open ("SANE.csv", std::ofstream::trunc);
    file.close();
    file.open ("ESP.csv", std::ofstream::trunc);
    file.close();
    file.open ("CoSyNE.csv", std::ofstream::trunc);
    file.close();
    connect(this, SIGNAL(collectData()), experiment, SLOT(collectData()));
    connect(experiment, SIGNAL(collectPerformanceData()), ui->performanceView, SLOT(collectData()), Qt::BlockingQueuedConnection);
    int runs = 25;
    int gens = 50;
    for (int i = 0; i < runs; i++) {
        //randomizeTask();

        experiment_changeNE(0);
        newPool();
        for (int g = 0; g < gens; g++) {
            randomizeTask();
            nextGen();
        }

        experiment_changeNE(1);
        newPool();
        for (int g = 0; g < gens; g++) {
            randomizeTask();
            nextGen();
        }

        experiment_changeNE(2);
        newPool();
        for (int g = 0; g < gens; g++) {
            randomizeTask();
            nextGen();
        }

        randomizeTask();
        experiment_changeNE(0);
        nextGen();
        experiment_changeNE(1);
        nextGen();
        experiment_changeNE(2);
        nextGen();

        collectData();
    }*/
}

void MainWindow::initMenu() {
    ui->menuView->addAction(ui->control->toggleViewAction());
    ui->menuView->addAction(ui->performance->toggleViewAction());
    ui->menuView->addAction(ui->instances->toggleViewAction());
    ui->menuView->addAction(ui->net->toggleViewAction());
}

void MainWindow::initConnections() {
    for (unsigned i = 0; i < algs.size(); i++) {
        ui->combobox_alg->addItem(algs[i]->name);
    }
    ui->combobox_alg->setCurrentIndex(int(experiment->alg));
    connect(ui->combobox_alg, SIGNAL(currentIndexChanged(int)), experiment, SLOT(changeNE(int)));
    connect(experiment, SIGNAL(genChanged(QString)), ui->label_gen, SLOT(setText(QString)));

    connect(ui->button_nextGen, SIGNAL(released()), SLOT(nextGen()));
    for (int i = 0; i < 10; i++) connect(ui->button_10gens, SIGNAL(released()), SLOT(nextGen()));
    connect(ui->button_changePool, SIGNAL(released()), SLOT(queuePoolDialog()));
    connect(ui->button_newPool, SIGNAL(released()), SLOT(newPool()));
    connect(ui->button_changeTask, SIGNAL(released()), SLOT(queueTaskDialog()));
    connect(ui->button_randomizeTask, SIGNAL(released()), SLOT(randomizeTask()));
    connect(ui->button_play, SIGNAL(clicked(bool)), SLOT(playPause(bool)));
    connect(ui->button_reset, SIGNAL(released()), SLOT(resetGen()));
    connect(ui->button_evaluate, SIGNAL(released()), SLOT(evaluateGen()));
    connect(ui->button_step, SIGNAL(released()), SLOT(step()));
    connect(ui->spinbox_fps, SIGNAL(valueChanged(int)), SLOT(playPause()));

    if (ui->checkbox_followSelected->isChecked()) ui->mainView->centerOnSelected();
    connect(ui->checkbox_followSelected, SIGNAL(stateChanged(int)), ui->mainView, SLOT(centerOnSelected()));
    connect(ui->checkbox_followSelected, SIGNAL(stateChanged(int)), SLOT(updateViews()));

    connect(ui->checkbox_showTop, SIGNAL(stateChanged(int)), ui->performanceView, SLOT(changeShowTop(int)));
    connect(ui->checkbox_showAvg, SIGNAL(stateChanged(int)), ui->performanceView, SLOT(changeShowAvg(int)));

    connect(this, SIGNAL(experiment_changeNE(int)), experiment, SLOT(changeNE(int)));
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
    // Main View
    ui->mainView->setExperiment(experiment);
    ui->netView->setExperiment(experiment);

    connect(experiment, SIGNAL(setViewRect(QRectF)), ui->mainView, SLOT(setViewRect(QRectF)));

    ui->progressBar->setValue(int(experiment->getT()));
    ui->progressBar->setMaximum(int(experiment->getTMax()));

    // Instance View
    instanceTableModel = new InstanceModel(this, int(experiment->getPopSize()));
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(instanceTableModel);
    //proxyModel->setDynamicSortFilter(true);
    ui->tableView->setModel(proxyModel);
    ui->tableView->sortByColumn(1, Qt::DescendingOrder);
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(setSelected(const QItemSelection &)));
    ui->tableView->setColumnWidth(0, 75);
    ui->tableView->setColumnWidth(1, 0);

    // Performance View
    for (unsigned i = 0; i < algs.size(); i++) {
        ui->performanceView->performanceMax[int(i)]->setName(ui->combobox_alg->itemText(int(i)));
    }

    resizeDocks({ui->performance}, {256}, Qt::Horizontal);
    resizeDocks({ui->performance}, {128}, Qt::Vertical);

    ui->label_inputDimMismatch->setVisible(false);
}

MainWindow::~MainWindow() { //not sure if necessary
    /*thread->quit();
    thread->wait();
    timer->stop();
    delete ui;
    delete experiment;
    delete timer;
    delete instanceTableModel;
    delete proxyModel;
    delete thread;*/
}

void MainWindow::updateViews() {
    ui->mainView->update();
    ui->netView->update();
    ui->progressBar->setMaximum(int(experiment->getTMax()));
    ui->progressBar->setValue(int(experiment->getT()));
    updateInstanceTable();
    if (ui->checkbox_trackFirst->isChecked()) {
        experiment->setSelected(proxyModel->mapToSource(proxyModel->index(0, 1)).row());
    }
    ui->mainView->following = ui->checkbox_followSelected->isChecked();
}

void MainWindow::updateInstanceTable() {
    instanceTableModel->fitness.resize(int(experiment->getPopSize()));
    for (int i = 0; i < int(experiment->getPopSize()); i++) {
        instanceTableModel->fitness[i] = experiment->getIndividual(i)->getFitness();
    }
    proxyModel->invalidate();
}

void MainWindow::step() {
    emit experiment_step();
    if (experiment->getT() >= experiment->getTMax()) playPause(false);
}

void MainWindow::playPause() {
    bool play = ui->button_play->isChecked();
    if (play && experiment->getT() >= experiment->getTMax()) {
        resetGen();
    }
    if (play) {
        timer->start(1000 / ui->spinbox_fps->value());
        ui->button_play->setIcon(QIcon(":/icons/pause.png"));
    }
    else {
        timer->stop();
        ui->button_play->setIcon(QIcon(":/icons/play.png"));
    }
}

void MainWindow::playPause(bool play) {
    ui->button_play->setChecked(play);
    playPause();
}

void MainWindow::setSelected(const QItemSelection &selection) {
    if (selection.indexes().size() == 0) experiment->setSelected(-1);
    else {
        QItemSelection realSelection = proxyModel->mapSelectionToSource(selection);
        experiment->setSelected(realSelection.indexes().at(0).row());
        //ui->mainView->following = true;
    }
    if (ui->checkbox_followSelected->isChecked()) ui->mainView->centerOnSelected();
    ui->checkbox_trackFirst->setChecked(false);
    updateViews();
}

void MainWindow::nextGen() {
    //ui->mainView->following = false;
    emit experiment_nextGen();
    ui->tableView->clearSelection();
    playPause(false);
}

void MainWindow::queuePoolDialog() {
    emit experiment_changePool();
}

void MainWindow::makePoolDialog() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->ne->paramDialog(&d);
    experiment->poolChanged = d.exec();
}

void MainWindow::newPool() {
    emit experiment_newPool();

    int i = int(experiment->alg);
    ui->performanceView->performanceMax[i]->clear();
    ui->performanceView->performanceMax[i]->append(0, 0.0);
    ui->performanceView->performanceAvg[i]->clear();
    ui->performanceView->performanceAvg[i]->append(0, 0.0);
    ui->performanceView->maxGen[i] = 0;
    unsigned max = 1;
    for (int i = 0; i < int(algs.size()); i++) {
        if (ui->performanceView->maxGen[i] > max) max = ui->performanceView->maxGen[i];
    }
    ui->performanceView->xAxis->setMax(max);
    ui->label_inputDimMismatch->setVisible(false);
}

void MainWindow::queueTaskDialog() {
    emit experiment_changeTask();
}

void MainWindow::makeTaskDialog() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->taskparams->paramDialog(&d);
    experiment->taskChanged = d.exec();
    ui->label_inputDimMismatch->setVisible(experiment->inputDimMismatch());
}

void MainWindow::randomizeTask() {
    emit experiment_randomizeTask();
}

void MainWindow::evaluateGen() {
    emit experiment_evaluateGen();
}

void MainWindow::resetGen() {
    emit experiment_resetGen();
}
