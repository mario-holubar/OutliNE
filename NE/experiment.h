#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QPainter>
#include "QMutex"

class EvaluationWorker {
public:
    EvaluationWorker(Experiment *experiment);
    const Experiment *experiment;
    void evaluateGen();
};

class Experiment : public QObject {
    Q_OBJECT
private:
    Task *task;
    QVector<Individual *> individuals;
    Pool *pool;
    unsigned currentGen;
    unsigned t;
    int selected;
public:
    PoolParams *params;
    TaskParams *taskparams;
    bool poolChanged;
    bool taskChanged;

    Experiment();
    ~Experiment();
    Individual *getIndividual(int i);
    void makeGenomes();
    void draw(QPainter *painter);
    QColor activationColorNeuron(double lerp);
    QColor activationColorWeight(double v, double a);
    void drawNet(QPainter *painter);

    unsigned getPopSize() {return params->n_genomes;}
    unsigned getTMax() {return params->tMax;}
    unsigned getCurrentGen() {return currentGen;}
    unsigned getT() {return t;}
    int getSelected() {return selected;}
    void setSelected(int i) {selected = i;}
signals:
    void requestPoolDialog();
    void requestTaskDialog();
    void updateView();
public slots:
    void stepAll();
    void evaluateGen();
    void nextGen();
    void resetGen();
    void changePool();
    void newPool();
    void changeTask();
    void randomizeTask();
};

#endif // EXPERIMENT_H
