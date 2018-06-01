#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QItemSelectionModel>

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

private:
    int selected;

public:
    Experiment(unsigned int e_popSize);
    Task task;
    QVector<Generation> gens;
    Individual *getIndividual(int i);
    void stepAll();
    void resetGen();
    void evaluateGen();
    void newGen();
    void draw(QPainter *painter);
    void setSelected(int i);
};

#endif // EXPERIMENT_H
