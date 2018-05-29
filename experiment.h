#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QGraphicsScene>

class Generation {
public:
    const unsigned int popSize;
    Generation();
    Generation(unsigned int g_popSize);
    QVector<Individual> pop;
};

class Experiment {
public:
    unsigned int popSize;
    unsigned int tMax;

    unsigned int currentGen;
    unsigned int t;

    Experiment(unsigned int e_popSize);
    ~Experiment();
    Task task;
    QVector<Generation> gens;
    QGraphicsScene *scene;
    void stepAll(bool updateGraphics);
    void evaluateGen();
    void newGen();
};

#endif // EXPERIMENT_H
