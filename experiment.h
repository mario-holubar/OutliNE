#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"

class Generation {
public:
    unsigned int popSize;
    Generation();
    Generation(unsigned int g_popSize);
    QVector<Individual> pop;
};

class Experiment {
public:
    unsigned int popSize;
    Experiment(unsigned int e_popSize, unsigned int e_inputSize, unsigned int e_outputSize);
    Task task;
    QVector<Generation> gens;
};

#endif // EXPERIMENT_H
