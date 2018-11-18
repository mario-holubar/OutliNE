#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QPainter>

class Experiment {
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
    bool immediateEvaluation = true;

    Experiment();
    ~Experiment();
    Individual *getIndividual(int i);
    void nextGen();
    void resetGen();
    void changePool(ParamDialog *d);
    void newPool();
    void changeTask(ParamDialog *d);
    void randomizeTask();
    void stepAll();
    void evaluateGen();
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
};

#endif // EXPERIMENT_H
