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
    task.createScene(&scene, popSize);
}

void Experiment::updateAll() {
    QList<QGraphicsItem *> items = scene.items();
    for (unsigned int i = 0; i < popSize; i++) {
        task.update(gens[0].pop.data() + i, items[i]);
    }
}
