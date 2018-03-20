#include "experiment.h"

Generation::Generation() :
    popSize(0),
    pop(QVector<Individual>(0))
{

}

Generation::Generation(unsigned int g_popSize) :
    popSize(g_popSize),
    pop(QVector<Individual>(g_popSize))
{
    pop[g_popSize - 1].x = 15;
}

Experiment::Experiment(unsigned int e_popSize, unsigned int e_inputSize, unsigned int e_outputSize) :
    popSize(e_popSize),
    task(e_inputSize, e_outputSize),
    gens(QVector<Generation>(0, Generation(e_popSize)))
{
    gens.append(Generation(e_popSize));
}
