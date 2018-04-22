#include "experiment.h"

Generation::Generation() :
    popSize(0),
    pop(QVector<Individual>(0)) {

}

Generation::Generation(unsigned int g_popSize) :
    popSize(g_popSize),
    pop(QVector<Individual>(g_popSize)) {

}

Experiment::Experiment(unsigned int e_popSize) :
    popSize(e_popSize),
    gens(QVector<Generation>(1, Generation(e_popSize))) {

}

void Experiment::updateAll() {
    //QVector<float> inputs(2, 1.0f);
    //Individual *i = (Individual *)(gens.at(0).pop.begin());
    //task.update(i);

    /*for (QMutableVectorIterator<Individual> i(gens[0].pop); i.hasNext(); i.next()) { //maybe don't use vectors at all? (constant size)
        task.update(i);
    }*/

    for (unsigned int i = 0; i < popSize; i++) {
        task.update(gens[0].pop.data() + i);
    }
}
