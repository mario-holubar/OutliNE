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
    SANEParams *params;
    Params *taskparams;
    Task *task;
    QVector<Individual *> individuals;
    Pool *pool;
    unsigned currentGen;
    unsigned t;
    int selected;
public:
    Experiment();
    ~Experiment();
    Individual *getIndividual(int i);
    //void evaluateGen();
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
    void makePoolDialog();
    void makeTaskDialog();
    void updateView();
public slots:
    void stepAll();
    void nextGen();
    void resetGen();
    void queuePoolDialog();
    void changePool(ParamDialog *);
    void newPool();
    void queueTaskDialog();
    void changeTask(ParamDialog *);
    void randomizeTask();
    void evaluateGen();
    void queueViewUpdate(); //
};

#endif // EXPERIMENT_H
