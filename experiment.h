#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QItemSelectionModel>
#include "task_racing.h"

class Generation {
private:
    const unsigned int popSize;
public:
    Generation();
    Generation(unsigned int g_popSize, Task *task);
    QVector<RacingIndividual> pop;
};

class Experiment {
private:
    unsigned int popSize;
    unsigned int tMax;

    unsigned int currentGen;
    unsigned int t;
    int selected;
    RacingTask task;
    QVector<Generation> gens;
public:
    Experiment(unsigned int e_popSize);
    RacingIndividual *getIndividual(int i);
    void stepAll();
    void resetGen();
    void evaluateGen();
    void newGen();
    void draw(QPainter *painter);
    unsigned int getPopSize();
    unsigned int getTMax();
    unsigned int getCurrentGen();
    unsigned int getT();
    int getSelected();
    void setSelected(int i);
    Generation getGen(int i);
};

#endif // EXPERIMENT_H
