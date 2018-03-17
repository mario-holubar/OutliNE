#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"

class Generation {
public:
    Generation();
    Generation(unsigned int popSize);
    QVector<Task::Individual> pop;
};

class Experiment {
public:
    Experiment();
    ~Experiment();
    Task *task;
    const unsigned int popSize;

    QVector<Generation> gens;
};

#endif // EXPERIMENT_H
