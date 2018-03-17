#include "experiment.h"

Generation::Generation() {

}

Generation::Generation(unsigned int popSize) :
    pop(QVector<Task::Individual>(popSize))
{

}

Experiment::Experiment() :
    popSize(8)
{
    task = new TestTask;
    gens.append(Generation(8));
}

Experiment::~Experiment() {
    delete task;
}
