#include "mainwindow.h"
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
    QLocale::setDefault(QLocale::c());

    thread = new QThread;
    experiment->moveToThread(thread);
    connect(experiment, SIGNAL(updateView()), this, SLOT(updateViews()), Qt::BlockingQueuedConnection);
    connect(experiment, SIGNAL(updatePerformance(unsigned, float, float)), this, SLOT(updatePerformance(unsigned, float, float)), Qt::BlockingQueuedConnection);
    thread->start();

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(step()));

    initMenu();
    initConnections();
    initViews();

    newPool();

    /*for (int a = 0; a < 3; a++) {
        emit experiment_changeNE(a);
        ui->combobox_alg->setCurrentIndex(a);
        for (int i = 0; i < 16; i++) {
            experiment_nextGen();
        }
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
    proxyModel->setDynamicSortFilter(true);
    ui->tableView->setModel(proxyModel);
    proxyModel->sort(1, Qt::DescendingOrder);
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(setSelected(const QItemSelection &)));
    ui->tableView->setColumnWidth(0, 75);
    ui->tableView->setColumnWidth(1, 75);

    // Performance View
    chart = new QChart;
    chart->setBackgroundVisible(false);

    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->layout()->setContentsMargins(0, 0, 5, 5);
    //chart->legend()->hide();
    chart->legend()->setLabelColor(Qt::gray);
    chart->legend()->setContentsMargins(0, 0, 0, 0);

    xAxis = new QValueAxis;
    yAxis = new QValueAxis;
    xAxis->setLabelsColor(Qt::gray);
    yAxis->setLabelsColor(Qt::gray);
    xAxis->setRange(0, 1);
    yAxis->setRange(0, 10);
    xAxis->setTickType(QValueAxis::TicksDynamic);
    xAxis->setTickInterval(5);
    xAxis->setLabelFormat("%d");
    xAxis->setGridLineColor(QColor(64, 64, 64, 128));
    xAxis->setMinorTickCount(4);
    xAxis->setMinorGridLineColor(QColor(64, 64, 64, 32));
    //xAxis->setShadesBorderColor(QColor(0, 0, 0, 0));
    //xAxis->setShadesColor(QColor(0, 0, 0, 8));
    //xAxis->setShadesVisible(true);
    yAxis->setTickType(QValueAxis::TicksDynamic);
    yAxis->setTickInterval(10);
    yAxis->setGridLineColor(QColor(64, 64, 64, 128));
    yAxis->setMinorTickCount(1);
    yAxis->setMinorGridLineColor(QColor(64, 64, 64, 32));
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    for (unsigned i = 0; i < algs.size(); i++) {
        QLineSeries *perf = new QLineSeries;
        perf->setName(ui->combobox_alg->itemText(int(i)));
        perf->append(0, 0.0);
        performanceMax.append(perf);
        chart->addSeries(perf);
        perf->attachAxis(xAxis);
        perf->attachAxis(yAxis);
    }
    for (unsigned i = 0; i < algs.size(); i++) {
        QLineSeries *perf2 = new QLineSeries;
        //perf2->setName(ui->combobox_alg->itemText(int(i)));
        perf2->setColor(performanceMax[int(i)]->color());
        perf2->append(0, 0.0);
        performanceAvg.append(perf2);
        chart->addSeries(perf2);
        chart->legend()->markers(perf2)[0]->setVisible(false);
        perf2->attachAxis(xAxis);
        perf2->attachAxis(yAxis);
    }
    for (unsigned i = 0; i < algs.size(); i++) maxGen.append(0);

    ui->performanceView->setChart(chart);
    resizeDocks({ui->performance}, {256}, Qt::Horizontal);
    resizeDocks({ui->performance}, {128}, Qt::Vertical);
}

MainWindow::~MainWindow() {
    delete ui;
    delete experiment;
    delete timer;
    delete instanceTableModel;
    delete proxyModel;
    thread->quit();
    thread->wait();
    delete thread;
    delete chart;
    performanceMax.clear();
    performanceAvg.clear();
    maxGen.clear();
    delete xAxis;
    delete yAxis;
}

void MainWindow::updateViews() {
    ui->mainView->update();
    ui->netView->update();
    ui->progressBar->setMaximum(int(experiment->getTMax()));
    ui->progressBar->setValue(int(experiment->getT()));
    updateInstanceTable();
    if (ui->checkbox_trackFirst->isChecked()) {
        experiment->setSelected(proxyModel->mapToSource(proxyModel->index(0, 1)).row());
        ui->mainView->following = true;
    }
}

void MainWindow::updatePerformance(unsigned gen, float fitnessMax, float fitnessAvg) {
    int alg = int(experiment->alg);
    QLineSeries *perf = performanceMax[alg];
    QLineSeries *perf2 = performanceAvg[alg];
    if (gen > maxGen[alg]) {
        perf->append(gen, double(fitnessMax));
        perf2->append(gen, double(fitnessAvg));
        maxGen[alg] = gen;
    }
    else {
        perf->replace(gen, perf->at(int(gen)).y(), gen, double(fitnessMax));
        perf2->replace(gen, perf2->at(int(gen)).y(), gen, double(fitnessAvg));
    }
    if (gen > xAxis->max()) xAxis->setMax(gen);
    if (double(fitnessMax) > yAxis->max()) yAxis->setMax(ceil(double(fitnessMax) / 10) * 10);
}

void MainWindow::updateInstanceTable() {
    instanceTableModel->fitness.resize(int(experiment->getPopSize()));
    for (int i = 0; i < int(experiment->getPopSize()); i++) {
        instanceTableModel->fitness[i] = experiment->getIndividual(i)->getFitness();
    }
    proxyModel->invalidate();
    ui->tableView->repaint();
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
        ui->mainView->following = true;
    }
    ui->mainView->centerOnSelected();
    updateViews();
}

void MainWindow::nextGen() {
    ui->mainView->following = false;
    emit experiment_nextGen();
    //ui->tableView->clearSelection();
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
}

void MainWindow::queueTaskDialog() {
    emit experiment_changeTask();
}

void MainWindow::makeTaskDialog() {
    ParamDialog d(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    experiment->taskparams->paramDialog(&d);
    experiment->taskChanged = d.exec();
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
