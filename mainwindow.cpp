#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <mainview.h>

#include <tinyann.hpp>
#include <tinyneat.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      experiment(64) {
    ui->setupUi(this);

    initMenu();
    initConnections();
    initViews();

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(update()));
    play = false;

    newGen();

    srand(time(NULL));
    neat::pool p(2, 2, 1, false);
    for (int i = 0; i < 8; i++) {
        qDebug() << p.species.size();
        unsigned int max_fitness = 0;
        for (auto s = p.species.begin(); s != p.species.end(); s++) {
            for (size_t i = 0; i < (*s).genomes.size(); i++) {
                ann::neuralnet n;
                neat::genome& g = (*s).genomes[i];
                n.from_genome(g);

                std::vector<double> input(2, 0.0);
                std::vector<double> output(2, 0.0);
                unsigned int fitness = 0;

                RacingIndividual *a = experiment.getIndividual(i);
                for (int t = 0; t < experiment.getTMax() / 8; t++) {
                    input[0] = a->speed;
                    input[1] = a->angle / 180.0f;
                    n.evaluate(input, output);
                    //qDebug() << input[0] << input[1];
                    //qDebug() << output[0] << output[1];
                    //qDebug() << "";
                    a->step(output);
                }
                fitness = a->getFitness();

                if (fitness > max_fitness) max_fitness = fitness;
                g.fitness = fitness;
            }
        }
        qDebug() << "Generation" << i + 1 << ":" << max_fitness;
        p.new_generation();
        experiment.newGen();
    }
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

    ui->progressBar->setMaximum(experiment.getTMax());

    instanceTableModel = new InstanceModel(this, experiment.getPopSize());
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
    ui->progressBar->setValue(experiment.getT());
    updateInstanceTable();
}

void MainWindow::updateInstanceTable() {
    for (unsigned int i = 0; i < experiment.getPopSize(); i++) {
        instanceTableModel->fitness[i] = experiment.getGen(experiment.getCurrentGen()).pop[i].getFitness();
    }
    proxyModel->invalidate();
    ui->tableView->repaint();
}

void MainWindow::newGen() {
    experiment.newGen();
    ui->tableView->clearSelection();
    updateInstanceTable();
    ui->progressBar->setValue(experiment.getT());
    if (play) playPause();
    ui->mainView->update();
}

void MainWindow::playPause() {
    play = !play;
    if (experiment.getT() >= experiment.getTMax()) play = false;
    if (play) {
        timer->start(1000.0f / 60.0f);
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
    ui->progressBar->setValue(experiment.getT());
}

void MainWindow::resetGen() {
    experiment.resetGen();
    updateInstanceTable();
    ui->mainView->update();
    ui->progressBar->setValue(experiment.getT());
}

void MainWindow::setSelected(const QItemSelection &selection) {
    if (selection.indexes().size() == 0) experiment.setSelected(-1);
    else {
        QItemSelection realSelection = proxyModel->mapSelectionToSource(selection);
        experiment.setSelected(realSelection.indexes().at(0).row());
    }
    ui->mainView->update();
}
