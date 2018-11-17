#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QPainter>

class Experiment {
private:
    Params *params;
    Task *task;
    QVector<Individual *> individuals;
    Pool pool;
    unsigned int currentGen;
    unsigned int t;
    unsigned int tMax;
    int selected;
public:
    Experiment();
    ~Experiment();
    Individual *getIndividual(int i);
    void newGen();
    void resetGen();
    void newMap();
    void stepAll();
    void evaluateGen();
    void draw(QPainter *painter);
    void drawNet(QPainter *painter);

    unsigned int getPopSize() {return params->popSize;}
    unsigned int getTMax() {return tMax;}
    unsigned int getCurrentGen() {return currentGen;}
    unsigned int getT() {return t;}
    int getSelected() {return selected;}
    void setSelected(int i) {selected = i;}
};

#endif // EXPERIMENT_H
